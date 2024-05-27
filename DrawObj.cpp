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
			Vector3 ndcAVertex = Transform(a + sphere.center, viewProjectionMatrix);
			Vector3 ndcBVertex = Transform(b + sphere.center, viewProjectionMatrix);
			Vector3 ndcCVertex = Transform(c + sphere.center, viewProjectionMatrix);
			Vector3 screenA = Transform(ndcAVertex, viewportMatrix);
			Vector3 screenB = Transform(ndcBVertex, viewportMatrix);
			Vector3 screenC = Transform(ndcCVertex, viewportMatrix);
			Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenB.x), int(screenB.y), color);
			Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenC.x), int(screenC.y), color);
		}
	}
}

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 center = Multiply(plane.normal, plane.distance);
	Vector3 perpendiculars[4];
	perpendiculars[0] = Normalize(Parpendicular(plane.normal));
	perpendiculars[1] = -perpendiculars[0];
	perpendiculars[2] = Cross(plane.normal, perpendiculars[0]);
	perpendiculars[3] = -perpendiculars[2];

	Vector3 points[4];
	for (int32_t index = 0; index < 4; ++index) {
		Vector3 extend = Multiply(2.0f, perpendiculars[index]);
		Vector3 point = Add(center, extend);
		points[index] = Transform(Transform(point, viewProjectionMatrix), viewportMatrix);
	}

	Novice::DrawLine(int(points[0].x), int(points[0].y), int(points[2].x), int(points[2].y), color);
	Novice::DrawLine(int(points[0].x), int(points[0].y), int(points[3].x), int(points[3].y), color);
	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[2].x), int(points[2].y), color);
	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[3].x), int(points[3].y), color);
}

void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 points[3];
	for (int32_t index = 0; index < 3; ++index) {
		points[index] = Transform(Transform(triangle.vertixces[index], viewProjectionMatrix), viewportMatrix);
	}
	Novice::DrawTriangle(int(points[0].x), int(points[0].y), int(points[1].x), int(points[1].y), int(points[2].x), int(points[2].y), color, kFillModeWireFrame);
}