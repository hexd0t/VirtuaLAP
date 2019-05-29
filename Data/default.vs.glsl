#version 330

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexcoord;

// out vec4 gl_Position //position is a required output, so we don't need to define it
out vec2 texcoord;
/*out vec3 position_world;//world space
out vec3 normal_cam;	//camera space
out vec3 eyedir_cam;	//vector towards the camera in camera space
out vec3 lightdir_cam;	//vector towards light in camera space*/

uniform mat4 model;
uniform mat3 normal;	//transpose(inverse(view * model)
uniform mat4 view;
uniform mat4 proj;
/*uniform vec3 lightPos;	//world space*/

void main() {

    gl_Position = vec4( inPosition, 1.0f );
    vec4 position_world = model * vec4( inPosition, 1.0f );
    vec4 position_cam = view * position_world;
    vec4 position_screen = proj * position_cam;

    //normal_cam = normal * inNormal;
    //see http://www.lighthouse3d.com/tutorials/glsl-tutorial/the-normal-matrix/ why this is different from view * model

    /*eyedir_cam = vec3(0,0,0) - position_cam.xyz;
    lightdir_cam = (view * vec4( lightPos, 1.0f ) ).xyz - position_cam.xyz;*/

    texcoord = inTexcoord;
    gl_Position = position_screen;

}