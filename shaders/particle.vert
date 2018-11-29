#version 330 core
layout (location = 0) in vec3 position;
layout (location = 0) in vec2 value;


//out vec2 TexCoords;
out vec4 ParticleColor;
out float r;
out vec2 val;

uniform mat4 projection, modelview;
uniform vec4 color;
uniform vec3 particle_pos;
uniform float radius;

void main()
{
    //    gl_Position = projection * modelview* vec4(particle_pos, 1.0);

    vec3 quad_position = position;

    float scale = 2.0f;
    //print the standard quad in the position given by the particle position, so project this position
    //add the quad position to the particle position, without modelview, in order to face always the camera
//    gl_Position.xyz += vec4(projection * vec4(quad_position, 1.0)).xyz ;

//    gl_Position = projection * modelview* vec4(particle_pos, 1.0);
//    gl_Position.xyz += vec4(vec4(quad_position, 1.0)).xyz ;

    gl_Position = projection * modelview* vec4(particle_pos, 1.0)  + vec4(projection*vec4(quad_position, 0.0));


    ParticleColor = vec4(abs(gl_Position.x),abs(gl_Position.y),abs(gl_Position.z),1.0);

    r = radius;
    val = value;

}

