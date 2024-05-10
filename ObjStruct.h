#pragma once
#include "Vector3.h"

struct Sphere {
	Vector3 center;
	float radius;
};

struct Line {
	Vector3 origine;
	Vector3 diff;
};

struct Ray {
	Vector3 origine;
	Vector3 diff;
};

struct Segment {
	Vector3 origine;
	Vector3 diff;
};