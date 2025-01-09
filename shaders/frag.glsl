#version 410 core

in vec4 gl_FragCoord;
out vec4 color;

struct Sphere {
    vec3 location;
    float radius;
    vec3 color;
    float emmission;
};

uniform Sphere u_Spheres[3];
uniform uint u_RandSeed;


struct Ray {
    vec3 direction;
    vec3 origin;
    vec3 color;
    vec3 brightness;
};

struct RayIntersect {
    bool exists;
    float dst;
    vec3 pos;
    vec3 normal;
    Ray new_ray;
};

uint TausStep(uint state) {
    uint b = (((state << 13) ^ state) >> 19);
    state = (((state & 4294967294u) << 12) ^ b);
    return state;
}

// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html
RayIntersect intersectSphere(Ray ray, Sphere sphere) {
    RayIntersect intersect;
    intersect.exists = false;
    ray.direction = normalize(ray.direction);
    float a = 1;
    float b = 2 * dot(ray.origin-sphere.location, ray.direction);
    float c = dot(ray.origin-sphere.location, ray.origin-sphere.location) - sphere.radius * sphere.radius;

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
        intersect.normal = normalize(intersect.pos - sphere.location);
        intersect.new_ray = ray;
        intersect.new_ray.color = ray.color * sphere.color;
        return intersect;
    }
}

RayIntersect findClosestIntersect(Ray ray) {
    float minIntersectDistance = 1000;
    int closestSphereIndex = -1;
    RayIntersect minIntersect;
    minIntersect.exists = false;

    for (int i = 0; i < 3; i++) {
        RayIntersect currentIntersect = intersectSphere(ray, u_Spheres[i]);
        if (currentIntersect.exists && currentIntersect.dst < minIntersectDistance) {
            closestSphereIndex = i;
            minIntersectDistance = currentIntersect.dst;
            minIntersect = currentIntersect;
        }
    }
    return minIntersect;
}

vec3 randomDirection(uint randomSeed) {
    bool found = false;
    uint prevSeed;
    while(!found) {
        uint r1 = TausStep(prevSeed);
        prevSeed = r1;
        uint r2 = TausStep(prevSeed);
        prevSeed = r2;
        uint r3 = TausStep(prevSeed);
        prevSeed = r3;

        float f1 = r1 * (1.0/float(0xffffffffu));
        float f2 = r2 * (1.0/float(0xffffffffu));
        float f3 = r3 * (1.0/float(0xffffffffu));

        vec3 cur = vec3(f1, f2, f3);
        if (f1*f1 + f2*f2 + f3*f3 <= 1) {
            vec3 output = normalize(cur);
            found = true;
            return output;
        }
    }
}

void main() {
    uint frameSeed = uint(gl_FragCoord.x * gl_FragCoord.y + gl_FragCoord.y);
    uint currentSeed = frameSeed + u_RandSeed;
    
    currentSeed = TausStep(currentSeed);
    float curRand = currentSeed * (1.0 / float(0xffffffffu));
    
    
    Ray ray;
    ray.direction = vec3((2.0/1280) * gl_FragCoord.x - 1, (2*0.5625/720) * gl_FragCoord.y - 0.5625, -1);
    ray.direction = normalize(ray.direction);
    ray.origin = vec3(0.0f, 0.0f, 0.0f);
    ray.color = vec3(0.0f, 0.0f, 0.0f);

    for (int bounce = 0; bounce < 5; bounce++) {
        RayIntersect currentIntersect = findClosestIntersect(ray);
        if (currentIntersect.exists) {
            ray = currentIntersect.new_ray;
        }
    }

    color = vec4(ray.color.x, ray.color.y, ray.color.z, 1.0f);
}
