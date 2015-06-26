#version 330 core

//The location corresponds to the value passed into the corresponding calls to glVertexAttribPointer
layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_col;
layout(location = 3) in vec2 in_uv;

uniform mat4 ModelViewProjection;

//Declare VS_OUT as an output interface block
out VS_OUT {
    vec3 color;
    vec2 uv;
} vs_out;

void main() {
    //Simply pass the color to the fragment shader
    vs_out.color = in_col;
    vs_out.uv = in_uv;
    
    //Tranform the vertex position into a homogenous 4D vector
    vec4 vert = vec4(in_pos, 1.0);
    
    //Output the position
    gl_Position = ModelViewProjection * vert;
}
