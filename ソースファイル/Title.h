//=============================================================================
//
// タイトル画面クラス定義 [Title.h]
// Author : KAZUKI KURA
//
//=============================================================================
#pragma once
#include "Scene.h"
#include "Input.h"
#include "Sound.h"
#include "Player.h"
#include "Wave.h"
#include <vector>
#include "TitleUI_Manager.h"

typedef enum {
	STAGESELECT_MENUE = 0,
	OPTION_MENUE,
	
	MAX_MENUE
}Title_Menue;

typedef enum {
	INPUT_OPTION = 0,
	SOUND_OPTION,
	EXIT_OPTION,
	
	MAX_OPTION
}Option_Menue;

// クラス定義
class CTitle : public CScene
{
private:
	// 画面表示ステート列挙
	enum TitleState {
		Start = 0,
		Menue,
		Option,
		Input,
		Sound,
	};

	CTitleUI_Manager m_UIManager;

	// 画面表示ステート
	TitleState m_State;
	// タイトル名画像
	//ID3D11ShaderResourceView* m_pTexTitle;
	// スタートメニュー画面ステート
	Title_Menue m_nCurMenue;
	// オプションメニューステート
	Option_Menue m_nCurOption;
	// 感度調整メニューステート
	Input_Menu m_nCurInput;
	// サウンドメニューステート
	Sound_Menu m_nCurSound;
	// 波処理クラス
	CWave* m_pWave;

	std::vector<int> m_DrawTextIndex;
	
	XMFLOAT2 EfkScreenPos;
public:
	CTitle();
	virtual ~CTitle();

	virtual bool Init();
	virtual void Fin();
	virtual void Update();
	virtual void Draw();

private :
	void StartUpdate();
	void StartDraw(ID3D11DeviceContext* pDC);
	void MenueUpdate();
	void MenueDraw(ID3D11DeviceContext* pDC);
	void InputUpdate();
	void InputDraw(ID3D11DeviceContext* pDC);
	void OptionUpdate();
	void OptionDraw(ID3D11DeviceContext* pDC);
	void SoundUpdate();
	void SoundDraw(ID3D11DeviceContext* pDC);
	
	void DeleteText();

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

	//void GenerateEfk();
	void SetEfkPos(XMFLOAT2 vPos);
	void DeleteEfk();
	void MoveToTextPosEfk();
};
