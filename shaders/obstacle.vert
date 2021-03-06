#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 fragNormal;
out vec2 vPos;


uniform mat4 projection, modelview;
uniform mat3 normalMatrix;
uniform vec3 translation = vec3(0.0);


void main()
{
        vPos = vec2(position.x + translation.x, position.z + translation.z);
        gl_Position = projection * modelview * vec4(position + translation, 1.0);
        fragNormal = normalMatrix * normal;
}

