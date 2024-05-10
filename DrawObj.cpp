#include "DrawObj.h"

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);

	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		Vector3 start = { -kGridEvery * float(kSubdivision / 2.0f), 0, (-kGridEvery * float(kSubdivision / 2.0f)) + (kGridEvery * float(xIndex)) };
		Vector3 end = { kGridEvery * float(kSubdivision / 2.0f), 0, start.z };
		Vector3 ndcStartVertex = Transform(start, viewProjectionMatrix);
		Vector3 ndcEndVertex = Transform(end, viewProjectionMatrix);
		Vector3 screenStart = Transform(ndcStartVertex, viewportMatrix);
		Vector3 screenEnd = Transform(ndcEndVertex, viewportMatrix);
		if (xIndex == kSubdivision / 2) {
			Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), BLACK);
		}
		else {
			Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), WHITE);
		}
	}

	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		Vector3 start = { (-kGridEvery * float(kSubdivision / 2.0f)) + (kGridEvery * float(zIndex)) , 0, -kGridEvery * float(kSubdivision / 2.0f) };
		Vector3 end = { start.x, 0, kGridEvery * float(kSubdivision / 2.0f) };
		Vector3 ndcStartVertex = Transform(start, viewProjectionMatrix);
		Vector3 ndcEndVertex = Transform(end, viewProjectionMatrix);
		Vector3 screenStart = Transform(ndcStartVertex, viewportMatrix);
		Vector3 screenEnd = Transform(ndcEndVertex, viewportMatrix);
		if (zIndex == kSubdivision / 2) {
			Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), BLACK);
		}
		else {
			Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), WHITE);
		}
	}
}


void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 16;
	const float kLonEvery = float(M_PI) * 2.0f / kSubdivision;
	const float kLatEvery = float(M_PI) / kSubdivision;

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex;
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;
			Vector3 a = { (cosf(lat) * cosf(lon)) * sphere.radius, (sinf(lat)) * sphere.radius, (cosf(lat) * sinf(lon)) * sphere.radius };
			Vector3 b = { (cosf(lat + kLatEvery) * cosf(lon)) * sphere.radius, (sinf(lat + kLatEvery)) * sphere.radius, (cosf(lat + kLatEvery) * sinf(lon)) * sphere.radius };
			Vector3 c = { (cosf(lat) * cosf(lon + kLonEvery)) * sphere.radius, (sinf(lat)) * sphere.radius, (cosf(lat) * sinf(lon + kLonEvery)) * sphere.radius };
			Vector3 ndcAVertex = Transform(a, viewProjectionMatrix);
			Vector3 ndcBVertex = Transform(b, viewProjectionMatrix);
			Vector3 ndcCVertex = Transform(c, viewProjectionMatrix);
			Vector3 screenA = Transform(ndcAVertex, viewportMatrix);
			Vector3 screenB = Transform(ndcBVertex, viewportMatrix);
			Vector3 screenC = Transform(ndcCVertex, viewportMatrix);
			Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenB.x), int(screenB.y), color);
			Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenC.x), int(screenC.y), color);
		}
	}
}