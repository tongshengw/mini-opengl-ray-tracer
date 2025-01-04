#version 410 core

in vec4 gl_FragCoord;
out vec4 color;

uint rand(uint seed) {
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

void main() {
    float randseed = 
    color = vec4(rand(randseed), rand(randseed), rand(randseed), 1.0f);
}
