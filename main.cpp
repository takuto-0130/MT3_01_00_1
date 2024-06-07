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

	if (IsCollisionAABBSphere(aabbOBBLocal, sphereOBBLocal)) {
		return true;
	}
	return false;
}


const char kWindowTitle[] = "LE2A_18_ヤマグチ_タクト_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);


	Vector3 cameraRotate{ 0.26f,0,0 };
	Vector3 translate{};
	Vector3 cametaPosition{ 0,1.9f,-6.49f };
	Vector3 rotate{};

	Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cametaPosition);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePrespectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

	uint32_t color = WHITE;

	AABB a{
		{-0.5f,-0.5f,-0.5f},
		{0.5f,0.5f,0.5f}
	};

	Segment segment{
		.origine{-0.7f, 0.3f, 0.5f},
		.diff{2.0f,-0.5f,0.0f}
	};

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
		ImGui::DragFloat3("aMax", &a.max.x, 0.01f);
		ImGui::DragFloat3("aMin", &a.min.x, 0.01f);
		ImGui::DragFloat3("segment.origine", &segment.origine.x, 0.01f);
		ImGui::DragFloat3("segment.diff", &segment.diff.x, 0.01f);
		ImGui::DragFloat2("WorldRotate", &rotate.x, 0.01f);
		ImGui::End();

		a.min.x = (std::min)(a.min.x, a.max.x);
		a.min.y = (std::min)(a.min.y, a.max.y);
		a.min.z = (std::min)(a.min.z, a.max.z);

		a.max.x = (std::max)(a.min.x, a.max.x);
		a.max.y = (std::max)(a.min.y, a.max.y);
		a.max.z = (std::max)(a.min.z, a.max.z);

		cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cametaPosition);
		cameraMatrix = Multiply(cameraMatrix, MakeRotateXMatrix(rotate.x));
		cameraMatrix = Multiply(cameraMatrix, MakeRotateYMatrix(rotate.y));
		viewMatrix = Inverse(cameraMatrix);

		worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

		if (IsCollisionAABBSegment(a, segment)) {
			color = RED;
		}
		else {
			color = WHITE;
		}


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(worldViewProjectionMatrix, viewportMatrix);
		DrawSegment(segment, worldViewProjectionMatrix, viewportMatrix, WHITE);
		DrawAABB(a, worldViewProjectionMatrix, viewportMatrix, color);


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
