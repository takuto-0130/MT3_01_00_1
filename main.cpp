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

bool IsCollision(const Sphere& sphere, const Plane& plane) {
	float distance = sqrtf((Dot(plane.normal, sphere.center) - plane.distance) * (Dot(plane.normal, sphere.center) - plane.distance));
	if (distance <= sphere.radius) {
		return true;
	}
	return false;
}

Vector3 Parpendicular(const Vector3& vector) {
	if (vector.x != 0 || vector.y != 0) {
		return { -vector.y,vector.x, 0.0f };
	}
	return { 0.0f,-vector.z,vector.y };
}

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 center = Multiply(plane.normal, plane.distance);
	Vector3 perpendiculars[4];
	perpendiculars[0] = Normalize(Parpendicular(plane.normal));
	perpendiculars[1] = -perpendiculars[0];
	perpendiculars[2] = Cross(plane.normal, perpendiculars[0]);
	perpendiculars[3] = -perpendiculars[2];

	Vector3 points[4];
	for (int32_t index = 0; index < 4; ++index) {
		Vector3 extend = Multiply(2.0f, perpendiculars[index]);
		Vector3 point = Add(center, extend);
		points[index] = Transform(Transform(point, viewProjectionMatrix), viewportMatrix);
	}

	Novice::DrawLine(int(points[0].x), int(points[0].y), int(points[2].x), int(points[2].y), color);
	Novice::DrawLine(int(points[0].x), int(points[0].y), int(points[3].x), int(points[3].y), color);
	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[2].x), int(points[2].y), color);
	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[3].x), int(points[3].y), color);
}


const char kWindowTitle[] = "LC1A_27_ヤマグチ_タクト_タイトル";

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


	Vector3 point1{ -1.5f,0.0f,0.0f };
	float radius = 1.0f;
	Sphere sphere1{
		point1,
		radius
	};
	uint32_t sphere1Color = WHITE;

	Matrix4x4 sphere1Matrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, {0,0,0}, sphere1.center);
	Matrix4x4 sphere1ViewMatrix = Inverse(sphere1Matrix);


	Plane plane{
		{0.1f,0.1f,0.1f},
		2.0f
	};

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
		ImGui::DragFloat3("plane.normal", &plane.normal.x, 0.01f);
		ImGui::DragFloat3("SphereCenter", &point1.x, 0.01f);
		ImGui::DragFloat("SphereRadius", &radius, 0.01f,0.0f,100.0f);
		ImGui::DragFloat3("cametaPosition", &cametaPosition.x, 0.01f);
		ImGui::DragFloat3("WorldRotate", &rotate.x, 0.01f);
		ImGui::End();
		plane.normal = Normalize(plane.normal);

		worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, translate);

		cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cametaPosition);
		viewMatrix = Inverse(cameraMatrix);

		worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

		sphere1.center = point1;
		sphere1.radius = radius;

		sphere1Matrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, {0,0,0}, sphere1.center);
		sphere1ViewMatrix = Inverse(sphere1Matrix);

		if (IsCollision(sphere1,plane)) {
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
		DrawPlane(plane, worldViewProjectionMatrix, viewportMatrix, WHITE);
		DrawSphere(sphere1, worldViewProjectionMatrix, viewportMatrix, sphere1Color);
		//DrawSphere(sphere2, Multiply(sphere2Matrix, Multiply(viewMatrix, projectionMatrix)), viewportMatrix, WHITE);

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
