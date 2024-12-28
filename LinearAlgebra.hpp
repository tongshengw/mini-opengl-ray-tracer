#ifndef LINALG_H
#define LINALG_H

#include <array>
#include <cmath>
#include <iostream>

template<typename T>
class v4; // Forward declaration

template<typename T>
class v3 {
public:
    v3()
        :x(0), y(0), z(0) {}

    v3(T x, T y, T z) 
	    :x(x), y(y), z(z) {}

    v3(v4<T> input)
        : v3(input.x, input.y, input.z) {}

    bool operator==(const v3<T> &rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    v3<T> operator+(v3<T> rhs) {
        return v3{x+rhs.x, y+rhs.y, z+rhs.z};
    }

    v3<T> operator-(v3<T> rhs) {
        return v3{x-rhs.x, y-rhs.y, z-rhs.z};
    }

    v3<T> operator-() {
        return v3{-x, -y, -z};
    }

    v3<T> operator*(float rhs) {
        return v3{x*rhs, y*rhs, z*rhs};
    }

    T dot(const v3<T> &rhs) const {
        return x*rhs.x + y*rhs.y + z*rhs.z;
    }

    v3<T> cross(const v3<T> &rhs) const {
        return v3<T>{
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x
        };
    }

    T x;
    T y;
    T z;

};

template<typename T>
v3<T> operator*(float scalar, v3<T> vector) {
    return vector*scalar;
}

template<typename T>
class v4 {
public:

    v4(T x, T y, T z, T w)
        : x(x), y(y), z(z), w(w) {}

    v4()
        : v4(0, 0, 0, 1) {}

    v4(T x, T y, T z) 
        : v4(x, y, z, w) {}

    bool operator==(const v4<T> &rhs) {
	return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }

    T x;
    T y;
    T z;
    T w;
};

/*
|a, b, c, d|
|e, f, g, h|
|i, j, k, l|
*/
template<typename T>
class m44 {
private:
    std::array<T, 16> data;
    
public:
    // Default constructor - identity matrix
    m44() {
        for(int i = 0; i < 16; i++) {
            data[i] = (i % 5 == 0) ? 1 : 0;  // 1's on diagonal, 0's elsewhere
        }
    }

    // Constructor with 16 elements
    m44(const std::array<T, 16>& elements)
        : data(elements) {}

    // Access operators (row, col)
    T& operator()(size_t row, size_t col) {
        return data[row * 4 + col];
    }

    const T& operator()(size_t row, size_t col) const {
        return data[row * 4 + col];
    }

    // Array access operator
    T& operator[](size_t index) {
        return data[index];
    }

    const T& operator[](size_t index) const {
        return data[index];
    }

private:
    //Matrix multiplication
    m44<T> multiply(const m44<T>& rhs) const {
        std::array<T, 16> result;
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                T sum = 0;
                for(int k = 0; k < 4; k++) {
                    sum += (*this)(i,k) * rhs(k,j);
                }
                result[i * 4 + j] = sum;
            }
        }
        return m44<T>(result);
    }

    // Vector multiplication
    v4<T> multiply(const v4<T>& rhs) const {
        T x = (*this)(0,0) * rhs.x + (*this)(0,1) * rhs.y + (*this)(0,2) * rhs.z + (*this)(0,3) * rhs.w;
        T y = (*this)(1,0) * rhs.x + (*this)(1,1) * rhs.y + (*this)(1,2) * rhs.z + (*this)(1,3) * rhs.w;
        T z = (*this)(2,0) * rhs.x + (*this)(2,1) * rhs.y + (*this)(2,2) * rhs.z + (*this)(2,3) * rhs.w;
        T w = (*this)(3,0) * rhs.x + (*this)(3,1) * rhs.y + (*this)(3,2) * rhs.z + (*this)(3,3) * rhs.w;
        return v4<T>(x, y, z, w);
    }

    // Operator overloads
    m44<T> operator*(const m44<T>& rhs) const { return multiply(rhs); }

    v4<T> operator*(const v4<T>& rhs) const { return multiply(rhs); }
};

class Quaternion {
public:
    float r;
    float i;
    float j;
    float k;

    Quaternion(float r, float i, float j, float k)
        : r(r), i(i), j(j), k(k) {
        enforce_normalisation();
    }

    Quaternion()
        : Quaternion(1, 0, 0, 0) {}
    
    Quaternion(const std::array<float, 3> &imaginary, float theta)
        : Quaternion(sin(theta/2), imaginary[0]*cos(theta/2), imaginary[1]*cos(theta/2), imaginary[2]*cos(theta/2)) {}

    void normalise() {
        float length = sqrt(pow(r, 2)+pow(i, 2)+pow(j, 2)+pow(k, 2));
        r /= length;
        i /= length;
        j /= length;
        k /= length;
    }

    float magnitude() const {
        return sqrt(pow(r, 2)+pow(i, 2)+pow(j, 2)+pow(k, 2));
    }

    Quaternion conjugate() const {
        return Quaternion({r, -i, -j, -k});
    }

    Quaternion operator*(const Quaternion &rhs) {
        return Quaternion({
            r * rhs.r - i * rhs.i - j * rhs.j - k * rhs.k,
            r * rhs.i + i * rhs.r + j * rhs.k - k * rhs.j,
            r * rhs.j - i * rhs.k + j * rhs.r + k * rhs.i,
            r * rhs.k + i * rhs.j - j * rhs.i + k * rhs.r
        });
    }

    void enforce_normalisation() {
        if (magnitude() >= 1.001 || magnitude() <0.999) {
            normalise();
            std::cout << "WARNING: initialised non-normalised quaternion, quaternion automatically normalised" << std::endl;
        }
    }

    std::pair<v3<float>, float> get_rotation_axis_angle() {
        enforce_normalisation();

        std::pair<v3<float>, float> output;
        float theta = asin(r);
        output.second = theta;

        output.first = {i/cos(theta), j/cos(theta), k/cos(theta)};

        return output;
    }

    v3<float> rotate_vector(v3<float> rhs) {
        Quaternion p{0, rhs.x, rhs.y, rhs.z};
        Quaternion result = (*this) * p * conjugate();
        return {result.i, result.j, result.k}; 
    }
};

#endif
