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
    vec3 L = sphereLocation - ray.origin;
    float tca = dot(L, ray.direction);
    if (tca < 0) {
        intersect.exists = false;
        return intersect;
    }
    float d2 = dot(L, L) - tca * tca;
    if (d2 > sphereRadius * sphereRadius) {
        intersect.exists = false;
        return intersect;
    }
    float thc = sqrt(sphereRadius * sphereRadius - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;

    if (t0 > t1) {
        float tmp = t0;
        t0 = t1;
        t1 = tmp;
    }
    if (t0 < 0) {
        t0 = t1;
        if (t0 < t1) {
            intersect.exists = false;
            return intersect;
        }
    }
    intersect.exists = true;
    intersect.pos = ray.origin + t0 * ray.direction;
    intersect.normal = intersect.pos - sphereLocation;
    intersect.dst = t0;
    return intersect;
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
