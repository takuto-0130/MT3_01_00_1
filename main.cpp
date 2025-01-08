#include <Novice.h>
#include <imgui.h>
#include "DrawObj.h"
#include <algorithm>
#include <vector>
#include "Vector3Func.h"


Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, const float& angle) {
	Matrix4x4 result = MakeIdentity4x4();
	result.m[0][0] = axis.x * axis.x * (1.0f - std::cosf(angle)) + std::cosf(angle);
	result.m[0][1] = axis.x * axis.y * (1.0f - std::cosf(angle)) + axis.z * std::sinf(angle);
	result.m[0][2] = axis.x * axis.z * (1.0f - std::cosf(angle)) - axis.y * std::sinf(angle);

	result.m[1][0] = axis.x * axis.y * (1.0f - std::cosf(angle)) - axis.z * std::sinf(angle);
	result.m[1][1] = axis.y * axis.y * (1.0f - std::cosf(angle)) + std::cosf(angle);
	result.m[1][2] = axis.y * axis.z * (1.0f - std::cosf(angle)) + axis.x * std::sinf(angle);

	result.m[2][0] = axis.x * axis.z * (1.0f - std::cosf(angle)) + axis.y * std::sinf(angle);
	result.m[2][1] = axis.y * axis.z * (1.0f - std::cosf(angle)) - axis.x * std::sinf(angle);
	result.m[2][2] = axis.z * axis.z * (1.0f - std::cosf(angle)) + std::cosf(angle);

	return result;
}

static const int kRowHeight = 20;
static const int kColumnWidth = 60;


void MatrixScreenPrintf(int x, int y, Matrix4x4 matrix, const char* text) {
	Novice::ScreenPrintf(x, y, "%s", text);
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(x + column * kColumnWidth,
				y + (row + 1) * kRowHeight, "%6.3f", matrix.m[row][column]);
		}
	}
}

const char kWindowTitle[] = "LE2A_20_ヤマグチ_タクト_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Vector3 axis = Normalize({ 1.0f,1.0f,1.0f });
	float angle = 0.44f;

	Matrix4x4 rotateMatrix = MakeRotateAxisAngle(axis, angle);

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

	


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		MatrixScreenPrintf(0, 0, rotateMatrix, "rotateMatrix");

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
