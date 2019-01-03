#version 330 core
in vec3 vNormal;
out vec4 fragColor;


uniform bool bLighting;
uniform vec4 color;


void main()
{
    fragColor = color * normalize(vNormal).z;
}
