#include "TitleUI_Sound.h"
#include "Input.h"
#include "Sound.h"
#include "TitleUI_Manager.h"
#include "Text.h"
#include "Polygon.h"
#include "EffekseerManager.h"
#include "TextureData.h"
#include "Fade.h"

namespace {
	// サウンド
	const XMFLOAT2 SOUND_POS_OFFSET = XMFLOAT2(-350.0f, -150.0f);

	const std::string TEXT_MASTER_WORD = std::string("Master");
	const XMFLOAT2 TEXT_MASTER_POS = XMFLOAT2(SOUND_POS_OFFSET.x, SOUND_POS_OFFSET.y + 135.0f);
	const XMFLOAT2 TEXT_MASTER_SIZE = XMFLOAT2(30.0f, 60.0f);
	const XMFLOAT4 TEXT_MASTER_COLOR = XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f);

	const std::string TEXT_BGM_WORD = std::string("BGM");
	const XMFLOAT2 TEXT_BGM_POS = XMFLOAT2(SOUND_POS_OFFSET.x, SOUND_POS_OFFSET.y + 45.0f);
	const XMFLOAT2 TEXT_BGM_SIZE = XMFLOAT2(30.0f, 60.0f);
	const XMFLOAT4 TEXT_BGM_COLOR = XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f);

	const std::string TEXT_SE_WORD = std::string("SE");
	const XMFLOAT2 TEXT_SE_POS = XMFLOAT2(SOUND_POS_OFFSET.x, SOUND_POS_OFFSET.y + -45.0f);
	const XMFLOAT2 TEXT_SE_SIZE = XMFLOAT2(30.0f, 60.0f);
	const XMFLOAT4 TEXT_SE_COLOR = XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f);

	const std::string TEXT_EXIT_WORD = std::string("Exit");
	const XMFLOAT2 TEXT_EXIT_POS = XMFLOAT2(SOUND_POS_OFFSET.x, SOUND_POS_OFFSET.y + -135.0f);
	const XMFLOAT2 TEXT_EXIT_SIZE = XMFLOAT2(30.0f, 60.0f);
	const XMFLOAT4 TEXT_EXIT_COLOR = XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f);
}

CTitleUI_Sound::CTitleUI_Sound()
{
	// カーソル初期化
	m_nCurSelect = (int)Sound_Menu::MASTER_SOUND;
	// 選択肢最大数設定
	m_nMaxSelect = (int)Sound_Menu::MAX_SOUND;
}

void CTitleUI_Sound::Update()
{
	// 選択肢変更
	MoveSelect();

	// 音量調整
	if (m_nCurSelect != EXIT_SOUND) {
		if (CInput::GetKeyPress(VK_A))
		{
			switch (m_nCurSelect)
			{
			case MASTER_SOUND:// マスター
			{
				float fVolume = CSound::GetVolume() - CSound::GetVolumeValue();
				if (fVolume < 0.0f) {
					fVolume = 0.0f;
				}
				CSound::SetVolume(fVolume);
			}
			break;
			case BGM_SOUND:// BGM
			{
				float fVolume = CSound::GetVolume(BGM_TITLE) - CSound::GetVolumeValue();
				if (fVolume < 0.0f) {
					fVolume = 0.0f;
				}
				CSound::SetVolumeBGM(fVolume);
			}
			break;
			case SE_SOUND:// SE
			{
				float fVolume = CSound::GetVolume(SE_MENUE) - CSound::GetVolumeValue();
				if (fVolume < 0.0f) {
					fVolume = 0.0f;
				}
				CSound::SetVolumeSE(fVolume);
			}
			break;
			default:
				break;
			}
		}
		if (CInput::GetKeyPress(VK_D))
		{
			switch (m_nCurSelect)
			{
			case MASTER_SOUND:// マスター
			{
				float fVolume = CSound::GetVolume() + CSound::GetVolumeValue();
				if (CSound::GetMaxVolume() < fVolume) {
					fVolume = CSound::GetMaxVolume();
				}
				CSound::SetVolume(fVolume);
			}
			break;
			case BGM_SOUND:// BGM
			{
				float fVolume = CSound::GetVolume(BGM_TITLE) + CSound::GetVolumeValue();
				if (CSound::GetMaxVolume() < fVolume) {
					fVolume = CSound::GetMaxVolume();
				}
				CSound::SetVolumeBGM(fVolume);
			}
			break;
			case SE_SOUND:// SE
			{
				float fVolume = CSound::GetVolume(SE_MENUE) + CSound::GetVolumeValue();
				if (CSound::GetMaxVolume() < fVolume) {
					fVolume = CSound::GetMaxVolume();
				}
				CSound::SetVolumeSE(fVolume);
			}
			break;
			default:
				break;
			}
		}
	}
	
	if (m_nCurSelect == Sound_Menu::EXIT_SOUND)
	{
		if (CInput::GetKeyRelease(VK_SPACE)) {
			// 決定音
			CSound::Play(SE_DECID);
			// オプションにステート変更
			CTitleUI_Manager::ChangeState(CTitleUI_Manager::TitleUIState::Option);
		}
	}
}

void CTitleUI_Sound::Draw()
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
	CPolygon::SetPos(0.0f, (TEXT_MASTER_POS.y + TEXT_EXIT_POS.y) / 2.0f);
	CPolygon::SetSize(900.0f, 400.0f);
	CPolygon::SetColor(WAKU_COLOR.x, WAKU_COLOR.y, WAKU_COLOR.z);
	CPolygon::SetAlpha(WAKU_COLOR.z);
	CPolygon::Draw(pDC);

	// サウンドメニュー
	for (int i = 0; i < MAX_SOUND; ++i)
	{

		// バー
		float fSizeRateX = CSound::GetMaxBarSizeX() / CSound::GetMaxVolume();
		float fSizeX;

		// マスター
		fSizeX = CSound::GetVolume() * fSizeRateX;
		CPolygon::SetTexture(nullptr);
		CPolygon::SetPos(-150.0f + fSizeX / 2.0f, TEXT_MASTER_POS.y);
		CPolygon::SetSize(fSizeX, 50.0f);
		CPolygon::SetColor(0.7f, 0.7f, 0.7f);
		CPolygon::SetAlpha(1.0f);
		CPolygon::Draw(pDC);

		// BGM
		fSizeX = CSound::GetVolume(BGM_TITLE) * fSizeRateX;
		//                            ↑取り出すボリュームはBGMならどれでもよい
		CPolygon::SetTexture(nullptr);
		CPolygon::SetPos(-150.0f + fSizeX / 2.0f, TEXT_BGM_POS.y);
		CPolygon::SetSize(fSizeX, 50.0f);
		CPolygon::SetColor(0.7f, 0.7f, 0.7f);
		CPolygon::SetAlpha(1.0f);
		CPolygon::Draw(pDC);

		// SE
		fSizeX = CSound::GetVolume(SE_MENUE) * fSizeRateX;
		//                            ↑取り出すボリュームはSEならどれでもよい
		CPolygon::SetTexture(nullptr);
		CPolygon::SetPos(-150.0f + fSizeX / 2.0f, TEXT_SE_POS.y);
		CPolygon::SetSize(fSizeX, 50.0f);
		CPolygon::SetColor(0.7f, 0.7f, 0.7f);
		CPolygon::SetAlpha(1.0f);
		CPolygon::Draw(pDC);

	}

	CScene::GetCurrent()->GetLight()->SetEnable(true);
}

void CTitleUI_Sound::MoveToTextPosEfk()
{
	// TODO forかつ選択肢増えてもここ触らずに
	if (Sound_Menu::MASTER_SOUND == m_nCurSelect)
	{
		// Efk位置セット
		SetEfkPos(XMFLOAT2(100.0f, 380.0f));
	}
	else if (Sound_Menu::BGM_SOUND == m_nCurSelect)
	{
		// Efk位置セット
		SetEfkPos(XMFLOAT2(100.0f, 500.0f));
	}
	else if (Sound_Menu::SE_SOUND == m_nCurSelect)
	{
		// Efk位置セット
		SetEfkPos(XMFLOAT2(100.0f, 620.0f));
	}
	else if (Sound_Menu::EXIT_SOUND == m_nCurSelect)
	{
		// Efk位置セット
		SetEfkPos(XMFLOAT2(100.0f, 740.0f));
	}
}

void CTitleUI_Sound::SetText()
{
	// 次表示テキスト設定
	m_DrawTextIndex.push_back(CText::OutPutText(TEXT_MASTER_WORD, TEXT_MASTER_POS, TEXT_MASTER_SIZE, TEXT_MASTER_COLOR));
	m_DrawTextIndex.push_back(CText::OutPutText(TEXT_BGM_WORD, TEXT_BGM_POS, TEXT_BGM_SIZE, TEXT_BGM_COLOR));
	m_DrawTextIndex.push_back(CText::OutPutText(TEXT_SE_WORD, TEXT_SE_POS, TEXT_SE_SIZE, TEXT_SE_COLOR));
	m_DrawTextIndex.push_back(CText::OutPutText(TEXT_EXIT_WORD, TEXT_EXIT_POS, TEXT_EXIT_SIZE, TEXT_EXIT_COLOR));
}