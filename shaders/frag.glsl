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

vec3 randomDirection(uint randomSeed) {
    uint prevSeed = randomSeed;
    const int MAX_ITERATIONS = 100;
    
    for(int i = 0; i < MAX_ITERATIONS; i++) {
        uint r1 = TausStep(prevSeed);
        prevSeed = r1;
        uint r2 = TausStep(prevSeed);
        prevSeed = r2;
        uint r3 = TausStep(prevSeed);
        prevSeed = r3;

        // Map to [-1, 1] range consistently
        float x = r1 * (2.0/float(0xffffffffu)) - 1.0;
        float y = r2 * (2.0/float(0xffffffffu)) - 1.0;
        float z = r3 * (2.0/float(0xffffffffu)) - 1.0;

        vec3 point = vec3(x, y, z);
        float lenSq = dot(point, point);
        
        // Check if point is inside unit sphere and not at origin
        if (lenSq <= 1.0 && lenSq > 0.0001) {
            return normalize(point);
        }
    }
    
    // Default fallback direction if no valid direction found
    return vec3(0.0, 1.0, 0.0);
}

// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html
RayIntersect intersectSphere(Ray ray, Sphere sphere, uint randomSeed) {
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
        intersect.new_ray.direction = randomDirection(randomSeed);
        if (dot(intersect.new_ray.direction, intersect.normal) < 0.0) {
            intersect.new_ray.direction = -intersect.new_ray.direction;
        }
        intersect.new_ray.origin = intersect.pos;
        
        // Set new ray properties
        // intersect.new_ray.brightness = ray.brightness * 0.5;  // Light falloff
        intersect.new_ray.color = 0.5 * ray.color;   // Surface color interaction
        
        // Add emission contribution
        // if (sphere.emmission > 0.0) {
            // intersect.new_ray.color += 0.5 * ray.color;
            // intersect.new_ray.brightness += sphere.emmission;
        // }
        
        return intersect;
    }
}

RayIntersect findClosestIntersect(Ray ray, uint randomSeed) {
    float minIntersectDistance = 1000;
    int closestSphereIndex = -1;
    RayIntersect minIntersect;
    minIntersect.exists = false;

    for (int i = 0; i < 3; i++) {
        RayIntersect currentIntersect = intersectSphere(ray, u_Spheres[i], randomSeed);
        if (currentIntersect.exists && currentIntersect.dst < minIntersectDistance) {
            closestSphereIndex = i;
            minIntersectDistance = currentIntersect.dst;
            minIntersect = currentIntersect;
        }
    }
    return minIntersect;
}

void main() {
    uint frameSeed = uint(gl_FragCoord.x * gl_FragCoord.y + gl_FragCoord.y);
    uint currentSeed = frameSeed + u_RandSeed;
    
    Ray ray;
    ray.direction = vec3((2.0/1280) * gl_FragCoord.x - 1, (2*0.5625/720) * gl_FragCoord.y - 0.5625, -1);
    ray.direction = normalize(ray.direction);
    ray.origin = vec3(0.0f, 0.0f, 0.0f);
    ray.color = vec3(1.0f, 1.0f, 1.0f);
    ray.brightness = vec3(1.0f, 1.0f, 1.0f); // Start with full brightness

    vec3 finalColor = vec3(0.0f);
    for (int bounce = 0; bounce < 3; bounce++) {
        RayIntersect currentIntersect = findClosestIntersect(ray, currentSeed);
        if (currentIntersect.exists) {
            ray = currentIntersect.new_ray;
            // finalColor += ray.color * ray.brightness;  // Accumulate color weighted by brightness
            finalColor = ray.color;
        }
    }

    color = vec4(finalColor, 1.0f);
}
