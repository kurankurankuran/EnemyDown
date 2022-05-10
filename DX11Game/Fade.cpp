// フェード イン/アウト [Fade.cpp]
#include "Fade.h"
#include "Polygon.h"
#include "Sound.h"

// 静的メンバ
EFade CFade::m_fade = FADE_NONE;
EScene CFade::m_sceneNext = SCENE_TITLE;
XMFLOAT4 CFade::m_vColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
float CFade::m_fRate = 1.0f / 60.0f;
int   CFade::m_nCnt = (int)(1.0f / m_fRate);
float CFade::m_fSoundVolume = 1.0f;
float CFade::m_fVolumeRate = 0.0f/*1.0f / 60.0f*/;
// 初期化
void CFade::Init()
{
	m_vColor.w = 1.0f;			// 不透明度
	m_fade = FADE_IN;			// 状態
	m_sceneNext = SCENE_TITLE;	// 次のシーン
	//m_fVolumeRate = CSound::GetVolume() / (float)m_nCnt;
}

// 更新
void CFade::Update()
{
	// 何もしていない
	if (m_fade == FADE_NONE) {
		return;
	}
	// フェードアウト処理
	if (m_fade == FADE_OUT) {
		m_vColor.w += m_fRate;	// 不透明度を増加
		if (m_vColor.w >= 1.0f) {
			// フェードイン処理に切替
			m_vColor.w = 1.0f;
			m_fade = FADE_IN;
			// シーン切替
			CScene::Change(m_sceneNext);
		}
		// 音もフェードアウト
		CSound::SetVolume(CSound::GetVolume() - m_fVolumeRate/*1.0f - m_vColor.w*/);
		return;
	}
	// フェードイン処理
	m_vColor.w -= m_fRate;	// 不透明度を下げる
	if (m_vColor.w <= 0.0f) {
		// フェード処理終了
		m_vColor.w = 0.0f;
		m_fade = FADE_NONE;
		//m_fSoundVolume = CSound::GetVolume();
		//m_fVolumeRate = m_fSoundVolume * m_fRate;
	}
	// 音もフェードイン
	CSound::SetVolume(CSound::GetVolume() + m_fVolumeRate /* 1.0f - m_vColor.w*/);
}

// 描画
void CFade::Draw()
{
	ID3D11DeviceContext* pDC = GetDeviceContext();

	SetZBuffer(false);
	SetBlendState(BS_ALPHABLEND);

	CPolygon::SetColor(m_vColor);
	CPolygon::SetTexture(nullptr);
	CPolygon::SetSize((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
	CPolygon::SetUV(0.0f, 0.0f);
	CPolygon::SetFrameSize(1.0f, 1.0f);
	CPolygon::SetPos(0.0f, 0.0f);
	CPolygon::Draw(pDC);

	CPolygon::SetColor(1.0f, 1.0f, 1.0f, 1.0f);
}

// フェードアウト開始
void CFade::Out(EScene scene, float fSecond)
{
	if (m_fade != FADE_OUT) {
		m_fade = FADE_OUT;
		m_sceneNext = scene;
		m_fRate = fSecond / 60.0f;

		m_fSoundVolume = CSound::GetVolume();
		m_fVolumeRate = m_fSoundVolume / (float)m_nCnt;
	}
}

// フェードイン アウト カラー設定
void CFade::SetColor(XMFLOAT3 vColor)
{
	m_vColor.x = vColor.x;
	m_vColor.y = vColor.y;
	m_vColor.z = vColor.z;
}
