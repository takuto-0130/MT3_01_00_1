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
	Vector3 project = Project(Subtruct(point, segment.origine), segment.diff);
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


	Segment segment{ {-2.0f,-1.0f,0.0f},{3.0f,2.0f,2.0f} };
	Vector3 point{ -1.5f,0.6f,0.6f };
	Vector3 closestPoint = ClosestPoint(point, segment);
	Sphere pointSphere{
		point,
		0.01f
	};

	Sphere closestPointSphere{
		closestPoint,
		0.01f
	};

	Matrix4x4 pointSphereMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, {0,0,0}, pointSphere.center);
	Matrix4x4 pointSphereViewMatrix = Inverse(pointSphereMatrix);

	Matrix4x4 closestPointSphereMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0,0,0 }, closestPointSphere.center);
	Matrix4x4 closestPointSphereViewMatrix = Inverse(closestPointSphereMatrix);

	Vector3 start = Transform(Transform(segment.origine, worldViewProjectionMatrix), viewportMatrix);
	Vector3 end = Transform(Transform(Add(segment.origine, segment.diff), worldViewProjectionMatrix), viewportMatrix);

	Vector3 project = Project(Subtruct(point, segment.origine), segment.diff);

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
		ImGui::DragFloat3("Point", &point.x, 0.01f);
		ImGui::DragFloat3("segment.origine.x", &segment.origine.x, 0.01f);
		ImGui::DragFloat3("segment.diff", &segment.diff.x, 0.01f);
		ImGui::InputFloat3("p", &project.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::End();
		cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cametaPosition);
		viewMatrix = Inverse(cameraMatrix);
		worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		DrawGrid(worldViewProjectionMatrix, viewportMatrix);
		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), WHITE);
		worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(pointSphereViewMatrix, projectionMatrix));
		DrawSphere(pointSphere, worldViewProjectionMatrix, viewportMatrix, RED);
		worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(closestPointSphereViewMatrix, projectionMatrix));
		DrawSphere(closestPointSphere, worldViewProjectionMatrix, viewportMatrix, BLACK);

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
