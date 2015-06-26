#version 330 core

out vec4 outputColor;
uniform sampler2D samp;

//Declare VS_OUT as an input interface block
in VS_OUT {
    vec3 color;
    vec2 uv;
} fs_in;

void main() {
    //Set the current fragment's color to the vec3 received from the vertex shader
    outputColor = texture(samp, fs_in.uv);
    //outputColor = vec4(fs_in.color, 1.0);
}
