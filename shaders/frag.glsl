#version 410 core

in vec4 gl_FragCoord;
out vec4 color;

struct Sphere {
    vec3 location;
    float radius;
    vec3 color;
};

// uniform vec3 u_CameraPos;
// uniform vec3 u_CameraDir;
uniform Sphere u_Spheres[3];
uniform int u_RandSeed;

struct Ray {
    vec3 direction;
    vec3 origin;
    vec3 color;
};

struct RayIntersect {
    bool exists;
    float dst;
    vec3 pos;
    vec3 normal;
    Ray new_ray;
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
RayIntersect intersectSphere(Ray ray, Sphere sphere) {
    vec3 sphereLocation = sphere.location;
    float sphereRadius = sphere.radius;

    RayIntersect intersect;
    intersect.exists = false;
    ray.direction = normalize(ray.direction);
    float a = 1;
    float b = 2 * dot(ray.origin-sphereLocation, ray.direction);
    float c = dot(ray.origin-sphereLocation, ray.origin-sphereLocation) - sphereRadius * sphereRadius;

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
        intersect.new_ray = ray;
        intersect.new_ray.color = ray.color + sphere.color;
        return intersect;
    }
}

rayIntersect findClosestIntersect(Ray ray) {
    float minIntersectDistance = 1000;
    int closestSphereIndex = -1;
    rayIntersect minIntersect;
    minIntersect.exists = false;

    for (int i = 0; i < 3; i++) {
        rayIntersect currentIntersect = intersectSphere(ray, u_Spheres[i]);
        if (currentIntersect.exists && currentIntersect.dst < minIntersectDistance) {
            closestSphereIndex = i;
            minIntersectDistance = currentIntersect.dst;
            minIntersect = currentIntersect;
        }
    }
    return minIntersect;
}

void main() {
    // uint randseed = gl_FragCoord.x * gl_FragCoord.y * u_RandSeed;
    
    Ray ray;
    ray.direction = vec3((2.0/1280) * gl_FragCoord.x - 1, (2*0.5625/720) * gl_FragCoord.y - 0.5625, -1);
    ray.direction = normalize(ray.direction);
    ray.origin = vec3(0.0f, 0.0f, 0.0f);

    rayIntersect currentIntersect = findClosestIntersect(ray);

    color = currentIntersect.new_ray.color;
}
