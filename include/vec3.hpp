#ifndef VEC3_HPP
#define VEC3_HPP
#include <iostream>
#include <string>

class vec3 {
public:
    float x, y, z;
    
    vec3();
    vec3(float x, float y, float z);

    float norm();
    float norm_squared();
    vec3 normalized();
    void operator+=(vec3 o);
    void operator/=(float t);
    float operator[](int i);
};

float dot(vec3 a, vec3 b);
vec3 lerp(vec3 a, vec3 b, float t);
vec3 reflect(vec3 v, vec3 n);
vec3 cross(vec3 a, vec3 b);
vec3 operator+(vec3 a, vec3 b);
vec3 operator*(float t, vec3 v);
vec3 operator-(vec3 a, vec3 b);
vec3 operator-(vec3 a);
vec3 operator/(vec3 v, float t);
std::ostream& operator<<(std::ostream& ostream, vec3 v);
#endif
