#version 420 core

flat in vec3 colorsExport;

out vec4 colorsOut;

void main(void)
{
   colorsOut = vec4(colorsExport, 1.0);
}