#version 330 core
layout (location = 0) in vec2 pos;   // the position variable has attribute position 0
// TODO 2.2 add velocity and timeOfBirth as vertex attributes
layout (location = 1) in vec2 velocity;
layout (location = 2) in float timeOfBirth;

// TODO 2.3 create and use a float uniform for currentTime

uniform float currentTime;
out float age;

// TODO 2.6 create out variable to send the age of the particle to the fragment shader

void main()
{
    // TODO 2.3 use the currentTime to control the particle in different stages of its lifetime

    age = currentTime - timeOfBirth;
    if (timeOfBirth == 0) {
        gl_Position = vec4(100.0, 100.0, 0.0, 1.0);
    }
    else {
        if (age > 10.0) {
            gl_Position = vec4(100.0, 100.0, 0.0, 1.0);
        } else {
            vec2 location = vec2(pos.x + age*velocity.x, pos.y + age * velocity.y - pow(age / 5, 2));
            gl_Position = vec4(location, 0.0, 1.0);
        }


        // TODO 2.6 send the age of the particle to the fragment shader using the out variable you have created

        // this is the output position and and point size (this time we are rendering points, instad of triangles!)
    }
    gl_PointSize = mix(10.0, 200.0, age / 20.0);
}