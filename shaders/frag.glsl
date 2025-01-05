#version 410 core

in vec4 gl_FragCoord;
out vec4 color;

uniform vec3 u_CameraPos;
uniform vec3 u_CameraDir;
uniform vec3 u_LightPos;
uniform float u_SphereLocations[3];
uniform float u_SphereRadii[3];

uint rand(uint seed) {
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

vec3 normalise(vec3 input) {
    float invLength = 1/sqrt(input.x * input.x + input.y * input.y + input.z * input.z);

    return {input.x * invLength, input.y * invLength, input.z * invLength};
}

bool intersect(vec3 ray, float sphereLocation, float sphereRadius) {
    
}

void main() {
    uint randseed = uint(gl_FragCoord.x * gl_FragCoord.y);

    vec3 ray = {(2/1280) * gl_FragCoord.x - 1, (2*0.5625/720) * gl_FragCoord.y - 0.5625, -1};

    for (int i = 0; i < 3; i++) {
        
    }

    

    color = vec4(gl_FragCoord.x/1280, gl_FragCoord.y/720, 0.0f, 1.0f);
}
