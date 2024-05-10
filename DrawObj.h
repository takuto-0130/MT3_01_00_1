#pragma once
#include "ObjStruct.h"
#include "DrawObj.h"
#include "Matrix4x4Func.h"
#include "Vector3Func.h"
#include <Novice.h>

const int kWindowWidth = 1280;
const int kWindowHeight = 720;

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);
