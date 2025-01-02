#version 410 core

in vec4 position;

uniform mat4 u_CameraMatrix;

void main() {
    gl_Position = u_CameraMatrix * vec4(position.x, position.y, position.z, position.w);
}
