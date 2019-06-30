#include "vec3.hpp"
#include <cmath>

vec3::vec3(float x, float y, float z): x(x), y(y), z(z)
{
}

vec3::vec3(): x(0.f), y(0.f), z(0.f)
{
}

float vec3::norm() 
{
    return sqrt(x*x+y*y+z*z);
}

float vec3::norm_squared() {
    return x*x + y*y + z*z;
}

vec3 vec3::normalized()
{
    return *this / norm();
}

void vec3::operator+=(vec3 v)
{
    x += v.x;
    y += v.y;
    z += v.z;
}

void vec3::operator/=(float t)
{
    x /= t;
    y /= t;
    z /= t;
}

vec3 operator+(vec3 a, vec3 b)
{
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

vec3 operator*(float t, vec3 v)
{
    return vec3(t * v.x, t * v.y, t * v.z);
}

float dot(vec3 a, vec3 b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

vec3 operator-(vec3 a, vec3 b)
{
    return a + (-b);
}

vec3 operator-(vec3 a)
{
    return vec3(-a.x, -a.y, -a.z);
}

vec3 operator/(vec3 a, float t)
{
    return (1.f/t) * a;
}

vec3 lerp(vec3 a, vec3 b, float t)
{
    return (1.f - t) * a + t * b;
}

std::ostream& operator<<(std::ostream& ostream, vec3 v)
{
    ostream << "vec3(" << v.x << "," << v.y << "," << v.z << ")";
    return ostream;
}
