//=============================================================================
//
// �Q�[���I�[�o�[�N���X���� [GameOver.cpp]
// Author : KAZUKI KURA
//
//=============================================================================
#include "GameOver.h"
#include "TextureData.h"
#include "Sound.h"
#include "Input.h"
#include "Polygon.h"
#include "Fade.h"

// �萔��`
namespace {
	const float g_fTexGameOverWidth = 1120.0f;
	const float g_fTexGameOverHeight = 960.0f;

	const float g_fTexStartWidth = 960.0f;
	const float g_fTexStartHeight = 640.0f;
	const float g_fTexStartY = SCREEN_HEIGHT / -3.0f;

}

// �R���X�g���N�^
CGameOver::CGameOver() : CScene()
{
	m_id = SCENE_GAMEOVER;

	m_bStart = false;
	m_nTimer = 0;
}

// �f�X�g���N�^
CGameOver::~CGameOver()
{
}

// ������
bool CGameOver::Init()
{
	// �J�����ݒ�
	GetCameraManager()->AddCamera(CCameraManager::ECameraMode::STATIC_CAMERAMODE);
	GetCameraManager()->ChangeCamera(CCameraManager::ECameraMode::STATIC_CAMERAMODE);

	if (!CScene::Init()) {
		return false;
	}

	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	m_bStart = false;
	m_nTimer = 0;

	// BGM�Đ��J�n
	CSound::Play(BGM_GAMEOVER);

	return true;
}

// �I������
void CGameOver::Fin()
{
	// BGM�Đ���~
	CSound::Stop(BGM_GAMEOVER);

	CScene::Fin();
}

// �X�V
void CGameOver::Update()
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

// �`��
void CGameOver::Draw()
{
	ID3D11DeviceContext* pDC = GetDeviceContext();

	SetZBuffer(false);
	SetBlendState(BS_ALPHABLEND);

	CPolygon::SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	CPolygon::SetTexture(CTextureData::GetTextureResource(GAMEOVER_BG));
	CPolygon::SetSize((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
	CPolygon::SetUV(0.0f, 0.0f);
	CPolygon::SetFrameSize(1.0f, 1.0f);
	CPolygon::SetPos(0.0f, 0.0f);
	CPolygon::Draw(pDC);

	CPolygon::SetTexture(CTextureData::GetTextureResource(GAMEOVER_GAMEOVER));
	CPolygon::SetSize(g_fTexGameOverWidth, g_fTexGameOverHeight);
	CPolygon::Draw(pDC);

	CPolygon::SetAlpha((m_nTimer <= 60) ? m_nTimer / 60.0f : (120 - m_nTimer) / 60.0f);
	CPolygon::SetTexture(CTextureData::GetTextureResource(PRESSSPACEKEY));
	CPolygon::SetSize(g_fTexStartWidth, g_fTexStartHeight);
	CPolygon::SetPos(0.0f, g_fTexStartY);
	CPolygon::Draw(pDC);
}
