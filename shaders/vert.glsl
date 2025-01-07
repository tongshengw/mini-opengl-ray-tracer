#version 410 core

in vec4 position;

uniform mat4 u_PerspectiveMatrix;

void main() {
    gl_Position = u_PerspectiveMatrix * vec4(position.x, position.y, position.z, position.w);
}
