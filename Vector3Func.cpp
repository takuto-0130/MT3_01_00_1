#include "Vector3Func.h"
#define _USE_MATH_DEFINES
#include <cmath>

Vector3 Cross(const Vector3& v1, const Vector3& v2)
{
    return Vector3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

float Length(const Vector3& v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector3 Normalize(const Vector3& v) {
    float len = Length(v);
    Vector3 result{ v.x / len, v.y / len, v.z / len };
    return result;
}

float Dot(const Vector3& v1, const Vector3& v2) {
    return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

Vector3 Subtruct(const Vector3& v1, const Vector3& v2) {
    return{ v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

Vector3 Add(const Vector3& v1, const Vector3& v2) {
    return{ v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

const Vector3 Multiply(const Vector3& v1, const Vector3& v2) {
    return{ v1.x * v2.x, v1.y * v2.y, v1.z * v2.z };
}

const Vector3 Multiply(const Vector3& v, const float& s) {
    return{ v.x * s, v.y * s, v.z * s };
}

const Vector3 Multiply(const float& s, const Vector3& v) {
    return{ v.x * s, v.y * s, v.z * s };
}

Vector3 operator-(const Vector3& v1, const Vector3& v2) {
    return Subtruct(v1, v2);
}

Vector3 operator-(const Vector3& a) {
    return { -a.x,-a.y,-a.z };
}

Vector3 operator+(const Vector3& v1, const Vector3& v2) {
    return Add(v1, v2);
}
