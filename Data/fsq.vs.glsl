#version 330

//Full Screen Quad Shader
//Just use the full Vertex format - it's less hassle not to register a second one just for this
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexcoord;

// out vec4 gl_Position //position is a required output, so we don't need to define it
out vec2 texcoord;

void main() {
    gl_Position = vec4(inPosition, 1.f);
    texcoord = inTexcoord;
}