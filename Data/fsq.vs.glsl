#version 330

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexcoord;

// out vec4 gl_Position //position is a required output, so we don't need to define it
out vec2 texcoord;

void main() {
    gl_Position = vec4(inPosition, 1.f);
    texcoord = inTexcoord;
}