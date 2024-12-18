// フェード イン/アウト [Fade.h]
#pragma once
#include "main.h"
#include "Scene.h"

// 定数定義
enum EFade {
	FADE_NONE = 0,	// 何もしていない
	FADE_IN,		// フェードイン処理
	FADE_OUT,		// フェードアウト処理

	MAX_FADE
};

// クラス定義
class CFade {
private:
	static EFade m_fade;
	static EScene m_sceneNext;
	static XMFLOAT4 m_vColor;
	static float m_fRate;
	static int m_nCnt;

	static float m_fSoundVolume;
	static float m_fVolumeRate;
public:
	static void Init();
	static void Update();
	static void Draw();

	static void Out(EScene scene, float fSecond = 1.0f);
	static EFade Get() { return m_fade; }
	static void SetColor(XMFLOAT3 vColor);
};
