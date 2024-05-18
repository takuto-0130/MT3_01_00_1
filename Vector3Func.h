#pragma once
#include "Vector3.h"

Vector3 Cross(const Vector3& v1, const Vector3& v2);

float Length(const Vector3& v);

Vector3 Normalize(const Vector3& v);

float Dot(const Vector3& v1, const Vector3& v2);

Vector3 Subtruct(const Vector3& v1, const Vector3& v2);

Vector3 Add(const Vector3& v1, const Vector3& v2);

const Vector3 Multiply(const Vector3& v1, const Vector3& v2);
const Vector3 Multiply(const Vector3& v, const float& s);
const Vector3 Multiply(const float& s, const Vector3& v);

Vector3 operator-(const Vector3& a, const Vector3& b);

Vector3 operator-(const Vector3& a);


Vector3 operator+(const Vector3& v1, const Vector3& v2);