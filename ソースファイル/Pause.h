//=============================================================================
//
// 一時停止処理 [pause.h]
// Author : RYUSUKE	YAMADA
//
//=============================================================================
#pragma once
//#include "Scene.h"

#include "main.h"
#include "Sound.h"
#include "Input.h"
#include <map>
#include "Camera.h"

// ポーズメニュー
typedef enum 
{
	PAUSE_MENU_CONTINUE = 0,		    // コンティニュー
	PAUSE_MENU_RETRY,					// リトライ
	PAUSE_MENU_INPUT,                   // マウス感度
	PAUSE_MENU_SOUND,                   // サウンド
	PAUSE_MENU_QUIT,				    // クイット

	PAUSE_MENU_MAX
} Pause_Menu;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
//enum Sound_Menue;
class CPause
{
private:
	enum PauseState {
		None = 0,
		Menu,
		Input,
		Sound,
	};



	
	//ID3D11ShaderResourceView*	m_pTextureControl;	// コントロールキーテクスチャ
	//ID3D11ShaderResourceView*	m_pTextureMenue[PAUSE_MENU_MAX];	// ポーズメニューテクスチャ

	bool m_bPause;              // ポーズフラグ
	PauseState m_State;
	Pause_Menu m_nSelectMenu;	// 選択メニュー
	Input_Menu m_nCurInput;
	Sound_Menu m_nCurSound;
	ID3D11ShaderResourceView* m_pTexSound[MAX_SOUND]; // ステージ番号文字
	typedef std::map< std::string, int > TextMap;
	TextMap m_TextMap;
	int m_nEfkHandle;
	XMFLOAT2 EfkScreenPos;
	CCamera* m_pCamera;
public:
	CPause();
	~CPause();

	HRESULT Init();
	void Fin();
	void Update();
	void Draw();

	//void SetPauseMenu(void);
	Pause_Menu GetPauseMenu(void);
	void ResetPauseMenu(void);

	bool GetPauseFlg() { return m_bPause; }
	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
private:
	void NoneUpdate();
	void NoneDraw(ID3D11DeviceContext* pDC);
	void MenuUpdate();
	void MenuDraw(ID3D11DeviceContext* pDC);
	void InputUpdate();
	void InputDraw(ID3D11DeviceContext* pDC);
	void SoundUpdate();
	void SoundDraw(ID3D11DeviceContext* pDC);

	void OutputMenuText();
	void DeleteMenuText();
	void OutputInputText();
	void DeleteInputText();
	void OutputSoundText();
	void DeleteSoundText();
	void DeleteTextAll();

	XMFLOAT3* CalcScreenToWorld(
		XMFLOAT3* pout,
		float Sx,  // スクリーンX座標
		float Sy,  // スクリーンY座標
		float fZ,  // 射影空間でのZ値（0～1）
		float Screen_w,
		float Screen_h,
		XMFLOAT4X4* View,
		XMFLOAT4X4* Prj
	);
	void GenerateEfk();
	void SetEfkPos(XMFLOAT2 vPos);
	void DeleteEfk();
	void MoveToTextPosEfk();
	
};