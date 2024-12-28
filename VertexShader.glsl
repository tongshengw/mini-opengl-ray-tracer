#version 410 core

in vec4 position;

uniform mat4 u_ModelMatrix;
uniform mat4 u_CameraMatrix;

void main() {
    gl_Position = vec4(position.x, position.y, position.z, position.w);
}
