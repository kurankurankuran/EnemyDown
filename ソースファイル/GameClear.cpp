//=============================================================================
//
// ゲームオーバークラス実装 [GameClear.cpp]
// Author : KAZUKI KURA
//
//=============================================================================
#include "GameClear.h"
#include "TextureData.h"
#include "Sound.h"
#include "Input.h"
#include "Polygon.h"
#include "Fade.h"

// 定数定義
namespace {
	const float g_fTexGameClearWidth = 1120.0f;
	const float g_fTexGameClearHeight = 960.0f;

	const float g_fTexThanksWidth = 960.0f;
	const float g_fTexThanksHeight = 800.0f;

	const float g_fTexStartWidth = 960.0f;
	const float g_fTexStartHeight = 640.0f;
	const float g_fTexStartY = SCREEN_HEIGHT / -3.0f;
}

// コンストラクタ
CGameClear::CGameClear() : CScene()
{
	m_id = SCENE_GAMECLEAR;
	m_nTimer = 0;
}

// デストラクタ
CGameClear::~CGameClear()
{
}

// 初期化
bool CGameClear::Init()
{
	// カメラ設定
	GetCameraManager()->AddCamera(CCameraManager::ECameraMode::STATIC_CAMERAMODE);
	GetCameraManager()->ChangeCamera(CCameraManager::ECameraMode::STATIC_CAMERAMODE);

	if (!CScene::Init()) {
		return false;
	}

	m_bStart = false;
	m_nTimer = 0;

	// BGM再生開始
	CSound::Play(BGM_GAMECLEAR);

	return true;
}

// 終了処理
void CGameClear::Fin()
{
	// BGM再生停止
	CSound::Stop(BGM_GAMECLEAR);

	CScene::Fin();
}

// 更新
void CGameClear::Update()
{
	++m_nTimer;
	if (m_nTimer > 120) {
		m_nTimer = 0;
	}

	if (!m_bStart) {
		if (CInput::GetKeyRelease(VK_SPACE) ||
			CInput::GetKeyRelease(VK_RETURN)) {
			m_bStart = true;
			CFade::Out(SCENE_TITLE); 
			CSound::Play(SE_DECID);
		}
	}
}

// 描画
void CGameClear::Draw()
{
	ID3D11DeviceContext* pDC = GetDeviceContext();

	SetZBuffer(false);
	SetBlendState(BS_ALPHABLEND);

	CPolygon::SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	CPolygon::SetTexture(CTextureData::GetTextureResource(GAMECLEAR_BG));
	CPolygon::SetSize((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
	CPolygon::SetUV(0.0f, 0.0f);
	CPolygon::SetFrameSize(1.0f, 1.0f);
	CPolygon::SetPos(0.0f, 0.0f);
	CPolygon::Draw(pDC);

	
	CPolygon::SetTexture(CTextureData::GetTextureResource(GAMECLEAR_CONGRATULATIONS));
	CPolygon::SetSize(g_fTexGameClearWidth, g_fTexGameClearHeight);
	CPolygon::SetPos(0.0f, 100.0f);
	CPolygon::Draw(pDC);

	CPolygon::SetTexture(CTextureData::GetTextureResource(GAMECLEAR_THANKYOUFORPLAYING));
	CPolygon::SetSize(g_fTexThanksWidth, g_fTexThanksHeight);
	CPolygon::SetPos(0.0f, -100.0f);
	CPolygon::Draw(pDC);
	

	CPolygon::SetAlpha((m_nTimer <= 60) ? m_nTimer / 60.0f : (120 - m_nTimer) / 60.0f);
	CPolygon::SetTexture(CTextureData::GetTextureResource(PRESSSPACEKEY));
	CPolygon::SetSize(g_fTexStartWidth, g_fTexStartHeight);
	CPolygon::SetPos(0.0f, g_fTexStartY);
	CPolygon::Draw(pDC);
}
