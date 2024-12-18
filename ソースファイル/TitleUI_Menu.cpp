#include "TitleUI_Menu.h"
#include "Input.h"
#include "Sound.h"
#include "TitleUI_Manager.h"
#include "Text.h"
#include "Polygon.h"
#include "EffekseerManager.h"
#include "TextureData.h"
#include "Fade.h"

namespace {
	// ���j���[
	const std::string TEXT_SELECT_WORD = std::string("Game Start");

	const XMFLOAT2 TEXT_SELECT_POS = XMFLOAT2(200.0f, -200.0f);
	const XMFLOAT2 TEXT_SELECT_SIZE = XMFLOAT2(30.0f, 60.0f);
	const XMFLOAT4 TEXT_SELECT_COLOR = XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f);

	const std::string TEXT_OPTION_WORD = std::string("Option");
	const XMFLOAT2 TEXT_OPTION_POS = XMFLOAT2(200.0f, -260.0f);
	const XMFLOAT2 TEXT_OPTION_SIZE = XMFLOAT2(30.0f, 60.0f);
	const XMFLOAT4 TEXT_OPTION_COLOR = XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f);
}

CTitleUI_Menu::CTitleUI_Menu()
{
	// �J�[�\��������
	m_nCurSelect = (int)Menu_Select::STAGESELECT_SELECT;
	// �I�����ő吔�ݒ�
	m_nMaxSelect = (int)Menu_Select::MAX_SELECT;
}


void CTitleUI_Menu::Update()
{
	// �I�����ύX
	MoveSelect();

	// ���菈��
	if (CInput::GetKeyRelease(VK_SPACE)) {
		// ���艹
		CSound::Play(SE_DECID);
		// ���݂̃J�[�\���ʒu�ɂ�鏈��
		switch ((Menu_Select)m_nCurSelect)
		{
		case Menu_Select::STAGESELECT_SELECT:// �X�e�[�W�Z���N�g
			// �t�F�[�h����
			CFade::Out(SCENE_GAME);
			// Efk�폜
			CEffekseerManager::Stop(m_nEfkHandle, true);
			// �V�[���`�F���W���艹
			CSound::Play(SE_DECID_SCENECHANGE);
			break;
		case Menu_Select::OPTION_SELECT:// �I�v�V����
			// �^�C�g���X�e�[�g�ύX
			CTitleUI_Manager::ChangeState(CTitleUI_Manager::TitleUIState::Option);
			break;

		default:
			break;
		}
	}
}

void CTitleUI_Menu::Draw()
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
	CPolygon::SetPos(325.0f, (TEXT_SELECT_POS.y + TEXT_OPTION_POS.y) / 2.0f);
	CPolygon::SetSize(500.0f, 200.0f);
	CPolygon::SetColor(WAKU_COLOR.x, WAKU_COLOR.y, WAKU_COLOR.z);
	CPolygon::SetAlpha(WAKU_COLOR.z);
	CPolygon::Draw(pDC);

	CScene::GetCurrent()->GetLight()->SetEnable(true);
}


void CTitleUI_Menu::MoveToTextPosEfk()
{
	if ((int)Menu_Select::STAGESELECT_SELECT == m_nCurSelect)
	{
		// Efk�ʒu�Z�b�g
		SetEfkPos(XMFLOAT2(850.0f, 630.0f));
	}
	else if ((int)Menu_Select::OPTION_SELECT == m_nCurSelect)
	{
		// Efk�ʒu�Z�b�g
		SetEfkPos(XMFLOAT2(850.0f, 710.0f));
	}
}

void CTitleUI_Menu::SetText()
{
	// �\���e�L�X�g�ݒ�
	m_DrawTextIndex.push_back(CText::OutPutText(TEXT_SELECT_WORD, TEXT_SELECT_POS, TEXT_SELECT_SIZE, TEXT_SELECT_COLOR));
	m_DrawTextIndex.push_back(CText::OutPutText(TEXT_OPTION_WORD, TEXT_OPTION_POS, TEXT_OPTION_SIZE, TEXT_OPTION_COLOR));
}