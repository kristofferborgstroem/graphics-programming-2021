#version 330 core

uniform vec3 camPosition; // so we can compute the view vector
out vec4 FragColor; // the output color of this fragment

// TODO exercise 8.4 setup the 'uniform' variables needed for lighting
// light uniforms
uniform vec3 ambientLightColor;
uniform vec3 light1Position;
uniform vec3 light1Color;
uniform vec3 light2Position;
uniform vec3 light2Color;

// material properties
uniform vec3 reflectionColor;
uniform float ambientReflectance;
uniform float diffuseReflectance;
uniform float specularReflectance;
uniform float specularExponent;

// attenuation uniforms
uniform float attenuationC0;
uniform float attenuationC1;
uniform float attenuationC2;

// TODO exercise 8.4 add the 'in' variables to receive the interpolated Position and Normal from the vertex shader
in vec3 P_frag;
in vec3 N_frag;


void main()
{

   // TODO exercise 8.4 - phong shading (i.e. Phong reflection model computed in the fragment shader)
   // Ambient
   vec3 ambient = ambientLightColor * ambientReflectance * reflectionColor;
   vec4 color = vec4(ambient,1);

   // diffuse component for light 1
   vec3 L = normalize(light1Position - P_frag);
   float diffuseModulation = max(dot(N_frag, L), 0.0);
   vec3 diffuse = light1Color * diffuseReflectance * diffuseModulation * reflectionColor;

   // specular component for light 1
   vec3 R = 2 * N_frag * dot(N_frag, L) - L;; // the same as reflect(-L_eye, normal)
   float specModulation = pow(max(dot(R, normalize(camPosition - P_frag)), 0.0), specularExponent);
   vec3 specular = light1Color * specularReflectance * specModulation;


   float dist = length(P_frag - light1Position);
   float att = 1.0 / (attenuationC0 + attenuationC1 * dist + attenuationC2 * dist * dist);
   color.xyz += (diffuse + specular) * att;

   // TODO exercise 8.5 - multiple lights, compute diffuse and specular of light 2
   // Diffuse
   L = normalize(light2Position - P_frag);
   diffuseModulation = max(dot(N_frag, L), 0.0);
   diffuse = light2Color * diffuseReflectance * diffuseModulation * reflectionColor;

   // Specular
   specModulation = pow(max(dot(R, normalize(camPosition - P_frag)), 0.0), specularExponent);
   specular = light2Color * specularReflectance * specModulation;

   dist = length(P_frag - light2Position);
   att = 1.0 / (attenuationC0 + attenuationC1 * dist + attenuationC2 * dist * dist);
   color.xyz += (diffuse + specular) * att;


   // TODO compute the final shaded color (e.g. add contribution of the (attenuated) lights 1 and 2)


   // TODO set the output color to the shaded color that you have computed
   FragColor = color;
}
// you might have noticed that the shading contribution of multiple lights can fit a for loop nicely
// we will be doing that later on