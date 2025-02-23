#include "LinearAlgebra.hpp"
#include <cassert>
#include <iostream>

bool floatEqual(float l, float r) {
    if (l <= r+0.001 && l >= r-0.001) {
        return true;
    }
    return false;
}

int main() {
    v3<float> f1{0,1,0};
    Quaternion q1{{1,0,0}, 3.141/2};
    v3<float> f2 = q1.rotate_vector(f1);
    assert(floatEqual(0, f2.x));
    assert(floatEqual(0, f2.y));
    assert(floatEqual(1, f2.z));
    std::cout << "tests successful" << std::endl;
}
