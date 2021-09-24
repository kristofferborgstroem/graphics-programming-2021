#version 330 core

out vec4 fragColor;
in float age;

// TODO 2.6: should receive the age of the particle as an input variable
float map(float value, float min1, float max1, float min2, float max2);
void main()
{
    // TODO 2.4 set the alpha value to 0.2 (alpha is the 4th value of the output color)
    // TODO 2.5 and 2.6: improve the particles appearance
    float dist = sqrt((gl_PointCoord.x - 0.5) * (gl_PointCoord.x - 0.5) + (gl_PointCoord.y - 0.5) * (gl_PointCoord.y - 0.5));

    float opacity = clamp(map(dist, 0.0, 1.0, 1.0, -3.0), 0.0, 1.0) / age;
    // remember to replace the default output (vec4(1.0,1.0,1.0,1.0)) with the color and alpha values that you have computed
    fragColor = mix(vec4(1.0, 1.0, 0.5, opacity), vec4(0.0, 0.0, 1.0, opacity), age / 10.0);

}

float map(float value, float min1, float max1, float min2, float max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}