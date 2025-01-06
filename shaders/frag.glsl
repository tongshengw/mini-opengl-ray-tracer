#version 410 core

in vec4 gl_FragCoord;
out vec4 color;

uniform vec3 u_CameraPos;
uniform vec3 u_CameraDir;
uniform vec3 u_LightPos;
uniform float u_LightRadius;
uniform vec3 u_SphereLocations[3];
uniform float u_SphereRadii[3];
uniform int u_RandSeed;

struct Ray {
    vec3 direction;
    vec3 origin;
};

struct rayIntersect {
    bool exists;
    float dst;
    vec3 pos;
    vec3 normal;
};

uint rand(uint seed) {
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html
rayIntersect intersectSphere(Ray ray, vec3 sphereLocation, float sphereRadius) {
    rayIntersect intersect;
    intersect.exists = false;
    ray.direction = normalize(ray.direction);
    float a = 1;
    float b = 2 * dot(ray.origin, ray.direction);
    float c = dot(ray.origin, ray.origin) * dot(ray.direction, ray.direction);

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return intersect;
    } else {
        float solution1 = (-b + sqrt(discriminant)) / (2);
        float solution2 = (-b - sqrt(discriminant)) / (2);
        float solution;
        if (solution1 < 0 && solution2 < 0) {
            return intersect;
        } else if (solution1 < 0) {
            solution = solution2;
        } else if (solution2 < 0) {
            solution = solution1;
        } else {
            float solution = min(solution1, solution2);
        }
        intersect.exists = true;
        intersect.dst = solution;
        intersect.pos = intersect.dst * ray.direction + ray.origin;
        intersect.normal = normalize(intersect.pos - sphereLocation);
        return intersect;
    }
}

void main() {
    // uint randseed = gl_FragCoord.x * gl_FragCoord.y * u_RandSeed;
    
    Ray ray;
    ray.direction = vec3((2/1280) * gl_FragCoord.x - 1, (2*0.5625/720) * gl_FragCoord.y - 0.5625, -1);
    ray.direction = normalize(ray.direction);
    ray.origin = vec3(0.0f, 0.0f, 0.0f);

    float minIntersectDistance = 1000;
    float intersectedSphereIndex = -1;

    for (int i = 0; i < 3; i++) {
        rayIntersect currentIntersect = intersectSphere(ray, u_SphereLocations[i], u_SphereRadii[i]);
        if (currentIntersect.exists && currentIntersect.dst < minIntersectDistance) {
            minIntersectDistance = currentIntersect.dst;
            intersectedSphereIndex = i;
        }
    }

    if (intersectedSphereIndex != -1) {
        color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    } else {
        color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
}
