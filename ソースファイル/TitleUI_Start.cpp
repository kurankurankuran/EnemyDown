#include "TitleUI_Start.h"
#include "Input.h"
#include "Sound.h"
#include "TitleUI_Manager.h"
#include "Text.h"
#include "Polygon.h"
#include "EffekseerManager.h"
#include "TextureData.h"
#include "Fade.h"

namespace {
	// �X�^�[�g
	const std::string TEXT_START_WORD = std::string("Press Space Button");
	const XMFLOAT2 TEXT_START_POS = XMFLOAT2(100.0f, -150.0f);
	const XMFLOAT2 TEXT_START_SIZE = XMFLOAT2(30.0f, 60.0f);
	const XMFLOAT4 TEXT_START_COLOR = XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f);
}

void CTitleUI_Start::Update()
{
	// �t�F�[�h���͏������Ȃ�
	if (CFade::Get() != FADE_NONE) return;
	if (CInput::GetKeyRelease(VK_SPACE) ||
		CInput::GetKeyRelease(VK_RETURN)) {
		// ���艹
		CSound::Play(SE_DECID);
		//�X�e�[�g�ύX����
		CTitleUI_Manager::ChangeState(CTitleUI_Manager::TitleUIState::Menu);
	}
}

void CTitleUI_Start::Draw()
{
	ID3D11DeviceContext* pDC = GetDeviceContext();
	CScene::GetCurrent()->GetLight()->SetEnable(false);
	SetZBuffer(false);
	SetBlendState(BS_ALPHABLEND);

	// �^�C�g�����S
	CPolygon::SetTexture(CTextureData::GetTextureResource(TITLE_LOG)/*m_pTexTitle*/);
	CPolygon::SetPos(0.0f, 0.0f);
	CPolygon::SetSize(TITLE_WIDTH, TITLE_HEIGHT);
	CPolygon::SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	CPolygon::Draw(pDC);

	// �g
	CPolygon::SetTexture(nullptr);
	CPolygon::SetPos(350.0f, TEXT_START_POS.y);
	CPolygon::SetSize(550.0f, 80.0f);
	CPolygon::SetColor(WAKU_COLOR.x, WAKU_COLOR.y, WAKU_COLOR.z);
	CPolygon::SetAlpha(WAKU_COLOR.z);
	CPolygon::Draw(pDC);

	CScene::GetCurrent()->GetLight()->SetEnable(true);
}


void CTitleUI_Start::MoveToTextPosEfk()
{
	// Efk�ʒu�Z�b�g
	SetEfkPos(XMFLOAT2(850.0f, 630.0f));
}

void CTitleUI_Start::SetText()
{
	// �\���e�L�X�g�ݒ�
	m_DrawTextIndex.push_back(CText::OutPutText(TEXT_START_WORD, TEXT_START_POS, TEXT_START_SIZE, TEXT_START_COLOR));
}