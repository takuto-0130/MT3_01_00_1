#include <Novice.h>
#include "Matrix4x4Func.h"
#include "Vector3Func.h"
#include <imgui.h>

const int kWindowWidth = 1280;
const int kWindowHeight = 720;

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

struct Sphere {
	Vector3 center;
	float radius;
};

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 16;
	const float kLonEvery = float(M_PI) * 2.0f / kSubdivision;
	const float kLatEvery = float(M_PI) / kSubdivision;

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex;
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;
			Vector3 a = { (cosf(lat) * cosf(lon))* sphere.radius, (sinf(lat))* sphere.radius, (cosf(lat) * sinf(lon))* sphere.radius };
			Vector3 b = { (cosf(lat + kLatEvery) * cosf(lon))* sphere.radius, (sinf(lat + kLatEvery))* sphere.radius, (cosf(lat + kLatEvery) * sinf(lon))* sphere.radius };
			Vector3 c = { (cosf(lat) * cosf(lon + kLonEvery))* sphere.radius, (sinf(lat))* sphere.radius, (cosf(lat) * sinf(lon + kLonEvery))* sphere.radius };
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

const char kWindowTitle[] = "LC1A_27_ヤマグチ_タクト_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	
	Vector3 rotate{ 0.26f,0,0 };
	Vector3 translate{};
	Vector3 cametaPosition{ 0,1.9f,-6.49f };

	Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, {0,0,0}, translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, cametaPosition);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePrespectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

	Sphere sphere{
		{0,0,0},
		1.0f
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
		ImGui::DragFloat3("CamaraRotate", &rotate.x, 0.01f);
		ImGui::DragFloat3("SphereCenter", &sphere.center.x, 0.01f);
		ImGui::DragFloat("Sphere", &sphere.radius, 0.01f);
		ImGui::End();
		cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, cametaPosition);
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
