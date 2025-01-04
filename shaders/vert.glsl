#version 410 core

in vec4 position;

uniform mat4 u_CameraMatrix;
uniform mat4 u_ModelMatrix;

void main() {
    gl_Position = u_CameraMatrix * u_ModelMatrix * vec4(position.x, position.y, position.z, position.w);
}
