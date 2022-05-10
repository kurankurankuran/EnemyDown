#include "TitleUI_Input.h"
#include "Input.h"
#include "Sound.h"
#include "TitleUI_Manager.h"
#include "Text.h"
#include "Polygon.h"
#include "EffekseerManager.h"
#include "TextureData.h"
#include "Fade.h"

namespace {
	// インプット
	const XMFLOAT2 INPUT_POS_OFFSET = XMFLOAT2(-400.0f, -150.0f);

	const std::string TEXT_SENSI_X_WORD = std::string("Horizontal");
	const XMFLOAT2 TEXT_SENSI_X_POS = XMFLOAT2(INPUT_POS_OFFSET.x, INPUT_POS_OFFSET.y + 135.0f);
	const XMFLOAT2 TEXT_SENSI_X_SIZE = XMFLOAT2(30.0f, 60.0f);
	const XMFLOAT4 TEXT_SENSI_X_COLOR = XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f);

	const std::string TEXT_SENSI_Y_WORD = std::string("Vertical");
	const XMFLOAT2 TEXT_SENSI_Y_POS = XMFLOAT2(INPUT_POS_OFFSET.x, INPUT_POS_OFFSET.y + 45.0f);
	const XMFLOAT2 TEXT_SENSI_Y_SIZE = XMFLOAT2(30.0f, 60.0f);
	const XMFLOAT4 TEXT_SENSI_Y_COLOR = XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f);

	const std::string TEXT_EXIT_INPUT_WORD = std::string("Exit");
	const XMFLOAT2 TEXT_EXIT_INPUT_POS = XMFLOAT2(INPUT_POS_OFFSET.x, INPUT_POS_OFFSET.y + -45.0f);
	const XMFLOAT2 TEXT_EXIT_INPUT_SIZE = XMFLOAT2(30.0f, 60.0f);
	const XMFLOAT4 TEXT_EXIT_INPUT_COLOR = XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f);

}

CTitleUI_Input::CTitleUI_Input()
{
	// カーソル初期化
	m_nCurSelect = (int)Input_Menu::MOUSESENSI_X_INPUT;
	// 選択肢最大数設定
	m_nMaxSelect = (int)Input_Menu::MAX_INPUT;
}

void CTitleUI_Input::Update()
{
	// 選択肢変更
	MoveSelect();

	// 感度調整
	if (CInput::GetKeyPress(VK_A))
	{
		switch (m_nCurSelect)
		{
		case MOUSESENSI_X_INPUT:// X感度
		{
			float fMouseSensi = CInput::GetMouseSensitivityX() - CInput::GetMouseSensiValRate();
			if (fMouseSensi < 0.0f) {
				fMouseSensi = 0.0f;
			}
			CInput::SetMouseSensitivityX(fMouseSensi);
		}	break;
		case MOUSESENSI_Y_INPUT:// Y感度
		{
			float fMouseSensi = CInput::GetMouseSensitivityY() - CInput::GetMouseSensiValRate();
			if (fMouseSensi < 0.0f) {
				fMouseSensi = 0.0f;
			}
			CInput::SetMouseSensitivityY(fMouseSensi);
		}	break;
		default:
			break;
		}
	}
	if (CInput::GetKeyPress(VK_D))
	{
		switch (m_nCurSelect)
		{
		case MOUSESENSI_X_INPUT:// X感度
		{
			float fMouseSensi = CInput::GetMouseSensitivityX() + CInput::GetMouseSensiValRate();
			if (CInput::GetMouseSensitivityMax() < fMouseSensi) {
				fMouseSensi = CInput::GetMouseSensitivityX();
			}
			CInput::SetMouseSensitivityX(fMouseSensi);
		}	break;
		case MOUSESENSI_Y_INPUT:// Y感度
		{
			float fMouseSensi = CInput::GetMouseSensitivityY() + CInput::GetMouseSensiValRate();
			if (CInput::GetMouseSensitivityMax() < fMouseSensi) {
				fMouseSensi = CInput::GetMouseSensitivityY();
			}
			CInput::SetMouseSensitivityY(fMouseSensi);
		}	break;
		default:
			break;
		}

	}
	
	if (m_nCurSelect == Input_Menu::EXIT_INPUT) 
	{
		if (CInput::GetKeyRelease(VK_SPACE)) {
			// 決定音
			CSound::Play(SE_DECID);
			// オプションにステート変更
			CTitleUI_Manager::ChangeState(CTitleUI_Manager::TitleUIState::Option);
		}
	}
}

void CTitleUI_Input::Draw()
{
	ID3D11DeviceContext* pDC = GetDeviceContext();
	CScene::GetCurrent()->GetLight()->SetEnable(false);
	SetZBuffer(false);
	SetBlendState(BS_ALPHABLEND);

	// タイトルロゴ
	CPolygon::SetTexture(CTextureData::GetTextureResource(TITLE_LOG)/*m_pTexTitle*/);
	CPolygon::SetPos(0.0f, 0.0f);
	CPolygon::SetSize(TITLE_WIDTH, TITLE_HEIGHT);
	CPolygon::SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	CPolygon::Draw(pDC);

	// プレート
	CPolygon::SetTexture(nullptr);
	CPolygon::SetPos(0.0f, -150.0f);
	CPolygon::SetSize(1000.0f, 400.0f);
	CPolygon::SetColor(0.5f, 0.3f, 0.1f);
	CPolygon::SetAlpha(0.4f);
	CPolygon::Draw(pDC);


	// インプットメニュー
	for (int i = 0; i < MAX_INPUT; ++i)
	{
		// バー
		switch (i)
		{
		case MOUSESENSI_X_INPUT:
		{
			float fSizeRateX = CInput::GetMaxBarSize() / CInput::GetMouseSensitivityMax();
			float fSizeX = CInput::GetMouseSensitivityX() * fSizeRateX;

			CPolygon::SetTexture(nullptr);
			CPolygon::SetPos(-100.0f + fSizeX / 2.0f, TEXT_SENSI_X_POS.y);
			CPolygon::SetSize(fSizeX, 50.0f);
			CPolygon::SetColor(0.0f, 1.0f, 1.0f);
			CPolygon::SetAlpha(1.0f);
			CPolygon::Draw(pDC);
		}
		break;
		case MOUSESENSI_Y_INPUT:
		{
			float fSizeRateX = CInput::GetMaxBarSize() / CInput::GetMouseSensitivityMax();
			float fSizeX = CInput::GetMouseSensitivityY() * fSizeRateX;

			CPolygon::SetTexture(nullptr);
			CPolygon::SetPos(-100.0f + fSizeX / 2.0f, TEXT_SENSI_Y_POS.y);
			CPolygon::SetSize(fSizeX, 50.0f);
			CPolygon::SetColor(0.0f, 1.0f, 1.0f);
			CPolygon::SetAlpha(1.0f);
			CPolygon::Draw(pDC);
		}
		break;
		default:
			break;
		}
	}

	CScene::GetCurrent()->GetLight()->SetEnable(true);
}

void CTitleUI_Input::MoveToTextPosEfk()
{
	if (Input_Menu::MOUSESENSI_X_INPUT == m_nCurSelect)
	{
		// Efk位置セット
		SetEfkPos(XMFLOAT2(50.0f, 380.0f));
	}
	else if (Input_Menu::MOUSESENSI_Y_INPUT == m_nCurSelect)
	{
		// Efk位置セット
		SetEfkPos(XMFLOAT2(50.0f, 500.0f));
	}
	else if (Input_Menu::EXIT_INPUT == m_nCurSelect)
	{
		// Efk位置セット
		SetEfkPos(XMFLOAT2(50.0f, 620.0f));
	}
}

void CTitleUI_Input::SetText()
{
	// 次表示テキスト設定
	m_DrawTextIndex.push_back(CText::OutPutText(TEXT_SENSI_X_WORD, TEXT_SENSI_X_POS, TEXT_SENSI_X_SIZE, TEXT_SENSI_X_COLOR));
	m_DrawTextIndex.push_back(CText::OutPutText(TEXT_SENSI_Y_WORD, TEXT_SENSI_Y_POS, TEXT_SENSI_Y_SIZE, TEXT_SENSI_Y_COLOR));
	m_DrawTextIndex.push_back(CText::OutPutText(TEXT_EXIT_INPUT_WORD, TEXT_EXIT_INPUT_POS, TEXT_EXIT_INPUT_SIZE, TEXT_EXIT_INPUT_COLOR));
}