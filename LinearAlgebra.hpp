#ifndef LINALG_H
#define LINALG_H

template<typename T>
class v3 {
public:
    v3() :x(0), y(0), z(0) {}

    v3(T x, T y, T z) 
	:x(x), y(y), z(z) {}

    bool operator==(const v3<T> &rhs) {
	return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    T x;
    T y;
    T z;
};

class Triangle {
public:
    Triangle(v3<float> a, v3<float> b, v3<float> c)
	:a(a), b(b), c(c) {}
    v3<float> a;
    v3<float> b;
    v3<float> c;
};

#endif
