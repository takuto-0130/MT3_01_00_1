#include <Novice.h>
#include "Matrix4x4Func.h"
#include "Vector3Func.h"

const int kWindowWidth = 1280;
const int kWindowHeight = 720;

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);

	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		Vector3 start = { -kGridHalfWidth * float(kGridEvery), 0, -kGridHalfWidth * float(kGridEvery + xIndex) };
		Vector3 end = { kGridHalfWidth * float(kGridEvery), 0, start.z };
		Vector3 ndcStartVertex = Transform(start, viewProjectionMatrix);
		Vector3 ndcEndVertex = Transform(end, viewProjectionMatrix);
		Vector3 screenStart = Transform(ndcStartVertex, viewportMatrix);
		Vector3 screenEnd = Transform(ndcEndVertex, viewportMatrix);
		if (xIndex == kSubdivision / 2) {
			Novice::DrawLine(int(screenStart.x), int(screenStart.z), int(screenEnd.x), int(screenEnd.z), BLACK);
		}
		else {
			Novice::DrawLine(int(screenStart.x), int(screenStart.z), int(screenEnd.x), int(screenEnd.z), WHITE);
		}
	}
}


const char kWindowTitle[] = "LC1A_27_ヤマグチ_タクト_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	

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
