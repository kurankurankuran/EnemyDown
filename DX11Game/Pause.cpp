//=============================================================================
//
// 一時停止処理 [pause.cpp]
// Author : RYUSUKE	YAMADA
//
//=============================================================================
#include "Pause.h"
#include "fade.h"
#include "Polygon.h"
#include "Texture.h"
#include "Sound.h"
//#include "Scene.h"
#include "Fade.h"
#include "Text.h"
#include "EffekseerManager.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
namespace {
	const float PLATE_WIDTH = 400.0f;                       // プレートの幅(横)
	const float PLATE_HEIGHT = 430.0f;                      // プレートの幅(縦)
	const float PLATE_POS_X = 0.0f;                         // プレートの位置(X座標)
	const float PLATE_POS_Y = 0.0f;                         // プレートの位置(Y座標)

	// 操作説明
	const std::string TEXT_MOVE_WORD = std::string("Move / WASD");
	const XMFLOAT2    TEXT_MOVE_POS = XMFLOAT2(-425.0f, 175.0f);
	const XMFLOAT2    TEXT_MOVE_SIZE = XMFLOAT2(25.0f, 50.0f);
	const XMFLOAT4    TEXT_MOVE_COLOR = XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f);

	const std::string TEXT_DASH_WORD = std::string("Dash / LSHIFT");
	const XMFLOAT2    TEXT_DASH_POS = XMFLOAT2(-425.0f, 100.0f);
	const XMFLOAT2    TEXT_DASH_SIZE = XMFLOAT2(25.0f, 50.0f);
	const XMFLOAT4    TEXT_DASH_COLOR = XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f);

	const std::string TEXT_JUMP_WORD = std::string("Jump / SPACE");
	const XMFLOAT2    TEXT_JUMP_POS = XMFLOAT2(-425.0f, 25.0f);
	const XMFLOAT2    TEXT_JUMP_SIZE = XMFLOAT2(25.0f, 50.0f);
	const XMFLOAT4    TEXT_JUMP_COLOR = XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f);

	const std::string TEXT_SATK_WORD = std::string("SAtk / LCLICK");
	const XMFLOAT2    TEXT_SATK_POS = XMFLOAT2(-425.0f, -50.0f);
	const XMFLOAT2    TEXT_SATK_SIZE = XMFLOAT2(25.0f, 50.0f);
	const XMFLOAT4    TEXT_SATK_COLOR = XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f);

	const std::string TEXT_BATK1_WORD = std::string("BAtk / RCLICK");
	const XMFLOAT2    TEXT_BATK1_POS = XMFLOAT2(-425.0f, -125.0f);
	const XMFLOAT2    TEXT_BATK1_SIZE = XMFLOAT2(25.0f, 50.0f);
	const XMFLOAT4    TEXT_BATK1_COLOR = XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f);

	const std::string TEXT_BATK2_WORD = std::string("     + LCLICK");
	const XMFLOAT2    TEXT_BATK2_POS = XMFLOAT2(-425.0f, -175.0f);
	const XMFLOAT2    TEXT_BATK2_SIZE = XMFLOAT2(25.0f, 50.0f);
	const XMFLOAT4    TEXT_BATK2_COLOR = XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f);

	// オプションメニュー
	const std::string TEXT_CONTINUE_WORD = std::string("Continue");
	const XMFLOAT2    TEXT_CONTINUE_POS = XMFLOAT2(225.0f, 175.0f);
	const XMFLOAT2    TEXT_CONTINUE_SIZE = XMFLOAT2(30.0f, 60.0f);
	const XMFLOAT4    TEXT_CONTINUE_COLOR = XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f);

	const std::string TEXT_RETRY_WORD = std::string("Retry");
	const XMFLOAT2    TEXT_RETRY_POS = XMFLOAT2(225.0f, 100.0f);
	const XMFLOAT2    TEXT_RETRY_SIZE = XMFLOAT2(30.0f, 60.0f);
	const XMFLOAT4    TEXT_RETRY_COLOR = XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f);

	const std::string TEXT_INPUT_WORD = std::string("Input");
	const XMFLOAT2    TEXT_INPUT_POS = XMFLOAT2(225.0f, 25.0f);
	const XMFLOAT2    TEXT_INPUT_SIZE = XMFLOAT2(30.0f, 60.0f);
	const XMFLOAT4    TEXT_INPUT_COLOR = XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f);

	const std::string TEXT_SOUND_WORD = std::string("Sound");
	const XMFLOAT2    TEXT_SOUND_POS = XMFLOAT2(225.0f, -50.0f);
	const XMFLOAT2    TEXT_SOUND_SIZE = XMFLOAT2(30.0f, 60.0f);
	const XMFLOAT4    TEXT_SOUND_COLOR = XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f);

	const std::string TEXT_TITLE_WORD = std::string("Title");
	const XMFLOAT2    TEXT_TITLE_POS = XMFLOAT2(225.0f, -125.0f);
	const XMFLOAT2    TEXT_TITLE_SIZE = XMFLOAT2(30.0f, 60.0f);
	const XMFLOAT4    TEXT_TITLE_COLOR = XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f);

	// インプットメニュー
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

	// サウンドメニュー
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

	const std::string TEXT_EXIT_SOUND_WORD = std::string("Exit");
	const XMFLOAT2 TEXT_EXIT_SOUND_POS = XMFLOAT2(SOUND_POS_OFFSET.x, SOUND_POS_OFFSET.y + -135.0f);
	const XMFLOAT2 TEXT_EXIT_SOUND_SIZE = XMFLOAT2(30.0f, 60.0f);
	const XMFLOAT4 TEXT_EXIT_SOUND_COLOR = XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f);
}


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
namespace {
	
	//const float CSound::GetVolumeValue() = 0.02f; // 音量変化量
	//const float CSound::GetMaxVolume() = 10.0f; // 最大音量
	//const float CSound::GetMaxBarSizeX() = 500.0f;


	const int SCALE_RATE = 1;             // 拡縮速度
	const float COLOR_RATE = 0.01f;       // 色変化速度
	const float SELECT_INTERVAL = -100.0f; // 枠間の距離間隔
	const float SELECT_SIZE_X = 240.0f;   // 画像の横サイズ
	const float SELECT_SIZE_Y = 200.0f;   // 画像の縦サイズ
	const float FRAME_WIDTH = 20.0f;      // 枠の幅サイズ

	const int MAX_SCALE_UP = 40;       // 膨張時最大
	const int MIN_SCALE_DOWN = -10;    // 縮小時最小

	
	
}


CPause::CPause()
{
	m_pCamera = nullptr;
}

CPause::~CPause()
{

}


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CPause::Init()
{
	m_nSelectMenu = PAUSE_MENU_CONTINUE;
	m_nCurInput = MOUSESENSI_X_INPUT;
	m_nCurSound = MASTER_SOUND; 
	m_bPause = false;
	m_State = None;
	m_TextMap.clear();
	m_nEfkHandle = -1;

	return S_OK;
}



//=============================================================================
// 終了処理
//=============================================================================
void CPause::Fin()
{
	
}



//=============================================================================
// 更新処理
//=============================================================================

void CPause::Update()
{
	if (CInput::GetKeyRelease(VK_Q)) {
		m_bPause = !m_bPause;
		//m_bSound = false;

		if (m_bPause)
		{
			// ステート変更
			m_State = Menu;

			// マウスカーソル非固定
			CInput::SetRockCursorFlag(false);
			// マウスカーソルの表示
			ShowCursor(TRUE);
			// メニューテキスト表示
			OutputMenuText();
		}
		else
		{
			// ステート変更
			m_State = None;

			// マウスカーソル固定
			CInput::SetRockCursorFlag(true);
			// マウスカーソルの非表示
			ShowCursor(FALSE);
			// テキスト全消去
			DeleteTextAll();
		}
	}

	switch (m_State)
	{
	case None:
		NoneUpdate();
		break;
	case Menu:
		MenuUpdate();
		break;
	case Input:
		InputUpdate();
	break; case Sound:
		SoundUpdate();
		break;
	default:
		break;
	}


}

//=============================================================================
// 描画処理
//=============================================================================

void CPause::Draw()
{
	if (!m_bPause)return;

	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// 半透明部分を描画
	SetBlendState(BS_ALPHABLEND);	// アルファブレンド有効
	SetZWrite(false);								// Zバッファ更新しない

	CPolygon::SetFrameSize(1.0f, 1.0f);
	CPolygon::SetUV(0.0f, 0.0f);

	switch (m_State)
	{
	case PauseState::Menu:
		// 左プレート
		CPolygon::SetTexture(nullptr);
		CPolygon::SetSize(PLATE_WIDTH, PLATE_HEIGHT);
		CPolygon::SetPos(PLATE_POS_X - 300.0f, PLATE_POS_Y);
		CPolygon::SetColor(1.0f, 0.2f, 0.2f/* g_fCol, g_fCol, g_fCol */);
		CPolygon::SetAlpha(0.4f);
		CPolygon::Draw(pDeviceContext);

		// 右プレート
		CPolygon::SetTexture(nullptr);
		CPolygon::SetSize(PLATE_WIDTH, PLATE_HEIGHT);
		CPolygon::SetPos(PLATE_POS_X + 300, PLATE_POS_Y);
		CPolygon::SetFrameSize(1.0f, 1.0f);
		CPolygon::SetUV(0.0f, 0.0f);
		CPolygon::SetColor(1.0f, 0.2f, 0.2f/* g_fCol, g_fCol, g_fCol */);
		CPolygon::SetAlpha(0.4f);
		CPolygon::Draw(pDeviceContext);
		break;

	case PauseState::Input:
		InputDraw(pDeviceContext);
		break;
	case PauseState::Sound:
		SoundDraw(pDeviceContext);
		break;
	default:
		break;
	}
	/*if (PauseState::Sound != m_State)
	{
		


	}
	else
	{
		
	}
*/
	CPolygon::SetColor(0.0f, 0.0f, 0.0f);
	SetZWrite(true);							// Zバッファ更新する
	SetBlendState(BS_NONE);		// アルファブレンド無効
}



//=============================================================================
// ポーズメニューの取得
//=============================================================================
Pause_Menu CPause::GetPauseMenu(void)
{
	return m_nSelectMenu;
}

//=============================================================================
// ポーズメニューのリセット
//=============================================================================
void CPause::ResetPauseMenu(void)
{
	m_nSelectMenu = PAUSE_MENU_CONTINUE;
	CSound::Play(SE_MENUE);
	//SetPauseMenu();
}


void CPause::NoneUpdate()
{
	
}

void CPause::NoneDraw(ID3D11DeviceContext* pDC)
{

}

void CPause::MenuUpdate()
{
	if (CInput::GetKeyTrigger(VK_W) /*|| m_bInputUpNow && !m_bInputUpOld*/)
	{
		// 色を元に戻す
		switch (m_nSelectMenu)
		{
		case PAUSE_MENU_CONTINUE:
			CText::SetColor(m_TextMap[TEXT_CONTINUE_WORD], TEXT_CONTINUE_COLOR);
			break;
		case PAUSE_MENU_RETRY:
			CText::SetColor(m_TextMap[TEXT_RETRY_WORD], TEXT_RETRY_COLOR);
			break;
		case PAUSE_MENU_INPUT:
			CText::SetColor(m_TextMap[TEXT_INPUT_WORD], TEXT_INPUT_COLOR);
			break;
		case PAUSE_MENU_SOUND:
			CText::SetColor(m_TextMap[TEXT_SOUND_WORD], TEXT_SOUND_COLOR);
			break;
		case PAUSE_MENU_QUIT:
			CText::SetColor(m_TextMap[TEXT_TITLE_WORD], TEXT_TITLE_COLOR);
			break;
		default:
			break;
		}

		CSound::Play(SE_MENUE);
		m_nSelectMenu = (Pause_Menu)((m_nSelectMenu + PAUSE_MENU_MAX - 1) % PAUSE_MENU_MAX);
		//SetPauseMenu();

	}
	else if (CInput::GetKeyTrigger(VK_S) /*|| m_bInputDownNow && !m_bInputDownOld*/)
	{
		// 色を元に戻す
		switch (m_nSelectMenu)
		{
		case PAUSE_MENU_CONTINUE:
			CText::SetColor(m_TextMap[TEXT_CONTINUE_WORD], TEXT_CONTINUE_COLOR);
			break;
		case PAUSE_MENU_RETRY:
			CText::SetColor(m_TextMap[TEXT_RETRY_WORD], TEXT_RETRY_COLOR);
			break;
		case PAUSE_MENU_INPUT:
			CText::SetColor(m_TextMap[TEXT_INPUT_WORD], TEXT_INPUT_COLOR);
			break;
		case PAUSE_MENU_SOUND:
			CText::SetColor(m_TextMap[TEXT_SOUND_WORD], TEXT_SOUND_COLOR);
			break;
		case PAUSE_MENU_QUIT:
			CText::SetColor(m_TextMap[TEXT_TITLE_WORD], TEXT_TITLE_COLOR);
			break;
		default:
			break;
		}
		CSound::Play(SE_MENUE);
		m_nSelectMenu = (Pause_Menu)((m_nSelectMenu + 1) % PAUSE_MENU_MAX);
		//SetPauseMenu();

		
	}

	// 色を変える
	switch (m_nSelectMenu)
	{
	case PAUSE_MENU_CONTINUE:
		CText::SetColor(m_TextMap[TEXT_CONTINUE_WORD], XMFLOAT4(TEXT_CONTINUE_COLOR.x, TEXT_CONTINUE_COLOR.y, 1.0f, TEXT_CONTINUE_COLOR.w));
		break;
	case PAUSE_MENU_RETRY:
		CText::SetColor(m_TextMap[TEXT_RETRY_WORD], XMFLOAT4(TEXT_RETRY_COLOR.x, TEXT_RETRY_COLOR.y, 1.0f, TEXT_RETRY_COLOR.w));
		break;
	case PAUSE_MENU_INPUT:
		CText::SetColor(m_TextMap[TEXT_INPUT_WORD], XMFLOAT4(TEXT_INPUT_COLOR.x, TEXT_INPUT_COLOR.y, 1.0f, TEXT_INPUT_COLOR.w));
		break;
	case PAUSE_MENU_SOUND:
		CText::SetColor(m_TextMap[TEXT_SOUND_WORD], XMFLOAT4(TEXT_SOUND_COLOR.x, TEXT_SOUND_COLOR.y, 1.0f, TEXT_SOUND_COLOR.w));
		break;
	case PAUSE_MENU_QUIT:
		CText::SetColor(m_TextMap[TEXT_TITLE_WORD], XMFLOAT4(TEXT_TITLE_COLOR.x, TEXT_TITLE_COLOR.y, 1.0f, TEXT_TITLE_COLOR.w));
		break;
	default:
		break;
	}

	if (CInput::GetKeyRelease(VK_SPACE)) 
	{
		switch (m_nSelectMenu)
		{
		case PAUSE_MENU_CONTINUE:
			m_bPause = false;
			// ステート変更
			m_State = None;
			// マウスカーソル固定
			CInput::SetRockCursorFlag(true);
			// マウスカーソルの非表示
			ShowCursor(FALSE);
			// メニューテキスト削除
			DeleteMenuText();
			// 決定音
			CSound::Play(SE_DECID);
			break;
		case PAUSE_MENU_RETRY:
			CFade::Out(CScene::GetCurrent()->GetID());
			// シーンチェンジ決定音
			CSound::Play(SE_DECID_SCENECHANGE);
			break;
		case PAUSE_MENU_INPUT:
			// ステート変更
			m_State = PauseState::Input;
			// メニューテキスト削除
			DeleteMenuText();
			// インプットテキスト表示
			OutputInputText();
			// 決定音
			CSound::Play(SE_DECID);
			break;
		case PAUSE_MENU_SOUND:
			// ステート変更
			m_State = PauseState::Sound;
			// メニューテキスト削除
			DeleteMenuText();
			// サウンドテキスト表示
			OutputSoundText();
			// 決定音
			CSound::Play(SE_DECID);
			break;
		case PAUSE_MENU_QUIT:
			m_bPause = false;
			// ステート変更
			m_State = None;
			// テキスト削除
			DeleteTextAll();
			// シーンチェンジ
			CFade::Out(SCENE_TITLE);
			// シーンチェンジ決定音
			CSound::Play(SE_DECID_SCENECHANGE);
			break;
		default:
			break;
		}
	}
}

void CPause::MenuDraw(ID3D11DeviceContext* pDC)
{

}

void CPause::InputUpdate()
{
	if (CInput::GetKeyTrigger(VK_W))
	{
		// 色を元に戻す
		switch (m_nCurInput)
		{
		case MOUSESENSI_X_INPUT:
			CText::SetColor(m_TextMap[TEXT_SENSI_X_WORD], TEXT_SENSI_X_COLOR);
			break;
		case MOUSESENSI_Y_INPUT:
			CText::SetColor(m_TextMap[TEXT_SENSI_Y_WORD], TEXT_SENSI_X_COLOR);
			break;
		case EXIT_INPUT:
			CText::SetColor(m_TextMap[TEXT_EXIT_INPUT_WORD], TEXT_EXIT_INPUT_COLOR);
			break;
		default:
			break;
		}


		CSound::Play(SE_MENUE);
		m_nCurInput = (Input_Menu)((m_nCurInput + MAX_INPUT - 1) % MAX_INPUT);

	}
	else if (CInput::GetKeyTrigger(VK_S))
	{
		// 色を元に戻す
		switch (m_nCurInput)
		{
		case MOUSESENSI_X_INPUT:
			CText::SetColor(m_TextMap[TEXT_SENSI_X_WORD], TEXT_SENSI_X_COLOR);
			break;
		case MOUSESENSI_Y_INPUT:
			CText::SetColor(m_TextMap[TEXT_SENSI_Y_WORD], TEXT_SENSI_X_COLOR);
			break;
		case EXIT_INPUT:
			CText::SetColor(m_TextMap[TEXT_EXIT_INPUT_WORD], TEXT_EXIT_INPUT_COLOR);
			break;
		default:
			break;
		}

		CSound::Play(SE_MENUE);
		m_nCurInput = (Input_Menu)((m_nCurInput + 1) % MAX_INPUT);

	}

	// 色を変える
	switch (m_nCurInput)
	{
	case MOUSESENSI_X_INPUT:
		CText::SetColor(m_TextMap[TEXT_SENSI_X_WORD], XMFLOAT4(TEXT_SENSI_X_COLOR.x, TEXT_SENSI_X_COLOR.y, 1.0f, TEXT_SENSI_X_COLOR.w));
		break;
	case MOUSESENSI_Y_INPUT:
		CText::SetColor(m_TextMap[TEXT_SENSI_Y_WORD], XMFLOAT4(TEXT_SENSI_Y_COLOR.x, TEXT_SENSI_Y_COLOR.y, 1.0f, TEXT_SENSI_Y_COLOR.w));
		break;
	case EXIT_INPUT:
		CText::SetColor(m_TextMap[TEXT_EXIT_INPUT_WORD], XMFLOAT4(TEXT_EXIT_INPUT_COLOR.x, TEXT_EXIT_INPUT_COLOR.y, 1.0f, TEXT_EXIT_INPUT_COLOR.w));
		break;
	default:
		break;
	}
	
	if (m_nCurInput != EXIT_INPUT) {
		if (CInput::GetKeyPress(VK_A))
		{
			switch (m_nCurInput)
			{
			case MOUSESENSI_X_INPUT:
			{
				float fMouseSensi = CInput::GetMouseSensitivityX() - CInput::GetMouseSensiValRate();
				if (fMouseSensi < 0.0f) {
					fMouseSensi = 0.0f;
				}
				CInput::SetMouseSensitivityX(fMouseSensi);
			}	break;
			case MOUSESENSI_Y_INPUT:
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
			switch (m_nCurInput)
			{
			case MOUSESENSI_X_INPUT:
			{
				float fMouseSensi = CInput::GetMouseSensitivityX() + CInput::GetMouseSensiValRate();
				if (CInput::GetMouseSensitivityMax() < fMouseSensi) {
					fMouseSensi = CInput::GetMouseSensitivityX();
				}
				CInput::SetMouseSensitivityX(fMouseSensi);
			}	break;
			case MOUSESENSI_Y_INPUT:
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
	}
	else
	{

		if (CInput::GetKeyRelease(VK_SPACE)) {
			// ステート変更
			m_State = PauseState::Menu;
			// インプットテキスト削除
			DeleteInputText();
			// メニューテキスト表示
			OutputMenuText();
			// 決定音
			CSound::Play(SE_DECID);
		}
	}
}

void CPause::InputDraw(ID3D11DeviceContext* pDC)
{
	// プレート
	CPolygon::SetTexture(nullptr);
	CPolygon::SetPos(0.0f, -150.0f);
	CPolygon::SetSize(900.0f, 400.0f);
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
}


void CPause::SoundUpdate()
{
	if (CInput::GetKeyTrigger(VK_W))
	{

		// 色を元に戻す
		switch (m_nCurSound)
		{
		case MASTER_SOUND:
			CText::SetColor(m_TextMap[TEXT_MASTER_WORD], TEXT_MASTER_COLOR);
			break;
		case BGM_SOUND:
			CText::SetColor(m_TextMap[TEXT_BGM_WORD], TEXT_BGM_COLOR);
			break;
		case SE_SOUND:
			CText::SetColor(m_TextMap[TEXT_SE_WORD], TEXT_SE_COLOR);
			break;
		case EXIT_SOUND:
			CText::SetColor(m_TextMap[TEXT_EXIT_SOUND_WORD], TEXT_EXIT_SOUND_COLOR);
			break;
		default:
			break;
		}


		CSound::Play(SE_MENUE);
		m_nCurSound = (Sound_Menu)((m_nCurSound + MAX_SOUND - 1) % MAX_SOUND);

		//// 選択肢箇所に移動
		//MoveToTextPosEfk();
	}
	else if (CInput::GetKeyTrigger(VK_S))
	{
		// 色を元に戻す
		switch (m_nCurSound)
		{
		case MASTER_SOUND:
			CText::SetColor(m_TextMap[TEXT_MASTER_WORD], TEXT_MASTER_COLOR);
			break;
		case BGM_SOUND:
			CText::SetColor(m_TextMap[TEXT_BGM_WORD], TEXT_BGM_COLOR);
			break;
		case SE_SOUND:
			CText::SetColor(m_TextMap[TEXT_SE_WORD], TEXT_SE_COLOR);
			break;
		case EXIT_SOUND:
			CText::SetColor(m_TextMap[TEXT_EXIT_SOUND_WORD], TEXT_EXIT_SOUND_COLOR);
			break;
		default:
			break;
		}

		CSound::Play(SE_MENUE);
		m_nCurSound = (Sound_Menu)((m_nCurSound + 1) % MAX_SOUND);

		//// 選択肢箇所に移動
		//MoveToTextPosEfk();
	}

	// 色を変える
	switch (m_nCurSound)
	{
	case MASTER_SOUND:
		CText::SetColor(m_TextMap[TEXT_MASTER_WORD], XMFLOAT4(TEXT_MASTER_COLOR.x, TEXT_MASTER_COLOR.y, 1.0f, TEXT_MASTER_COLOR.w));
		break;
	case BGM_SOUND:
		CText::SetColor(m_TextMap[TEXT_BGM_WORD], XMFLOAT4(TEXT_BGM_COLOR.x, TEXT_BGM_COLOR.y, 1.0f, TEXT_BGM_COLOR.w));
		break;
	case SE_SOUND:
		CText::SetColor(m_TextMap[TEXT_SE_WORD], XMFLOAT4(TEXT_SE_COLOR.x, TEXT_SE_COLOR.y, 1.0f, TEXT_SE_COLOR.w));
		break;
	case EXIT_SOUND:
		CText::SetColor(m_TextMap[TEXT_EXIT_SOUND_WORD], XMFLOAT4(TEXT_EXIT_SOUND_COLOR.x, TEXT_EXIT_SOUND_COLOR.y, 1.0f, TEXT_EXIT_SOUND_COLOR.w));
		break;
	default:
		break;
	}


	if (m_nCurSound != EXIT_SOUND) {
		if (CInput::GetKeyPress(VK_A))
		{
			switch (m_nCurSound)
			{
			case MASTER_SOUND:
			{
				float fVolume = CSound::GetVolume() - CSound::GetVolumeValue();
				if (fVolume < 0.0f) {
					fVolume = 0.0f;
				}
				CSound::SetVolume(fVolume);
			}
			break;
			case BGM_SOUND:
			{
				float fVolume = CSound::GetVolume(BGM_TITLE) - CSound::GetVolumeValue();
				if (fVolume < 0.0f) {
					fVolume = 0.0f;
				}
			
				CSound::SetVolumeBGM(fVolume);
			}
			break;
			case SE_SOUND:
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
			switch (m_nCurSound)
			{
			case MASTER_SOUND:
			{
				float fVolume = CSound::GetVolume() + CSound::GetVolumeValue();
				if (CSound::GetMaxVolume() < fVolume) {
					fVolume = CSound::GetMaxVolume();
				}
				CSound::SetVolume(fVolume);
			}
			break;
			case BGM_SOUND:
			{
				float fVolume = CSound::GetVolume(BGM_TITLE) + CSound::GetVolumeValue();
				if (CSound::GetMaxVolume() < fVolume) {
					fVolume = CSound::GetMaxVolume();
				}
				CSound::SetVolumeBGM(fVolume);
			}
			break;
			case SE_SOUND:
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
	else
	{
		if (CInput::GetKeyRelease(VK_SPACE)) {
			// ステート変更
			m_State = PauseState::Menu;
			// サウンドテキスト削除
			DeleteSoundText();
			// メニューテキスト表示
			OutputMenuText();
			// 決定音
			CSound::Play(SE_DECID);
		}
	}
}

void CPause::SoundDraw(ID3D11DeviceContext* pDC)
{
	// プレート
	CPolygon::SetTexture(nullptr);
	CPolygon::SetPos(0.0f, -150.0f);
	CPolygon::SetSize(900.0f, 400.0f);
	CPolygon::SetColor(0.5f, 0.3f, 0.1f);
	CPolygon::SetAlpha(0.4f);
	CPolygon::Draw(pDC);


	// サウンドメニュー
	for (int i = 0; i < MAX_SOUND; ++i)
	{
		float fPosY = -10;
		fPosY -= i * SELECT_SIZE_Y;
		fPosY -= i * SELECT_INTERVAL;


		// バー
		switch (i)
		{
		case MASTER_SOUND:
		{
			float fSizeRateX = CSound::GetMaxBarSizeX() / CSound::GetMaxVolume();
			float fSizeX = CSound::GetVolume() * fSizeRateX;

			CPolygon::SetTexture(nullptr);
			CPolygon::SetPos(-150.0f + fSizeX / 2.0f, TEXT_MASTER_POS.y);
			CPolygon::SetSize(fSizeX, 50.0f);
			CPolygon::SetColor(0.0f, 1.0f, 1.0f);
			CPolygon::SetAlpha(1.0f);
			CPolygon::Draw(pDC);
		}
		break;
		case BGM_SOUND:
		{
			float fSizeRateX = CSound::GetMaxBarSizeX() / CSound::GetMaxVolume();
			float fSizeX = CSound::GetVolume(BGM_TITLE) * fSizeRateX;
			//                                 ↑取り出すボリュームはBGMならどれでもよい
			CPolygon::SetTexture(nullptr);
			CPolygon::SetPos(-150.0f + fSizeX / 2.0f, TEXT_BGM_POS.y);
			CPolygon::SetSize(fSizeX, 50.0f);
			CPolygon::SetColor(0.0f, 1.0f, 1.0f);
			CPolygon::SetAlpha(1.0f);
			CPolygon::Draw(pDC);
		}
		break;
		case SE_SOUND:
		{
			float fSizeRateX = CSound::GetMaxBarSizeX() / CSound::GetMaxVolume();
			float fSizeX = CSound::GetVolume(SE_MENUE) * fSizeRateX;
			//                                 ↑取り出すボリュームはSEならどれでもよい
			CPolygon::SetTexture(nullptr);
			CPolygon::SetPos(-150.0f + fSizeX / 2.0f, TEXT_SE_POS.y);
			CPolygon::SetSize(fSizeX, 50.0f);
			CPolygon::SetColor(0.0f, 1.0f, 1.0f);
			CPolygon::SetAlpha(1.0f);
			CPolygon::Draw(pDC);
		}
		break;
		}
	}
}

void CPause::OutputMenuText()
{
	// 文字セット  左パネル
	m_TextMap[TEXT_MOVE_WORD] = CText::OutPutText(TEXT_MOVE_WORD, TEXT_MOVE_POS, TEXT_MOVE_SIZE, TEXT_MOVE_COLOR);
	m_TextMap[TEXT_DASH_WORD] = CText::OutPutText(TEXT_DASH_WORD, TEXT_DASH_POS, TEXT_DASH_SIZE, TEXT_DASH_COLOR);
	m_TextMap[TEXT_JUMP_WORD] = CText::OutPutText(TEXT_JUMP_WORD, TEXT_JUMP_POS, TEXT_JUMP_SIZE, TEXT_JUMP_COLOR);
	m_TextMap[TEXT_SATK_WORD] = CText::OutPutText(TEXT_SATK_WORD, TEXT_SATK_POS, TEXT_SATK_SIZE, TEXT_SATK_COLOR);
	m_TextMap[TEXT_BATK1_WORD] = CText::OutPutText(TEXT_BATK1_WORD, TEXT_BATK1_POS, TEXT_BATK1_SIZE, TEXT_BATK1_COLOR);
	m_TextMap[TEXT_BATK2_WORD] = CText::OutPutText(TEXT_BATK2_WORD, TEXT_BATK2_POS, TEXT_BATK2_SIZE, TEXT_BATK2_COLOR);

	// 文字セット 右パネル
	m_TextMap[TEXT_CONTINUE_WORD] = CText::OutPutText(TEXT_CONTINUE_WORD, TEXT_CONTINUE_POS, TEXT_CONTINUE_SIZE, TEXT_CONTINUE_COLOR);
	m_TextMap[TEXT_RETRY_WORD] = CText::OutPutText(TEXT_RETRY_WORD, TEXT_RETRY_POS, TEXT_RETRY_SIZE, TEXT_RETRY_COLOR);
	m_TextMap[TEXT_INPUT_WORD] = CText::OutPutText(TEXT_INPUT_WORD, TEXT_INPUT_POS, TEXT_INPUT_SIZE, TEXT_INPUT_COLOR);
	m_TextMap[TEXT_SOUND_WORD] = CText::OutPutText(TEXT_SOUND_WORD, TEXT_SOUND_POS, TEXT_SOUND_SIZE, TEXT_SOUND_COLOR);
	m_TextMap[TEXT_TITLE_WORD] = CText::OutPutText(TEXT_TITLE_WORD, TEXT_TITLE_POS, TEXT_TITLE_SIZE, TEXT_TITLE_COLOR);
}

void CPause::DeleteMenuText()
{
	// 文字削除　左パネル
	CText::DeleteText(m_TextMap[TEXT_MOVE_WORD]);
	CText::DeleteText(m_TextMap[TEXT_DASH_WORD]);
	CText::DeleteText(m_TextMap[TEXT_JUMP_WORD]);
	CText::DeleteText(m_TextMap[TEXT_SATK_WORD]);
	CText::DeleteText(m_TextMap[TEXT_BATK1_WORD]);
	CText::DeleteText(m_TextMap[TEXT_BATK2_WORD]);

	// 文字削除　右パネル
	CText::DeleteText(m_TextMap[TEXT_CONTINUE_WORD]);
	CText::DeleteText(m_TextMap[TEXT_RETRY_WORD]);
	CText::DeleteText(m_TextMap[TEXT_INPUT_WORD]);
	CText::DeleteText(m_TextMap[TEXT_SOUND_WORD]);
	CText::DeleteText(m_TextMap[TEXT_TITLE_WORD]);

	m_TextMap.clear();
}

void CPause::OutputInputText()
{
	m_TextMap[TEXT_SENSI_X_WORD] = (CText::OutPutText(TEXT_SENSI_X_WORD, TEXT_SENSI_X_POS, TEXT_SENSI_X_SIZE, TEXT_SENSI_X_COLOR));
	m_TextMap[TEXT_SENSI_Y_WORD] = (CText::OutPutText(TEXT_SENSI_Y_WORD, TEXT_SENSI_Y_POS, TEXT_SENSI_Y_SIZE, TEXT_SENSI_Y_COLOR));
	m_TextMap[TEXT_EXIT_INPUT_WORD] = (CText::OutPutText(TEXT_EXIT_INPUT_WORD, TEXT_EXIT_INPUT_POS, TEXT_EXIT_INPUT_SIZE, TEXT_EXIT_INPUT_COLOR));

}

void CPause::DeleteInputText()
{
	CText::DeleteText(m_TextMap[TEXT_SENSI_X_WORD]);
	CText::DeleteText(m_TextMap[TEXT_SENSI_Y_WORD]);
	CText::DeleteText(m_TextMap[TEXT_EXIT_INPUT_WORD]);

}

void CPause::OutputSoundText()
{
	m_TextMap[TEXT_MASTER_WORD] = (CText::OutPutText(TEXT_MASTER_WORD, TEXT_MASTER_POS, TEXT_MASTER_SIZE, TEXT_MASTER_COLOR));
	m_TextMap[TEXT_BGM_WORD] = (CText::OutPutText(TEXT_BGM_WORD, TEXT_BGM_POS, TEXT_BGM_SIZE, TEXT_BGM_COLOR));
	m_TextMap[TEXT_SE_WORD] = (CText::OutPutText(TEXT_SE_WORD, TEXT_SE_POS, TEXT_SE_SIZE, TEXT_SE_COLOR));
	m_TextMap[TEXT_EXIT_SOUND_WORD] = (CText::OutPutText(TEXT_EXIT_SOUND_WORD, TEXT_EXIT_SOUND_POS, TEXT_EXIT_SOUND_SIZE, TEXT_EXIT_SOUND_COLOR));
}

void CPause::DeleteSoundText()
{
	CText::DeleteText(m_TextMap[TEXT_MASTER_WORD]);
	CText::DeleteText(m_TextMap[TEXT_BGM_WORD]);
	CText::DeleteText(m_TextMap[TEXT_SE_WORD]);
	CText::DeleteText(m_TextMap[TEXT_EXIT_SOUND_WORD]);
}

void CPause::DeleteTextAll()
{
	for (auto itr = m_TextMap.begin(); itr != m_TextMap.end();)
	{
		CText::DeleteText(itr->second);
		++itr;
	}
	m_TextMap.clear();
}


// スクリーン座標をワールド座標に変換
XMFLOAT3* CPause::CalcScreenToWorld(
	XMFLOAT3* pout,
	float Sx,  // スクリーンX座標
	float Sy,  // スクリーンY座標
	float fZ,  // 射影空間でのZ値（0～1）
	float Screen_w,
	float Screen_h,
	XMFLOAT4X4* View,
	XMFLOAT4X4* Prj
) {
	// 各行列の逆行列を算出
	XMMATRIX InvView, InvPrj, InvViewport;
	InvView = XMMatrixInverse(nullptr, XMLoadFloat4x4(View));
	InvPrj = XMMatrixInverse(nullptr, XMLoadFloat4x4(Prj));
	XMFLOAT4X4 VP;
	XMStoreFloat4x4(&VP, XMMatrixIdentity());
	VP._11 = Screen_w / 2.0f; VP._22 = -Screen_h / 2.0f;
	VP._41 = Screen_w / 2.0f; VP._42 = Screen_h / 2.0f;
	InvViewport = XMMatrixInverse(nullptr, XMLoadFloat4x4(&VP));

	// 逆変換
	XMMATRIX tmp = InvViewport * InvPrj * InvView;
	XMStoreFloat3(pout, XMVector3TransformCoord(XMVectorSet(Sx, Sy, fZ, 1.0f), tmp));

	return pout;
}


void CPause::GenerateEfk()
{
	// 選択中光球生成
	m_nEfkHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::MACHINGUN_GENERATER_PLAYER_EFK), true, TIMER_MACHINGUN_GENERATER_PLAYER_EFK);
}

void CPause::SetEfkPos(XMFLOAT2 vPos)
{
	// 光源位置
	XMFLOAT2 EfkScreenPos(vPos.x, vPos.y);
	XMFLOAT3 EfkPos;
	CalcScreenToWorld(
		&EfkPos,
		EfkScreenPos.x, EfkScreenPos.y, 0.997f,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		&m_pCamera->GetViewMatrix(),
		&m_pCamera->GetProjMatrix());
	XMFLOAT4X4 EfkMat;
	XMStoreFloat4x4(&EfkMat, XMMatrixIdentity());

	XMMATRIX mtxScl;
	mtxScl = XMMatrixScaling(0.25f, 0.25f, 0.25f);
	XMStoreFloat4x4(&EfkMat, XMMatrixMultiply(XMLoadFloat4x4(&EfkMat), mtxScl));

	EfkMat._41 = EfkPos.x;
	EfkMat._42 = EfkPos.y;
	EfkMat._43 = EfkPos.z;

	CEffekseerManager::SetMatrix(m_nEfkHandle, true, EfkMat);
}

void CPause::DeleteEfk()
{
	CEffekseerManager::Stop(m_nEfkHandle, true);
}

void CPause::MoveToTextPosEfk()
{
	switch (m_State)
	{
	case PauseState::None:

		break;
	case PauseState::Menu:
		if (Pause_Menu::PAUSE_MENU_CONTINUE == m_nSelectMenu)
		{
			// Efk位置セット
			SetEfkPos(XMFLOAT2(850.0f, 300.0f));
		}
		else if (Pause_Menu::PAUSE_MENU_RETRY == m_nSelectMenu)
		{
			// Efk位置セット
			SetEfkPos(XMFLOAT2(850.0f, 400.0f));
		}
		else if (Pause_Menu::PAUSE_MENU_SOUND == m_nSelectMenu)
		{
			// Efk位置セット
			SetEfkPos(XMFLOAT2(850.0f, 500.0f));
		}
		else if (Pause_Menu::PAUSE_MENU_QUIT == m_nSelectMenu)
		{
			// Efk位置セット
			SetEfkPos(XMFLOAT2(850.0f, 600.0f));
		}
		break;
	case PauseState::Sound:
		if (Sound_Menu::MASTER_SOUND == m_nCurSound)
		{
			// Efk位置セット
			SetEfkPos(XMFLOAT2(0.0f, 500.0f));
		}
		else if (Sound_Menu::BGM_SOUND == m_nCurSound)
		{
			// Efk位置セット
			SetEfkPos(XMFLOAT2(0.0f, 550.0f));
		}
		else if (Sound_Menu::SE_SOUND == m_nCurSound)
		{
			// Efk位置セット
			SetEfkPos(XMFLOAT2(0.0f, 600.0f));
		}
		else if (Sound_Menu::EXIT_SOUND == m_nCurSound)
		{
			// Efk位置セット
			SetEfkPos(XMFLOAT2(0.0f, 650.0f));
		}
		break;
	default:
		break;
	}

}