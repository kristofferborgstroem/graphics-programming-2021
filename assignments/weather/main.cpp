#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <math.h>
#include "shader.h"
#include "glmutils.h"
#include "primitives.h"
// structure to hold render info
// -----------------------------
struct SceneObject{
    unsigned int VAO;
    unsigned int vertexCount;
    void drawSceneObject() const{
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES,  vertexCount, GL_UNSIGNED_INT, 0);
    }
};

struct Particles {
    unsigned int VAO, VBO;
    unsigned int vertexCount;
    void draw() const{
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, vertexCount * 3);
    }
};
// function declarations
// ---------------------
unsigned int createArrayBuffer(const std::vector<float> &array);
unsigned int createElementArrayBuffer(const std::vector<unsigned int> &array);
unsigned int createVertexArray(const std::vector<float> &positions, const std::vector<float> &colors, const std::vector<unsigned int> &indices);
unsigned int createParticleVAO(const std::vector<float> &positions);
std::vector<float> setupParticles();
void setup();
void setView();
void drawObjects();
void drawParticles();
// glfw and input functions
// ------------------------
void cursorInRange(float screenX, float screenY, int screenW, int screenH, float min, float max, float &x, float &y);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void cursor_input_callback(GLFWwindow* window, double posX, double posY);
void drawCube(glm::mat4 model);
// screen settings
// ---------------
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;
// global variables used for rendering
// -----------------------------------
SceneObject cube;
SceneObject floorObj;
Particles particles;
Shader* shaderProgram;
Shader* particleShader;
// global variables used for control
// ---------------------------------

// Camera stuff
float currentTime;
glm::vec3 camForward(.0f, .0f, -1.0f);
glm::vec3 camPosition(.0f, 1.6f, 0.0f);
float linearSpeed = 0.15f, rotationGain = 30.0f;
glm::vec2 cursorPos = glm::vec2(0.0f);
float yaw = 0.0f;
float pitch = 0.0f;
glm::mat4 viewProjection = glm::mat4(1.f);
glm::mat4 viewProjectionPrev = glm::mat4(1.f);

// Particle Stuff
const unsigned int particleSize = 6;
const unsigned int numParticles = 1000;
const unsigned int vertexBufferSize = 65536;
const unsigned int boxSize = 10;
glm::vec3 offset = glm::vec3(0);
glm::vec3 gravity = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 forwardOffset = glm::vec3(0);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Exercise 5.2", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_input_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // setup mesh objects
    // ---------------------------------------
    setup();
    // set up the z-buffer
    // Notice that the depth range is now set to glDepthRange(-1,1), that is, a left handed coordinate system.
    // That is because the default openGL's NDC is in a left handed coordinate system (even though the default
    // glm and legacy openGL camera implementations expect the world to be in a right handed coordinate system);
    // so let's conform to that
    glDepthRange(-1,1); // make the NDC a LEFT handed coordinate system, with the camera pointing towards +z
    glEnable(GL_DEPTH_TEST); // turn on z-buffer depth test
    glDepthFunc(GL_LESS); // draws fragments that are closer to the screen in NDC
    // render loop
    // -----------
    // render every loopInterval seconds
    float loopInterval = 0.02f;
    auto begin = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window))
    {
        // update current time
        auto frameStart = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> appTime = frameStart - begin;
        currentTime = appTime.count();
        processInput(window);
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        // notice that we also need to clear the depth buffer (aka z-buffer) every new frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderProgram->use();

        viewProjectionPrev = viewProjection;
        setView();
        drawObjects();


        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
        particleShader->use();
        drawParticles();
        //glDisable(GL_BLEND);

        glfwSwapBuffers(window);
        glfwPollEvents();
        // control render loop frequency
        std::chrono::duration<float> elapsed = std::chrono::high_resolution_clock::now()-frameStart;
        while (loopInterval > elapsed.count()) {
            elapsed = std::chrono::high_resolution_clock::now() - frameStart;
        }
    }
    delete shaderProgram;
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void setView() {
    glm::mat4 scale = glm::scale(1.f, 1.f, 1.f);
    glm::mat4 projection = glm::perspectiveFov(70.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, .01f, 10.0f);
    glm::mat4 view = glm::lookAt(camPosition, camPosition + camForward, glm::vec3(0,1,0));
    viewProjection = projection * view;
}

void drawObjects(){
    glm::mat4 scale = glm::scale(1.f, 1.f, 1.f);

    // draw floor (the floor was built so that it does not need to be transformed)
    shaderProgram->setMat4("model", viewProjection);
    floorObj.drawSceneObject();
    // draw 2 cubes in different locations and with different orientations
    drawCube(viewProjection * glm::translate(2.0f, 1.f, 2.0f) * glm::rotateY(glm::half_pi<float>()) * scale);
    drawCube(viewProjection * glm::translate(-2.0f, 1.f, -2.0f) * glm::rotateY(glm::quarter_pi<float>()) * scale);
}

void drawParticles () {
    gravity += glm::vec3(0.0f, -0.3f, 0.0f);
    forwardOffset = camForward;
    offset = gravity - (camPosition + forwardOffset + glm::vec3(boxSize / 2.0));
    offset = glm::mod(offset, glm::vec3(boxSize));
    particleShader->setMat4("viewProj", viewProjection);
    particleShader->setMat4("viewProjPrev", viewProjectionPrev);
    particleShader->setVec3("camPos", camPosition);
    particleShader->setVec3("offset", offset);
    particleShader->setFloat("boxSize", boxSize);
    particles.draw();
}


void drawCube(glm::mat4 model){
    // draw object
    shaderProgram->setMat4("model", model);
    cube.drawSceneObject();
}

void setup(){
    // initialize shaders
    shaderProgram = new Shader("shaders/shader.vert", "shaders/shader.frag");
    particleShader = new Shader("shaders/rain.vert", "shaders/rain.frag");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // load floor mesh into openGL
    floorObj.VAO = createVertexArray(floorVertices, floorColors, floorIndices);
    floorObj.vertexCount = floorIndices.size();
    // load cube mesh into openGL
    cube.VAO = createVertexArray(cubeVertices, cubeColors, cubeIndices);
    cube.vertexCount = cubeIndices.size();

    // load particles
    particles.VAO = createParticleVAO(setupParticles());
    particles.vertexCount = numParticles * particleSize;
}

// VAO, VBO, EBO solid objects
unsigned int createVertexArray(const std::vector<float> &positions, const std::vector<float> &colors, const std::vector<unsigned int> &indices){
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    // bind vertex array object
    glBindVertexArray(VAO);
    // set vertex shader attribute "pos"
    createArrayBuffer(positions); // creates and bind  the VBO
    int posAttributeLocation = glGetAttribLocation(shaderProgram->ID, "pos");
    glEnableVertexAttribArray(posAttributeLocation);
    glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
    // set vertex shader attribute "color"
    createArrayBuffer(colors); // creates and bind the VBO
    int colorAttributeLocation = glGetAttribLocation(shaderProgram->ID, "color");
    glEnableVertexAttribArray(colorAttributeLocation);
    glVertexAttribPointer(colorAttributeLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);
    // creates and bind the EBO
    createElementArrayBuffer(indices);
    return VAO;
}
unsigned int createArrayBuffer(const std::vector<float> &array){
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, array.size() * sizeof(GLfloat), &array[0], GL_STATIC_DRAW);
    return VBO;
}
unsigned int createElementArrayBuffer(const std::vector<unsigned int> &array){
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, array.size() * sizeof(unsigned int), &array[0], GL_STATIC_DRAW);
    return EBO;
}

// VAO, VBO particles
unsigned int createParticleVAO(const std::vector<float> &positions) {
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    createArrayBuffer(positions);
    int posAttributeLocation = glGetAttribLocation(particleShader->ID, "pos");
    glEnableVertexAttribArray(posAttributeLocation);
    glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
    return VAO;
}

std::vector<float> setupParticles() {
    std::vector<float> array = std::vector<float>(numParticles * particleSize);
    for (unsigned int i = 0; i < numParticles; i++) {
        float x = (float) rand() / (float) RAND_MAX * boxSize;
        float y = (float) rand() / (float) RAND_MAX * boxSize;
        float z = (float) rand() / (float) RAND_MAX * boxSize;

        for (int j = 0; j < 2; ++j) {
            array.push_back(x);
            array.push_back(y);
            array.push_back(z);
        }
    }
    return array;
}

// Receive Input
void cursorInRange(float screenX, float screenY, int screenW, int screenH, float min, float max, float &x, float &y){
    float sum = max - min;
    float xInRange = (float) screenX / (float) screenW * sum - sum/2.0f;
    float yInRange = (float) screenY / (float) screenH * sum - sum/2.0f;
    x = xInRange;
    y = -yInRange; // flip screen space y axis
}
void cursor_input_callback(GLFWwindow* window, double posX, double posY){

    int w, h;
    glfwGetWindowSize(window, &w, &h);

    glm::vec2 cursorLast = cursorPos;
    cursorInRange(posX, posY, w, h, 0.0f, 1.0f, cursorPos.x, cursorPos.y);

    float xoffset = cursorPos.x - cursorLast.x;
    float yoffset = cursorPos.y - cursorLast.y;

    float sensitivity = rotationGain;
    yaw += xoffset * sensitivity;
    pitch += yoffset * sensitivity;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camForward = glm::normalize(front);
}
void processInput(GLFWwindow *window) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    glm::vec3 forward = camForward * linearSpeed;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camPosition += glm::vec3(forward.x, 0.0f, forward.z);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camPosition -= glm::vec3(forward.x, 0.0f, forward.z);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camPosition -= glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f))) * linearSpeed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camPosition += glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f))) * linearSpeed;
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}