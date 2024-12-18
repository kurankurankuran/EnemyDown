#include "TitleUI_Option.h"
#include "Input.h"
#include "Sound.h"
#include "TitleUI_Manager.h"
#include "Text.h"
#include "Polygon.h"
#include "EffekseerManager.h"
#include "TextureData.h"
#include "Fade.h"

namespace {
	// オプション
	const std::string TEXT_INPUT_WORD = std::string("Input");
	const XMFLOAT2 TEXT_INPUT_POS = XMFLOAT2(200.0f, -200.0f);
	const XMFLOAT2 TEXT_INPUT_SIZE = XMFLOAT2(30.0f, 60.0f);
	const XMFLOAT4 TEXT_INPUT_COLOR = XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f);

	const std::string TEXT_SOUND_WORD = std::string("Sound");
	const XMFLOAT2 TEXT_SOUND_POS = XMFLOAT2(200.0f, -260.0f);
	const XMFLOAT2 TEXT_SOUND_SIZE = XMFLOAT2(30.0f, 60.0f);
	const XMFLOAT4 TEXT_SOUND_COLOR = XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f);

	const std::string TEXT_EXIT_OPTION_WORD = std::string("Exit");
	const XMFLOAT2 TEXT_EXIT_OPTION_POS = XMFLOAT2(200.0f, -320.0f);
	const XMFLOAT2 TEXT_EXIT_OPTION_SIZE = XMFLOAT2(30.0f, 60.0f);
	const XMFLOAT4 TEXT_EXIT_OPTION_COLOR = XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f);
}

CTitleUI_Option::CTitleUI_Option()
{
	// カーソル初期化
	m_nCurSelect = (int)Option_Select::INPUT_SELECT;
	// 選択肢最大数設定
	m_nMaxSelect = (int)Option_Select::MAX_SELECT;
}

void CTitleUI_Option::Update()
{
	// フェード中は処理しない
	if (CFade::Get() != FADE_NONE) return;
	// 選択肢変更
	MoveSelect();
	
	// 決定処理
	if (CInput::GetKeyRelease(VK_SPACE)) {
		// 決定音
		CSound::Play(SE_DECID);
		// 現在のカーソル位置による処理
		switch ((Option_Select)m_nCurSelect)
		{
		case Option_Select::INPUT_SELECT:
			// インプットにステート変更
			CTitleUI_Manager::ChangeState(CTitleUI_Manager::TitleUIState::Input);
			break;
		case Option_Select::SOUND_SELECT:
			// サウンドにステート変更
			CTitleUI_Manager::ChangeState(CTitleUI_Manager::TitleUIState::Sound);
			break;
		case Option_Select::EXIT_SELECT:
			// メニューにステート変更
			CTitleUI_Manager::ChangeState(CTitleUI_Manager::TitleUIState::Menu);
			break;
		default:
			break;
		}
	}
}

void CTitleUI_Option::Draw()
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

	// 枠
	CPolygon::SetTexture(nullptr);
	CPolygon::SetPos(325.0f, (TEXT_SOUND_POS.y + TEXT_EXIT_OPTION_POS.y) / 2.0f);
	CPolygon::SetSize(500.0f, 300.0f);
	CPolygon::SetColor(WAKU_COLOR.x, WAKU_COLOR.y, WAKU_COLOR.z);
	CPolygon::SetAlpha(WAKU_COLOR.z);
	CPolygon::Draw(pDC);

	CScene::GetCurrent()->GetLight()->SetEnable(true);
}

void CTitleUI_Option::MoveToTextPosEfk()
{
	if ((int)Option_Select::INPUT_SELECT == m_nCurSelect)
	{
		// Efk位置セット
		SetEfkPos(XMFLOAT2(850.0f, 630.0f));
	}
	else if ((int)Option_Select::SOUND_SELECT == m_nCurSelect)
	{
		// Efk位置セット
		SetEfkPos(XMFLOAT2(850.0f, 710.0f));
	}
	else if ((int)Option_Select::EXIT_SELECT == m_nCurSelect)
	{
		// Efk位置セット
		SetEfkPos(XMFLOAT2(850.0f, 790.0f));
	}
}

void CTitleUI_Option::SetText()
{
	// 表示テキスト設定
	m_DrawTextIndex.push_back(CText::OutPutText(TEXT_INPUT_WORD, TEXT_INPUT_POS, TEXT_INPUT_SIZE, TEXT_INPUT_COLOR));
	m_DrawTextIndex.push_back(CText::OutPutText(TEXT_SOUND_WORD, TEXT_SOUND_POS, TEXT_SOUND_SIZE, TEXT_SOUND_COLOR));
	m_DrawTextIndex.push_back(CText::OutPutText(TEXT_EXIT_OPTION_WORD, TEXT_EXIT_OPTION_POS, TEXT_EXIT_OPTION_SIZE, TEXT_EXIT_OPTION_COLOR));
}