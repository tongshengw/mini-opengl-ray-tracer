#ifndef LINALG_H
#define LINALG_H

template<typename T>
class v3 {
public:
    v3(T x, T y, T z) 
	:x(x), y(y), z(z) {}

    bool operator==(const v3<T> &rhs) {
	return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    T x;
    T y;
    T z;
};

#endif
