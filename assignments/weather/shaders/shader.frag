#version 330 core
out vec4 FragColor;
in  vec4 vtxColor;
void main()
{
   FragColor = vec4(vtxColor.r, vtxColor.g * 0, vtxColor.b * 0, vtxColor.a);
}