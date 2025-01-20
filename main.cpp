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
#pragma endregion

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
	Matrix4x4 R = MakeIdentity4x4();
	/*R.m[0][0] = q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z;
	R.m[0][1] = 2 * (q.x * q.y + q.w * q.z);
	R.m[0][2] = 2 * (q.x * q.z - q.w * q.y);

	R.m[0][0] = 2 * (q.x * q.y - q.w * q.z);
	R.m[0][1] = q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z;
	R.m[0][2] = 2 * (q.y * q.z + q.w * q.x);

	R.m[0][0] = 2 * (q.x * q.z + q.w * q.y);
	R.m[0][1] = 2 * (q.y * q.z - q.w * q.x);
	R.m[0][2] = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;*/
	// クォータニオンの成分
	float x = q.x;
	float y = q.y;
	float z = q.z;
	float w = q.w;

	// 回転行列の計算
	R.m[0][0] = 1.0f - 2.0f * (y * y + z * z);
	R.m[0][1] = 2.0f * (x * y - w * z);
	R.m[0][2] = 2.0f * (x * z + w * y);
	R.m[0][3] = 0.0f;

	R.m[1][0] = 2.0f * (x * y + w * z);
	R.m[1][1] = 1.0f - 2.0f * (x * x + z * z);
	R.m[1][2] = 2.0f * (y * z - w * x);
	R.m[1][3] = 0.0f;

	R.m[2][0] = 2.0f * (x * z - w * y);
	R.m[2][1] = 2.0f * (y * z + w * x);
	R.m[2][2] = 1.0f - 2.0f * (x * x + y * y);
	R.m[2][3] = 0.0f;

	R.m[3][0] = 0.0f;
	R.m[3][1] = 0.0f;
	R.m[3][2] = 0.0f;
	R.m[3][3] = 1.0f;
	return R;
}


const char kWindowTitle[] = "LE2A_20_ヤマグチ_タクト_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	
	Quaternion rotation = MakeRotateAxisAngleQuaternion(Vector3{ 1.0f,0.4f,-0.2f }, 0.45f);
	Vector3 pointY = { 2.1f,-0.9f,1.3f };
	Matrix4x4 rotateMatrix = MakeRotateMatrix(rotation);
	Vector3 rotateByQuaternion = RotateVector(pointY, rotation);
	Vector3 rotateByMatrix = Transform(pointY, rotateMatrix);

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
		
		Novice::ScreenPrintf(0, 0, "%5.2f %5.2f %5.2f %5.2f   : rotation", rotation.x, rotation.y, rotation.z, rotation.w);


		Novice::ScreenPrintf(0, 100, "%5.2f %5.2f %5.2f    : rotateByQuaternion", rotateByQuaternion.x, rotateByQuaternion.y, rotateByQuaternion.z);
		Novice::ScreenPrintf(0, 120, "%5.2f %5.2f %5.2f    : rotateByMatrix", rotateByMatrix.x, rotateByMatrix.y, rotateByMatrix.z);
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
