#version 330 core

in vec3 fragNormal;
in vec2 vPos;
uniform float radius;
uniform bool obstacle = false;
uniform vec3 translation;

out vec4 fragColor;

uniform vec4 color;


void main()
{
    if(obstacle == true)
    {
        vec2 center = vec2(translation.x, translation.z);
        float dist = distance(center,vPos);
        if(dist > radius)
            discard;
    }
    fragColor = color * normalize(fragNormal).z;
    fragColor = color;

}
