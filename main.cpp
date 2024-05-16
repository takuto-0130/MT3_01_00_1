#include <Novice.h>
#include <imgui.h>
#include "DrawObj.h"

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


const char kWindowTitle[] = "LC1A_27_ヤマグチ_タクト_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	
	Vector3 cameraRotate{ 0.26f,0,0 };
	Vector3 translate{};
	Vector3 cametaPosition{ 0,1.9f,-6.49f };

	Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, {0,0,0}, translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cametaPosition);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePrespectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);


	Vector3 point1{ -1.5f,0.0f,0.0f };
	Vector3 point2{ 1.5f,0,0 };
	Sphere sphere1{
		point1,
		1.0f
	};
	uint32_t sphere1Color = WHITE;

	Sphere sphere2{
		point2,
		1.0f
	};

	Matrix4x4 sphere1Matrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, {0,0,0}, sphere1.center);

	Matrix4x4 sphere2Matrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0,0,0 }, sphere2.center);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

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
		ImGui::DragFloat3("Point", &point1.x, 0.01f);
		ImGui::DragFloat3("closestPoint", &point2.x, 0.01f);
		ImGui::End();

		cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cametaPosition);
		viewMatrix = Inverse(cameraMatrix);

		worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

		sphere2.center = point2;

		sphere1.center = point1;

		sphere1Matrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0,0,0 }, sphere1.center);
		sphere2Matrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0,0,0 }, sphere2.center);

		if (Length(sphere1.center - sphere2.center) <= sphere1.radius + sphere2.radius) {
			sphere1Color = RED;
		}
		else {
			sphere1Color = WHITE;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		
		DrawGrid(worldViewProjectionMatrix, viewportMatrix);
		DrawSphere(sphere1, Multiply(sphere1Matrix, Multiply(viewMatrix, projectionMatrix)), viewportMatrix, sphere1Color);
		DrawSphere(sphere2, Multiply(sphere2Matrix, Multiply(viewMatrix, projectionMatrix)), viewportMatrix, WHITE);

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
