#include <Novice.h>
#include <imgui.h>
#include "DrawObj.h"

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
		{-0.6f,0,-0.3f},
		{0,0.6f,0.3f}
	};

	AABB b{
		{0.5f,0,-0.3f},
		{1.1f,0.6f,0.3f}
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
		ImGui::DragFloat3("bMax", &b.max.x, 0.01f);
		ImGui::DragFloat3("bMin", &b.min.x, 0.01f);
		ImGui::DragFloat3("cameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("cametaPosition", &cametaPosition.x, 0.01f);
		ImGui::DragFloat3("WorldRotate", &rotate.x, 0.01f);
		ImGui::End();

		a.min.x = (std::min)(a.min.x, a.max.x);
		a.min.y = (std::min)(a.min.y, a.max.y);
		a.min.z = (std::min)(a.min.z, a.max.z);

		a.max.x = (std::max)(a.min.x, a.max.x);
		a.max.y = (std::max)(a.min.y, a.max.y);
		a.max.z = (std::max)(a.min.z, a.max.z);


		b.min.x = (std::min)(b.min.x, b.max.x);
		b.min.y = (std::min)(b.min.y, b.max.y);
		b.min.z = (std::min)(b.min.z, b.max.z);

		b.max.x = (std::max)(b.min.x, b.max.x);
		b.max.y = (std::max)(b.min.y, b.max.y);
		b.max.z = (std::max)(b.min.z, b.max.z);

		worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, translate);

		cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cametaPosition);
		viewMatrix = Inverse(cameraMatrix);

		worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

		if (IsCollisionAABB(a, b)) {
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
		DrawAABB(a, worldViewProjectionMatrix, viewportMatrix, color);
		DrawAABB(b, worldViewProjectionMatrix, viewportMatrix, WHITE);

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
