#include <Novice.h>
#include <imgui.h>
#include "DrawObj.h"
#include <algorithm>
#include <vector>
#include "Vector3Func.h"

struct Quaternion {
	float x;
	float y;
	float z;
	float w;
};

Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs) {
	Quaternion qr{};
	Vector3 qv = { lhs.x,lhs.y,lhs.z };
	Vector3 rv = { rhs.x,rhs.y,rhs.z };
	qr.w = (lhs.w * rhs.w) - Dot( qv, rv );
	Vector3 qrv = Cross(qv, rv) + (rhs.w * qv) + (lhs.w * rv);
	qr.x = qrv.x;
	qr.y = qrv.y;
	qr.z = qrv.z;
	return qr;
};

Quaternion IdentityQuaternion() {
	Quaternion qr{};
	qr = { 0,0,0,1 };
	return qr;
};

Quaternion Conjugate(const Quaternion& quaternion) {
	Quaternion qr{};
	qr = { quaternion.x * -1.0f,quaternion.y * -1.0f,quaternion.z * -1.0f,quaternion.w };
	return qr;
};

float Norm(const Quaternion& quaternion) {
	return std::sqrtf(quaternion.x * quaternion.x + quaternion.y * quaternion.y + quaternion.z * quaternion.z + quaternion.w * quaternion.w);
}

Quaternion Normalize(const Quaternion& quaternion) {
	Quaternion qr{};
	float norm = Norm(quaternion);
	qr = { quaternion.x / norm, quaternion.y / norm, quaternion.z / norm, quaternion.w / norm };
	return qr;
};

Quaternion Inverse(const Quaternion& quaternion) {
	Quaternion qr{};
	qr = Conjugate(quaternion);
	float norm = Norm(quaternion);
	qr = { qr.x / (norm * norm), qr.y / (norm * norm), qr.z / (norm * norm), qr.w / (norm * norm) };
	return qr;
};

const char kWindowTitle[] = "LE2A_20_ヤマグチ_タクト_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Quaternion q1 = { 2.0f,3.0f,4.0f,1.0f };
	Quaternion q2 = { 1.0f,3.0f,5.0f,2.0f };
	Quaternion identity = IdentityQuaternion();
	Quaternion conj = Conjugate(q1);
	Quaternion inv = Inverse(q1);
	Quaternion normal = Normalize(q1);
	Quaternion mul1 = Multiply(q1, q2);
	Quaternion mul2 = Multiply(q2, q1);
	float norm = Norm(q1);


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
		Novice::ScreenPrintf(0, 0, "%5.2f %5.2f %5.2f %5.2f   : Identity", identity.x, identity.y, identity.z, identity.w);
		Novice::ScreenPrintf(0, 20, "%5.2f %5.2f %5.2f %5.2f   : Conjugate", conj.x, conj.y, conj.z, conj.w);
		Novice::ScreenPrintf(0, 40, "%5.2f %5.2f %5.2f %5.2f   : Inverse", inv.x, inv.y, inv.z, inv.w);
		Novice::ScreenPrintf(0, 60, "%5.2f %5.2f %5.2f %5.2f   : Normalize", normal.x, normal.y, normal.z, normal.w);
		Novice::ScreenPrintf(0, 80, "%5.2f %5.2f %5.2f %5.2f  : Multiply(q1, q2)", mul1.x, mul1.y, mul1.z, mul1.w);
		Novice::ScreenPrintf(0, 100, "%5.2f %5.2f %5.2f %5.2f  : Multiply(q2, q1)", mul2.x, mul2.y, mul2.z, mul2.w);
		Novice::ScreenPrintf(0, 120, "%5.2f                     : Norm", norm);


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
