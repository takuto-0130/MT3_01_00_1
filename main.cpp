#include <Novice.h>
#include <imgui.h>
#include "DrawObj.h"

Vector3 Project(const Vector3& v1, const Vector3& v2) {
	Vector3 v2Normalize = Normalize(v2);
};

Vector3 ClosestPoint(const Vector3& v1, const Vector3& v2) {

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

	Sphere sphere{
		point,
		0.01f
	};

	Vector3 rotateS{};
	Matrix4x4 sphereMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotateS, sphere.center);
	//Matrix4x4 viewMatrixS = Inverse(sphereMatrix);
	Matrix4x4 SphereViewProjectionMatrix = Multiply(sphereMatrix, Multiply(viewMatrix, projectionMatrix));

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
		ImGui::DragFloat3("CamaraTranslate", &cametaPosition.x, 0.01f);
		ImGui::DragFloat3("CamaraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("SphereCenter", &sphere.center.x, 0.01f);
		ImGui::DragFloat("Sphere", &sphere.radius, 0.01f);
		ImGui::End();
		cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cametaPosition);
		viewMatrix = Inverse(cameraMatrix);
		worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

		sphereMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotateS, sphere.center);
		SphereViewProjectionMatrix = Multiply(sphereMatrix, Multiply(viewMatrix, projectionMatrix));

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		
		DrawGrid(worldViewProjectionMatrix, viewportMatrix);
		DrawSphere(sphere, SphereViewProjectionMatrix, viewportMatrix, BLUE);

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
