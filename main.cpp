#include <Novice.h>
#include <imgui.h>
#include "DrawObj.h"
#include <algorithm>

bool IsCollision(const Sphere& sphere, const Plane& plane) {
	float distance = sqrtf((Dot(plane.normal, sphere.center) - plane.distance) * (Dot(plane.normal, sphere.center) - plane.distance));
	if (distance <= sphere.radius) {
		return true;
	}
	return false;
}

bool IsCollisionLinePlane(const Segment& segment, const Plane& plane) {
	float dot = Dot(segment.diff, plane.normal);
	if (dot == 0) {
		return false;
	}
	return true;
}

bool IsCollisionSegmentPlane(const Segment& segment, const Plane& plane) {
	float dot = Dot(segment.diff, plane.normal);
	if (dot == 0) {
		return false;
	}
	float t = (plane.distance - Dot(segment.origine, plane.normal)) / dot;
	if (t < 0 || t > 1) {
		return false;
	}
	return true;
}

bool IsCollisionSegmentTriangle(const Segment& segment, const Triangle& triangle) {
	Vector3 v01 = triangle.vertixces[1] - triangle.vertixces[0];
	Vector3 v12 = triangle.vertixces[2] - triangle.vertixces[1];
	Vector3 v20 = triangle.vertixces[0] - triangle.vertixces[2];

	Vector3 normal = Cross(v01, v12);
	float distance = Dot(triangle.vertixces[0], normal);
	Plane plane = {
		normal,
		distance
	};
	if (!(IsCollisionSegmentPlane(segment, plane))) {
		return false;
	}
	float dot = Dot(segment.diff, plane.normal);
	float t = (plane.distance - Dot(segment.origine, plane.normal)) / dot;
	Vector3 point = ((segment.diff) * t) + segment.origine;

	Vector3 v1p = point - triangle.vertixces[1];
	Vector3 v2p = point - triangle.vertixces[2];
	Vector3 v0p = point - triangle.vertixces[0];

	Vector3 corss01 = Cross(v01, v1p);
	Vector3 corss12 = Cross(v12, v2p);
	Vector3 corss20 = Cross(v20, v0p);

	if (Dot(corss01, normal) >= 0.0f &&
		Dot(corss12, normal) >= 0.0f &&
		Dot(corss20, normal) >= 0.0f) {
		return true;
	}
	return false;
}

bool IsCollisionAABB(const AABB& a, const AABB& b) {
	if ((a.min.x <= b.max.x && a.max.x >= b.min.x) &&
		(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
		(a.min.z <= b.max.z && a.max.z >= b.min.z)) {
		return true;
	}
	return false;
}

bool IsCollisionAABBSphere(const AABB& a, const Sphere& sphere) {
	Vector3 closestPoint = { std::clamp(sphere.center.x,a.min.x,a.max.x), std::clamp(sphere.center.y,a.min.y,a.max.y), std::clamp(sphere.center.z,a.min.z,a.max.z) };
	float distance = Length(closestPoint - sphere.center);
	if (distance <= sphere.radius) {
		return true;
	}
	return false;
}

bool IsCollisionAABBLine(const AABB& a, const Line& line) {
	if (line.diff.x == 0.0f && line.diff.y == 0.0f && line.diff.z == 0.0f) {
		return false;
	}
	//X軸
	float tMinX = (a.min.x - line.origine.x) / line.diff.x;

	float tMaxX = (a.max.x - line.origine.x) / line.diff.x;

	float tNearX = min(tMinX, tMaxX);
	float tFarX = max(tMinX, tMaxX);


	//Y軸
	float tMinY = (a.min.y - line.origine.y) / line.diff.y;

	float tMaxY = (a.max.y - line.origine.y) / line.diff.y;

	float tNearY = min(tMinY, tMaxY);
	float tFarY = max(tMinY, tMaxY);


	//Z軸
	float tMinZ = (a.min.z - line.origine.z) / line.diff.z;

	float tMaxZ = (a.max.z - line.origine.z) / line.diff.z;

	float tNearZ = min(tMinZ, tMaxZ);
	float tFarZ = max(tMinZ, tMaxZ);


	float tMin = max(max(tNearX, tNearY), tNearZ);

	float tMax = min(min(tFarX, tFarY), tFarZ);

	if (tMin <= tMax) {
		return true;
	}
	return false;
}


bool IsCollisionAABBSegment(const AABB& a, const Segment& segment) {
	if (segment.diff.x == 0.0f && segment.diff.y == 0.0f && segment.diff.z == 0.0f) {
		return false;
	}
	//X軸
	float tMinX = (a.min.x - segment.origine.x) / segment.diff.x;

	float tMaxX = (a.max.x - segment.origine.x) / segment.diff.x;

	float tNearX = min(tMinX, tMaxX);
	float tFarX = max(tMinX, tMaxX);


	//Y軸
	float tMinY = (a.min.y - segment.origine.y) / segment.diff.y;

	float tMaxY = (a.max.y - segment.origine.y) / segment.diff.y;

	float tNearY = min(tMinY, tMaxY);
	float tFarY = max(tMinY, tMaxY);


	//Z軸
	float tMinZ = (a.min.z - segment.origine.z) / segment.diff.z;

	float tMaxZ = (a.max.z - segment.origine.z) / segment.diff.z;

	float tNearZ = min(tMinZ, tMaxZ);
	float tFarZ = max(tMinZ, tMaxZ);


	float tMin = max(max(tNearX, tNearY), tNearZ);

	float tMax = min(min(tFarX, tFarY), tFarZ);

	if (tMin > tMax) {
		return false;
	}
	if ((tMin >= 0.0f && tMin <= 1.0f) || (tMax >= 0.0f && tMax <= 1.0f) ||
		(tMin <= 0.0f && tMax >= 1.0f)) {
		return true;
	}


	return false;
}

bool IsCollisionOBBSphere(const OBB& obb, const Sphere& sphere) {
	Matrix4x4 obbMat = {};
	obbMat.m[0][0] = obb.oriientations[0].x;
	obbMat.m[0][1] = obb.oriientations[0].y;
	obbMat.m[0][2] = obb.oriientations[0].z;

	obbMat.m[1][0] = obb.oriientations[1].x;
	obbMat.m[1][1] = obb.oriientations[1].y;
	obbMat.m[1][2] = obb.oriientations[1].z;

	obbMat.m[2][0] = obb.oriientations[2].x;
	obbMat.m[2][1] = obb.oriientations[2].y;
	obbMat.m[2][2] = obb.oriientations[2].z;

	obbMat.m[3][0] = obb.center.x;
	obbMat.m[3][1] = obb.center.y;
	obbMat.m[3][2] = obb.center.z;

	obbMat.m[3][3] = 1.0f;

	Matrix4x4 obbMatInverse = Inverse(obbMat);
	Vector3 centerInOBBLocalSpace = Transform(sphere.center, obbMatInverse);

	AABB aabbOBBLocal{ .min = -obb.size, .max = obb.size };
	Sphere sphereOBBLocal{ centerInOBBLocalSpace, sphere.radius };
	return IsCollisionAABBSphere(aabbOBBLocal, sphereOBBLocal);
}

bool IsCollisionOBBSegment(const OBB& obb, const Segment& segment) {
	Matrix4x4 obbMat = {};
	obbMat.m[0][0] = obb.oriientations[0].x;
	obbMat.m[0][1] = obb.oriientations[0].y;
	obbMat.m[0][2] = obb.oriientations[0].z;

	obbMat.m[1][0] = obb.oriientations[1].x;
	obbMat.m[1][1] = obb.oriientations[1].y;
	obbMat.m[1][2] = obb.oriientations[1].z;

	obbMat.m[2][0] = obb.oriientations[2].x;
	obbMat.m[2][1] = obb.oriientations[2].y;
	obbMat.m[2][2] = obb.oriientations[2].z;

	obbMat.m[3][0] = obb.center.x;
	obbMat.m[3][1] = obb.center.y;
	obbMat.m[3][2] = obb.center.z;

	obbMat.m[3][3] = 1.0f;

	Matrix4x4 obbMatInverse = Inverse(obbMat);
	Vector3 localOrigin = Transform(segment.origine, obbMatInverse);
	Vector3 localEnd = Transform(segment.origine + segment.diff, obbMatInverse);

	AABB aabbOBBLocal{ .min = -obb.size, .max = obb.size };
	Segment localSegment{ .origine{localOrigin}, .diff{localEnd - localOrigin} };
	return IsCollisionAABBSegment(aabbOBBLocal, localSegment);
}

void OBBVertex(const OBB& obb, Vector3* vertixces) {
	Vector3 sizeX = (obb.oriientations[0]) * obb.size.x;
	Vector3 sizeY = (obb.oriientations[1]) * obb.size.y;
	Vector3 sizeZ = (obb.oriientations[2]) * obb.size.z;
	vertixces[0] = -sizeX - sizeY - sizeZ + obb.center;
	vertixces[1] = -sizeX + sizeY - sizeZ + obb.center;
	vertixces[2] = sizeX - sizeY - sizeZ + obb.center;
	vertixces[3] = sizeX + sizeY - sizeZ + obb.center;
	vertixces[4] = -sizeX - sizeY + sizeZ + obb.center;
	vertixces[5] = -sizeX + sizeY + sizeZ + obb.center;
	vertixces[6] = sizeX - sizeY + sizeZ + obb.center;
	vertixces[7] = sizeX + sizeY + sizeZ + obb.center;
}

/// <summary>
/// 分離軸があるかどうか
/// </summary>
/// <param name="normal"> 正規化法線ベクトル </param>
/// <param name="vertixces1"> OBB1の頂点 </param>
/// <param name="vertixces2"> OBB2の頂点 </param>
/// <returns></returns>
bool isHST(const Vector3& normal, const  Vector3* vertixces1, const  Vector3* vertixces2) {
	float t[8];
	for (size_t i = 0; i < 8; i++) {
		t[i] = Dot(normal, vertixces1[i]);
	}
	float minT1 = t[0], maxT1 = t[0];
	for (size_t i = 1; i < 8; i++) {
		minT1 = (std::min)(t[i], minT1);
		maxT1 = (std::max)(t[i], maxT1);
	}
	float L1 = maxT1 - minT1;

	for (size_t i = 0; i < 8; i++) {
		t[i] = Dot(normal, vertixces2[i]);
	}
	float minT2 = t[0], maxT2 = t[0];
	for (size_t i = 1; i < 8; i++) {
		minT2 = (std::min)(t[i], minT2);
		maxT2 = (std::max)(t[i], maxT2);
	}
	float L2 = maxT2 - minT2;
	float sumSpan = L1 + L2;
	float longSpan = (std::max)(maxT1, maxT2) - (std::min)(minT1, minT2);
	if (sumSpan < longSpan) {
		return true;
	}


	return false;
}

bool IsCollisionOBBOBB(const OBB& obb1, const OBB& obb2) {
	Vector3 obb1Vertices[8]{};
	OBBVertex(obb1, obb1Vertices);
	Vector3 obb2Vertices[8]{};
	OBBVertex(obb2, obb2Vertices);
	for (size_t i = 0; i < 3; i++) {
		if (isHST(obb1.oriientations[i], obb1Vertices, obb2Vertices)) {
			return false;
		}
		if (isHST(obb2.oriientations[i], obb1Vertices, obb2Vertices)) {
			return false;
		}
	}
	Vector3 cross{};
	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 3; j++) {
			cross = Cross(obb1.oriientations[i], obb2.oriientations[j]);
			cross = Normalize(cross);
			if (isHST(cross, obb1Vertices, obb2Vertices)) {
				return false;
			}
		}
	}
	return true;
}

void OBBRotation(const Vector3& rotate, OBB& obb) {
	Matrix4x4 OBBrotateMatrix;

	OBBrotateMatrix =
		Multiply(MakeRotateXMatrix(rotate.x), Multiply(MakeRotateYMatrix(rotate.y), MakeRotateZMatrix(rotate.z)));
	obb.oriientations[0].x = OBBrotateMatrix.m[0][0];
	obb.oriientations[0].y = OBBrotateMatrix.m[0][1];
	obb.oriientations[0].z = OBBrotateMatrix.m[0][2];

	obb.oriientations[1].x = OBBrotateMatrix.m[1][0];
	obb.oriientations[1].y = OBBrotateMatrix.m[1][1];
	obb.oriientations[1].z = OBBrotateMatrix.m[1][2];

	obb.oriientations[2].x = OBBrotateMatrix.m[2][0];
	obb.oriientations[2].y = OBBrotateMatrix.m[2][1];
	obb.oriientations[2].z = OBBrotateMatrix.m[2][2];
}


const char kWindowTitle[] = "LE2A_18_ヤマグチ_タクト_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Vector3 a{ 0.2f,1.0f,0.0f };
	Vector3 b{ 2.4f,3.1f,1.2f };
	Vector3 c = a + b;
	Vector3 d = a - b;
	Vector3 e = a * 2.4f;
	Vector3 rotate{ 0.4f,1.43f,-0.8f };
	Matrix4x4 rXM = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rYM = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rZM = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rM = rXM * rYM * rZM;
	
	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		ImGui::Begin("Window");
		ImGui::Text("c:%f, %f, %f", c.x, c.y, c.z);
		ImGui::Text("d:%f, %f, %f", d.x, d.y, d.z);
		ImGui::Text("e:%f, %f, %f", e.x, e.y, e.z);
		ImGui::Text("matrix:\n%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f",
			rM.m[0][0], rM.m[0][1], rM.m[0][2], rM.m[0][3],
			rM.m[1][0], rM.m[1][1], rM.m[1][2], rM.m[1][3],
			rM.m[2][0], rM.m[2][1], rM.m[2][2], rM.m[2][3],
			rM.m[3][0], rM.m[3][1], rM.m[3][2], rM.m[3][3]);
		ImGui::End();


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
