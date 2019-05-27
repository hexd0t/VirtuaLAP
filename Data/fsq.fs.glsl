#version 330

// Properties the Vertex Shader supplied
in vec2 texcoord;
out vec4 endColor;
uniform sampler2D diffuseTextureSampler;

void main(void) {
    endColor = texture( diffuseTextureSampler, texcoord );
}