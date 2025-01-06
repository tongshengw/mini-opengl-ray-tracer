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

vec3 normalise(vec3 input) {
    float invLength = 1/sqrt(input.x * input.x + input.y * input.y + input.z * input.z);

    return {input.x * invLength, input.y * invLength, input.z * invLength};
}

// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html
rayIntersect intersectSphere(Ray ray, float sphereLocation, float sphereRadius) {
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
    intersect.pos = ray.origin + t0 * ray.origin;
    intersect.normal = intersect.pos - sphereLocation;
    intersect.dst = t0;
    return intersect;
}

void main() {
    uint randseed = uint(gl_FragCoord.x * gl_FragCoord.y * u_RandSeed);
    
    Ray ray;
    ray.direction = {(2/1280) * gl_FragCoord.x - 1, (2*0.5625/720) * gl_FragCoord.y - 0.5625, -1};
    ray.origin = {0.0f, 0.0f, 0.0f};

    for (int rayBounceCount = 0; rayBounceCount < 5; rayBounceCount++) {


        float minIntersectDistance = -1;
        float intersectedSphereIndex = -1;

        for (int i = 0; i < 3; i++) {
            rayIntersect currentIntersect = intersect(ray, u_SphereLocations[i], u_SphereRadii[i]);
            if ((minIntersectDistance == -1 && currentIntersect.exists) || (currentIntersect.exists && currentIntersect.dst < minIntersectDistance)) {
                minIntersectDistance = currentIntersect;
                intersectedSphereIndex = i;
            }

        }

        if (minIntersectDistance > 0) {
            color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        } else {
            color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
    color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
