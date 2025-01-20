#include <Novice.h>
#include <imgui.h>
#include "DrawObj.h"
#include <algorithm>
#include <vector>
#include "Vector3Func.h"

#pragma region // 前回まで
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

Quaternion MakeRotateAxisAngleQuaternion(const Vector3& vec, const float theta) {
	Quaternion q = IdentityQuaternion();
	Vector3 n = Normalize(vec);
	q.x = n.x * std::sinf(theta / 2.0f);
	q.y = n.y * std::sinf(theta / 2.0f);
	q.z = n.z * std::sinf(theta / 2.0f);
	q.w = std::cosf(theta / 2.0f);
	return q;
}

Vector3 RotateVector(const Vector3& v, const Quaternion& q) {
	Quaternion result = IdentityQuaternion();
	Quaternion qV = { v.x, v.y, v.z, 0 };
	Quaternion qConj = Conjugate(q);
	result = Multiply(Multiply(q, qV), qConj);
	return Vector3(result.x, result.y, result.z);
}

Matrix4x4 MakeRotateMatrix(const Quaternion& q) {
	Matrix4x4 result = MakeIdentity4x4();
	result.m[0][0] = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	result.m[0][1] = 2 * (q.x * q.y + q.w * q.z);
	result.m[0][2] = 2 * (q.x * q.z - q.w * q.y);

	result.m[1][0] = 2 * (q.x * q.y - q.w * q.z);
	result.m[1][1] = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
	result.m[1][2] = 2 * (q.y * q.z + q.w * q.x);

	result.m[2][0] = 2 * (q.x * q.z + q.w * q.y);
	result.m[2][1] = 2 * (q.y * q.z - q.w * q.x);
	result.m[2][2] = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	return result;
}
#pragma endregion

float Dot(const Quaternion& q0, const Quaternion& q1) {
	return q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
}

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t) {
	float dot = Dot(q0, q1);
	Quaternion q0cul = q0;
	if (dot < 0) {
		q0cul = { -q0.x, -q0.y, -q0.z,-q0.w };
		dot = -dot;
	}
	float theta = std::acosf(dot);
	float q0num = std::sinf((1 - t) * theta) / std::sinf(theta);
	float q1num = std::sinf(t * theta) / std::sinf(theta);
	Quaternion result = { q0num * q0cul.x + q1num * q1.x, q0num * q0cul.y + q1num * q1.y,
		q0num * q0cul.z + q1num * q1.z, q0num * q0cul.w + q1num * q1.w };
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

	
	Quaternion rotation0 = MakeRotateAxisAngleQuaternion(Vector3{ 0.71f, 0.71f, 0.0f }, 0.3f);
	Quaternion rotation1 = MakeRotateAxisAngleQuaternion(Vector3{ 0.71f, 0.0f, 0.71f }, 3.141592f);

	Quaternion interpolate0 = Slerp(rotation0, rotation1, 0.0f);
	Quaternion interpolate1 = Slerp(rotation0, rotation1, 0.3f);
	Quaternion interpolate2 = Slerp(rotation0, rotation1, 0.5f);
	Quaternion interpolate3 = Slerp(rotation0, rotation1, 0.7f);
	Quaternion interpolate4 = Slerp(rotation0, rotation1, 1.0f);

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
		
		Novice::ScreenPrintf(0, 0, "%5.2f %5.2f %5.2f %5.2f   : interpolate0, Slerp(q0, q1, 0.0f)", interpolate0.x, interpolate0.y, interpolate0.z, interpolate0.w);
		Novice::ScreenPrintf(0, 20, "%5.2f %5.2f %5.2f %5.2f   : interpolate1, Slerp(q0, q1, 0.3f)", interpolate1.x, interpolate1.y, interpolate1.z, interpolate1.w);
		Novice::ScreenPrintf(0, 40, "%5.2f %5.2f %5.2f %5.2f   : interpolate2, Slerp(q0, q1, 0.5f)", interpolate2.x, interpolate2.y, interpolate2.z, interpolate2.w);
		Novice::ScreenPrintf(0, 60, "%5.2f %5.2f %5.2f %5.2f   : interpolate3, Slerp(q0, q1, 0.7f)", interpolate3.x, interpolate3.y, interpolate3.z, interpolate3.w);
		Novice::ScreenPrintf(0, 80, "%5.2f %5.2f %5.2f %5.2f   : interpolate4, Slerp(q0, q1, 1.0f)", interpolate4.x, interpolate4.y, interpolate4.z, interpolate4.w);
		/*Novice::ScreenPrintf(0, 0, "%5.2f %5.2f %5.2f %5.2f   : Identity", identity.x, identity.y, identity.z, identity.w);
		Novice::ScreenPrintf(0, 20, "%5.2f %5.2f %5.2f %5.2f   : Conjugate", conj.x, conj.y, conj.z, conj.w);
		Novice::ScreenPrintf(0, 40, "%5.2f %5.2f %5.2f %5.2f   : Inverse", inv.x, inv.y, inv.z, inv.w);*/


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
