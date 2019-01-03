#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 projection, modelview;
uniform mat3 normalMatrix;
uniform vec3 space_pos;
uniform mat4 rotation;

out vec3 vNormal;

void main()
{
    vec4 pos = rotation * vec4(position,1.0);
    gl_Position = projection * modelview * (pos + vec4(space_pos, 0.0));
    vNormal = normalMatrix * normal;
}

