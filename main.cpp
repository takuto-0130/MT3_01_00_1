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
	int playSoundLength;
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
	soundData.playSoundLength = data.size / format.fmt.nBlockAlign;

	return soundData;
}


void SoundUnload(SoundData* soundData) {
	delete[] soundData->pBuffer;

	soundData->playSoundLength = 0;
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

	/*IXAudio2SubmixVoice* pSubmixVoice = submixVoice;
	pSourceVoice->SetOutputMatrix(pSubmixVoice, 1, 1, &DSPSettings.ReverbLevel);*/

	XAUDIO2_FILTER_PARAMETERS FilterParameters = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * DSPSettings.LPFDirectCoefficient), 1.0f };
	pSourceVoice->SetFilterParameters(&FilterParameters);

	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.PlayBegin = 0;
	buf.PlayLength = soundData.playSoundLength;
	FLOAT32 pan = -1.0f;
	pSourceVoice->SetChannelVolumes(1, &pan);
	hr = pSourceVoice->SubmitSourceBuffer(&buf);
	hr = pSourceVoice->Start();
}

const char kWindowTitle[] = "LE2A_18_ヤマグチ_タクト_タイトル";

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
	Emitter.InnerRadius =10.0f;
	Emitter.InnerRadiusAngle = 360.0f * (float(M_PI) / 180.0f); 
	//Emitter.pChannelAzimuths = new float[2] { -X3DAUDIO_PI / 2, X3DAUDIO_PI / 2 };	// エミッターの方位角

	X3DAUDIO_VECTOR EmitterOrientFront = { 0,0,-1 };
	X3DAUDIO_VECTOR EmitterOrientTop = { 0,1,0 };
	X3DAUDIO_VECTOR EmitterPosition = Emitter.Position;
	X3DAUDIO_VECTOR EmitterVelocity = { 0,0,1 };
	X3DAUDIO_VECTOR ListenerOrientFront = { 0,0,1 };
	X3DAUDIO_VECTOR ListenerOrientTop = { 0,1,0 };
	X3DAUDIO_VECTOR ListenerPosition = { 0.0f,0.0f,0.0f };
	X3DAUDIO_VECTOR ListenerVelocity = Listener.Velocity;

	

	X3DAUDIO_DSP_SETTINGS DSPSettings = {};
	DSPSettings.SrcChannelCount = Emitter.ChannelCount;
	DSPSettings.DstChannelCount = deviceDetails.InputChannels;
	DSPSettings.pMatrixCoefficients = new float[Emitter.ChannelCount * deviceDetails.InputChannels];
	DSPSettings.pDelayTimes = new float[deviceDetails.InputChannels];

	SoundData soundData1 = SoundLoadWave("NoviceResources/fanfare.wav");
	SoundData soundData2 = SoundLoadWave("C:/Windows/Media/Alarm01.wav");

	int frameCount = 0;
	float masterVolume = 1.0f;
	float source1Volume = 0.7f;
	float source2Volume = 0.7f;
	
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


	Vector3 point1{ 0.0f,0.0f,0.0f };
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
	bool isPlay = false;
	bool isPlay2 = false;
	IXAudio2SourceVoice* pSourceVoice[2] = { nullptr };
	IXAudio2Voice* pMasterVoice = masterVoice;
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
		EmitterPosition = { point1.x,point1.y,point1.z };
		ListenerPosition = { 0,0,0 };
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
				X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER /*| X3DAUDIO_CALCULATE_DELAY | X3DAUDIO_CALCULATE_REVERB*/,
				&DSPSettings);
		//}

		ImGui::Begin("Window");
		ImGui::SliderFloat("MasterVolume", &masterVolume, 0.0f, 1.0f);
		ImGui::SliderFloat("source1Volume", &source1Volume, 0.0f, 1.0f);
		ImGui::SliderFloat("source2Volume", &source2Volume, 0.0f, 1.0f);
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
			HRESULT hr;
			//
			hr = xAudio2->CreateSourceVoice(&pSourceVoice[0], &soundData1.wfex);
			assert(SUCCEEDED(hr));
			//
			XAUDIO2_VOICE_DETAILS voiceDetalis;
			pSourceVoice[0]->GetVoiceDetails(&voiceDetalis);
			pSourceVoice[0]->SetOutputMatrix(pMasterVoice, 1, deviceDetails.InputChannels, DSPSettings.pMatrixCoefficients);
			pSourceVoice[0]->SetFrequencyRatio(DSPSettings.DopplerFactor);

			XAUDIO2_FILTER_PARAMETERS FilterParameters = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * DSPSettings.LPFDirectCoefficient), 1.0f };
			pSourceVoice[0]->SetFilterParameters(&FilterParameters);

			XAUDIO2_BUFFER buf{};
			buf.pAudioData = soundData1.pBuffer;
			buf.AudioBytes = soundData1.bufferSize;
			buf.Flags = XAUDIO2_END_OF_STREAM;
			buf.PlayBegin = 0;
			buf.PlayLength = soundData1.playSoundLength;
			FLOAT32 pan = -1.0f;
			pSourceVoice[0]->SetChannelVolumes(1, &pan);
			hr = pSourceVoice[0]->SubmitSourceBuffer(&buf);
			hr = pSourceVoice[0]->Start();
			isPlay = true;
		}
		if(isPlay == true)
		{
			pSourceVoice[0]->SetOutputMatrix(pMasterVoice, 1, deviceDetails.InputChannels, DSPSettings.pMatrixCoefficients);
			pSourceVoice[0]->SetVolume(source1Volume);
		}

		if (keys[DIK_S] && !(preKeys[DIK_S])) {
			HRESULT hr;
			//
			hr = xAudio2->CreateSourceVoice(&pSourceVoice[1], &soundData2.wfex);
			assert(SUCCEEDED(hr));
			//
			XAUDIO2_VOICE_DETAILS voiceDetalis;
			pSourceVoice[1]->GetVoiceDetails(&voiceDetalis);
			pSourceVoice[1]->SetOutputMatrix(pMasterVoice, 1, deviceDetails.InputChannels, DSPSettings.pMatrixCoefficients);
			pSourceVoice[1]->SetFrequencyRatio(DSPSettings.DopplerFactor);

			XAUDIO2_FILTER_PARAMETERS FilterParameters = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * DSPSettings.LPFDirectCoefficient), 1.0f };
			pSourceVoice[1]->SetFilterParameters(&FilterParameters);

			XAUDIO2_BUFFER buf{};
			buf.pAudioData = soundData2.pBuffer;
			buf.AudioBytes = soundData2.bufferSize;
			buf.Flags = XAUDIO2_END_OF_STREAM;
			buf.PlayBegin = 0;
			buf.PlayLength = soundData2.playSoundLength;
			FLOAT32 pan = -1.0f;
			pSourceVoice[1]->SetChannelVolumes(1, &pan);
			hr = pSourceVoice[1]->SubmitSourceBuffer(&buf);
			hr = pSourceVoice[1]->Start();
			isPlay2 = true;
		}
		if (isPlay2 == true)
		{
			pSourceVoice[1]->SetOutputMatrix(pMasterVoice, 1, deviceDetails.InputChannels, DSPSettings.pMatrixCoefficients);
			pSourceVoice[1]->SetVolume(source2Volume);
		}
		pMasterVoice->SetVolume(masterVolume);
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		
		/*DrawGrid(worldViewProjectionMatrix, viewportMatrix);
		DrawSphere(sphere1, worldViewProjectionMatrix, viewportMatrix, sphere1Color);*/
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
	if (isPlay == true)
	{
		pSourceVoice[0]->DestroyVoice();
	}
	if (isPlay2 == true)
	{
		pSourceVoice[1]->DestroyVoice();
	}
	xAudio2.Reset();
	SoundUnload(&soundData1);
	SoundUnload(&soundData2);

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
