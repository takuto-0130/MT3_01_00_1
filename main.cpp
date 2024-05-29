#include <Novice.h>
#include <imgui.h>
#include "DrawObj.h"
#include <xaudio2.h>
#include <x3daudio.h>
#include <fstream>
#include <wrl.h>

#pragma comment(lib,"xaudio2.lib")

struct ChunkHeader {
	char id[4];
	int32_t size;
};

struct RiffHeader {
	ChunkHeader chunk;
	char type[4];
};

struct FormatChunk {
	ChunkHeader chunk;
	WAVEFORMATEX fmt;
};

struct SoundData {
	WAVEFORMATEX wfex;
	BYTE* pBuffer;
	unsigned int bufferSize;
};

SoundData SoundLoadWave(const char* filename) {

	std::ifstream file;
	file.open(filename, std::ios_base::binary);
	assert(file.is_open());

	RiffHeader riff;
	file.read((char*)&riff.chunk.id, sizeof(riff));
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	FormatChunk format = {};
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt", 4) == 0) {//資料では!=0
		assert(0);
	}
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	if (strncmp(data.id, "JUNK", 4) == 0) {
		file.seekg(data.size, std::ios_base::cur);
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);
	file.close();

	SoundData soundData = {};
	
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
}


void SoundUnload(SoundData* soundData) {
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}


void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData, const X3DAUDIO_DSP_SETTINGS& DSPSettings, const XAUDIO2_VOICE_DETAILS& deviceDetails, IXAudio2MasteringVoice* masterVoice/*, IXAudio2SubmixVoice* submixVoice*/) {
	HRESULT hr;

	IXAudio2SourceVoice* pSourceVoice = nullptr;
	hr = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(hr));
	
	IXAudio2Voice* pMasterVoice = masterVoice;
	XAUDIO2_VOICE_DETAILS voiceDetalis;
	pSourceVoice->GetVoiceDetails(&voiceDetalis);
	pSourceVoice->SetOutputMatrix(pMasterVoice, 1, deviceDetails.InputChannels, DSPSettings.pMatrixCoefficients);
	pSourceVoice->SetFrequencyRatio(DSPSettings.DopplerFactor);

	/*IXAudio2Voice* pSubmixVoice = masterVoice;
	pSourceVoice->SetOutputMatrix(pSubmixVoice, 1, 1, &DSPSettings.ReverbLevel);*/

	XAUDIO2_FILTER_PARAMETERS FilterParameters = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * DSPSettings.LPFDirectCoefficient), 1.0f };
	pSourceVoice->SetFilterParameters(&FilterParameters);

	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	FLOAT32 pan = -1.0f;
	pSourceVoice->SetChannelVolumes(1, &pan);
	hr = pSourceVoice->SubmitSourceBuffer(&buf);
	hr = pSourceVoice->Start();
}

const char kWindowTitle[] = "LC1A_27_ヤマグチ_タクト_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
	HRESULT result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));
	///X3DAudio
	//出力形式のチャネル マスクを取得
	DWORD dwChannelMask;
	masterVoice->GetChannelMask(&dwChannelMask);
	X3DAUDIO_HANDLE X3DInstance;
	X3DAudioInitialize(dwChannelMask, 3.435f, X3DInstance);

	XAUDIO2_VOICE_DETAILS deviceDetails = {};
	masterVoice->GetVoiceDetails(&deviceDetails);
	
	//リスナーとエミッターを初期化
	X3DAUDIO_LISTENER Listener = {};

	X3DAUDIO_EMITTER Emitter = {};
	Emitter.ChannelCount = 1; //エミッターの数 (必ず1以上)
	Emitter.CurveDistanceScaler = Emitter.DopplerScaler = 1.0f;
	//エミッターの数が1を超える場合は↓も設定
	//Emitter.ChannelRadius;
	//Emitter.pChannelAzimuths;
	//Emitter.InnerRadius = 0.3f;

	X3DAUDIO_VECTOR EmitterOrientFront = { 0,0,-1 };
	X3DAUDIO_VECTOR EmitterOrientTop = Emitter.OrientTop;
	X3DAUDIO_VECTOR EmitterPosition = Emitter.Position;
	X3DAUDIO_VECTOR EmitterVelocity = { 0,0,1 };
	X3DAUDIO_VECTOR ListenerOrientFront = { 0,0,1 };
	X3DAUDIO_VECTOR ListenerOrientTop = Listener.OrientTop;
	X3DAUDIO_VECTOR ListenerPosition = Listener.Position;
	X3DAUDIO_VECTOR ListenerVelocity = Listener.Velocity;

	X3DAUDIO_DISTANCE_CURVE_POINT volumePoints[10] = {};
	X3DAUDIO_DISTANCE_CURVE volumeCurve = {};

	/*volumePoints[0].Distance = 0.0f;
	volumePoints[0].DSPSetting = 1.0f;
	volumePoints[1].Distance = 0.2f;
	volumePoints[1].DSPSetting = 1.0f;
	volumePoints[2].Distance = 0.3f;
	volumePoints[2].DSPSetting = 0.5f;
	volumePoints[3].Distance = 0.4f;
	volumePoints[3].DSPSetting = 0.35f;
	volumePoints[4].Distance = 0.5f;
	volumePoints[4].DSPSetting = 0.23f;
	volumePoints[5].Distance = 0.6f;
	volumePoints[5].DSPSetting = 0.16f;
	volumePoints[6].Distance = 0.7f;
	volumePoints[6].DSPSetting = 0.1f;
	volumePoints[7].Distance = 0.8f;
	volumePoints[7].DSPSetting = 0.06f;
	volumePoints[8].Distance = 0.9f;
	volumePoints[8].DSPSetting = 0.04f;
	volumePoints[9].Distance = 1.0f;
	volumePoints[9].DSPSetting = 0.0f;
	volumeCurve.PointCount = 10;
	volumeCurve.pPoints = volumePoints;

	Emitter.pVolumeCurve = &volumeCurve;

	X3DAUDIO_CONE m_emitterCone;

	m_emitterCone.InnerAngle = X3DAUDIO_PI / 2;
	m_emitterCone.OuterAngle = X3DAUDIO_PI;
	m_emitterCone.InnerVolume = 1.0f;
	m_emitterCone.OuterVolume = 0.0f;
	Emitter.pCone = &m_emitterCone;*/

	X3DAUDIO_DSP_SETTINGS DSPSettings = {};
	FLOAT32* matrix = new FLOAT32[deviceDetails.InputChannels];
	DSPSettings.SrcChannelCount = Emitter.ChannelCount;
	DSPSettings.DstChannelCount = deviceDetails.InputChannels;
	DSPSettings.pMatrixCoefficients = matrix;

	SoundData soundData1 = SoundLoadWave("NoviceResources/fanfare.wav");
	SoundData soundData2 = SoundLoadWave("C:/Windows/Media/Alarm01.wav");

	int frameCount = 0;
	
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
	float radius = 0.1f;
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
		frameCount++;
		EmitterPosition = { cametaPosition.x,cametaPosition.y,cametaPosition.z };
		ListenerPosition = { cametaPosition.x,cametaPosition.y,cametaPosition.z };
		//if (frameCount % 3 == 0) {
			Emitter.OrientFront = EmitterOrientFront;
			Emitter.OrientTop = EmitterOrientTop;
			Emitter.Position = EmitterPosition;
			Emitter.Velocity = EmitterVelocity;
			Listener.OrientFront = ListenerOrientFront;
			Listener.OrientTop = ListenerOrientTop;
			Listener.Position = ListenerPosition;
			Listener.Velocity = ListenerVelocity;

			X3DAudioCalculate(X3DInstance, &Listener, &Emitter,
				X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER | X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_REVERB,
				&DSPSettings);
		//}


		ImGui::Begin("Window");
		ImGui::DragFloat3("plane.normal", &plane.normal.x, 0.01f);
		ImGui::DragFloat3("SphereCenter", &point1.x, 0.01f);
		ImGui::DragFloat("SphereRadius", &radius, 0.01f,0.0f,100.0f);
		ImGui::DragFloat3("cametaPosition", &cametaPosition.x, 0.01f);
		ImGui::DragFloat3("WorldRotate", &rotate.x, 0.01f);
		ImGui::InputFloat("distance", &DSPSettings.EmitterToListenerDistance);
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
		if (keys[DIK_SPACE] && !(preKeys[DIK_SPACE])) {
			SoundPlayWave(xAudio2.Get(), soundData1, DSPSettings, deviceDetails,masterVoice);
		}


		if (keys[DIK_S] && !(preKeys[DIK_S])) {
			SoundPlayWave(xAudio2.Get(), soundData2, DSPSettings,  deviceDetails, masterVoice);
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
	xAudio2.Reset();
	SoundUnload(&soundData1);
	SoundUnload(&soundData2);

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
