#include "Vector3Func.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <assert.h>

Vector3 Cross(const Vector3& v1, const Vector3& v2)
{
    return Vector3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

Vector3 Project(const Vector3& v1, const Vector3& v2) {
    Vector3 v2Normalize = Normalize(v2);
    float dot = Dot(v1, v2Normalize);
    Vector3 result{ v2Normalize.x * dot, v2Normalize.y * dot, v2Normalize.z * dot };
    return result;
};

Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
    Vector3 project = Project(point - segment.origine, segment.diff);
    return { segment.origine.x + project.x, segment.origine.y + project.y, segment.origine.z + project.z };
};

Vector3 Parpendicular(const Vector3& vector) {
    if (vector.x != 0 || vector.y != 0) {
        return { -vector.y,vector.x, 0.0f };
    }
    return { 0.0f,-vector.z,vector.y };
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
    return { a.x * -1.0f,a.y * -1.0f,a.z * -1.0f };
}

Vector3 operator+(const Vector3& v1, const Vector3& v2) {
    return Add(v1, v2);
}

Vector3 operator*(const Vector3& v, const float& s) {
    return Multiply(v, s);
}

Vector3 operator*(const float& s, const Vector3& v) {
    return Multiply(v, s);
}

Vector3 operator/(const Vector3& v, const float& s)
{
    return Vector3(v.x/s,v.y/s,v.z/s);
}

float Lerp(const float& a, const float& b, float t) {
    return a + t * (b - a);
}

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) {
    return v1 + t * (v2 - v1);
}

Vector3 Bezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, float t) {
    Vector3 p0p1 = Lerp(p0, p1, t);
    Vector3 p1p2 = Lerp(p1, p2, t);
    Vector3 p = Lerp(p0p1, p1p2, t);
    return p;
}

Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
    float s = 0.5f;		// 除算が重いので1/2の代用
    float t2 = t * t;	// tの2乗
    float t3 = t2 * t;	// tの3乗

    Vector3 e3 = -p0 + 3 * p1 - 3 * p2 + p3;
    Vector3 e2 = 2 * p0 - 5 * p1 + 4 * p2 - p3;
    Vector3 e1 = -p0 + p2;
    Vector3 e0 = 2 * p1;

    return s * (e3 * t3 + e2 * t2 + e1 * t + e0);
}

Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t) {
    assert(points.size() >= 4);
    size_t division = points.size() - 1;
    float areaWidth = 1.0f / division;
    float t_2 = std::fmod(t, areaWidth) * division;
    t_2 = std::clamp(t_2, 0.0f, 1.0f);

    size_t index = static_cast<size_t>(t / areaWidth);
    index = (std::min)(index, division - 1l);

    size_t index0 = index - 1;
    size_t index1 = index;
    size_t index2 = index + 1;
    size_t index3 = index + 2;
    if (index == 0) {
        index0 = index1;
    }
    if (index3 >= points.size()) {
        index3 = index2;
    }
    const Vector3& p0 = points[index0];
    const Vector3& p1 = points[index1];
    const Vector3& p2 = points[index2];
    const Vector3& p3 = points[index3];
    return CatmullRomInterpolation(p0, p1, p2, p3, t_2);
}

Vector3 Reflect(const Vector3& input, const Vector3& normal) {
    return input - (2.0f * Dot(input, normal) * normal);
}
