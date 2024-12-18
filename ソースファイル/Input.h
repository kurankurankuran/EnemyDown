//=======================================================================================
//
//	  CInput クラス定義 [Input.h]
//
//=======================================================================================
#pragma once
#include <windows.h>

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	NUM_KEY_MAX		(256)				// キー最大数
#define NUM_JOY_MAX		(32)				// ゲームパッド最大数

// 未定義部分を定義
#define VK_0			'0'
#define VK_1			'1'
#define VK_2			'2'
#define VK_3			'3'
#define VK_4			'4'
#define VK_5			'5'
#define VK_6			'6'
#define VK_7			'7'
#define VK_8			'8'
#define VK_9			'9'
#define VK_A			'A'
#define VK_B			'B'
#define VK_C			'C'
#define VK_D			'D'
#define VK_E			'E'
#define VK_F			'F'
#define VK_G			'G'
#define VK_H			'H'
#define VK_I			'I'
#define VK_J			'J'
#define VK_K			'K'
#define VK_L			'L'
#define VK_M			'M'
#define VK_N			'N'
#define VK_O			'O'
#define VK_P			'P'
#define VK_Q			'Q'
#define VK_R			'R'
#define VK_S			'S'
#define VK_T			'T'
#define VK_U			'U'
#define VK_V			'V'
#define VK_W			'W'
#define VK_X			'X'
#define VK_Y			'Y'
#define VK_Z			'Z'

#define JOYSTICKID3		2
#define JOYSTICKID4		3
#define JOYSTICKID5		4
#define JOYSTICKID6		5
#define JOYSTICKID7		6
#define JOYSTICKID8		7
#define JOYSTICKID9		8
#define JOYSTICKID10	9
#define JOYSTICKID11	10
#define JOYSTICKID12	11
#define JOYSTICKID13	12
#define JOYSTICKID14	13
#define JOYSTICKID15	14
#define JOYSTICKID16	15

#define MOUSEBUTTON_L	0
#define MOUSEBUTTON_R	1
#define MOUSEBUTTON_M	2
#define MOUSEBUTTON_X1	3
#define MOUSEBUTTON_X2	4

enum Input_Menu {
	MOUSESENSI_X_INPUT = 0,
	MOUSESENSI_Y_INPUT,
	EXIT_INPUT,
	
	MAX_INPUT
};

//*****************************************************************************
// クラス定義
//*****************************************************************************
class CInput
{
private:
	static WORD			m_wKeyState[NUM_KEY_MAX];			// キーボードの押下状態を保持するワーク
	static WORD			m_wKeyStateTrigger[NUM_KEY_MAX];	// キーボードのトリガ状態を保持するワーク
	static WORD			m_wKeyStateRelease[NUM_KEY_MAX];	// キーボードのリリース状態を保持するワーク
	static WORD			m_wKeyStateRepeat[NUM_KEY_MAX];		// キーボードのリピート状態を保持するワーク
	static int			m_nKeyStateRepeatCnt[NUM_KEY_MAX];	// キーボードのリピートカウンタ

	static DWORD		m_dwJoyCount;
	static JOYINFOEX	m_joyState[NUM_JOY_MAX];
	static DWORD		m_dwJoyButtonTrigger[NUM_JOY_MAX];
	static DWORD		m_dwJoyButtonRelease[NUM_JOY_MAX];

	static POINT		m_ptMouse;                           // マウスのクライアント座標
	

	static float        m_ptMouseSensitivityX;               // マウス感度(横方向)
	static float        m_ptMouseSensitivityY;               // マウス感度(縦方向)
	static float        m_ptMouseSensitivityMax;             // 最大マウス感度
	static float        m_ptMouseSensiValRate;               // マウス感度変化量
	static float        m_fMouseBarLenMax;                   // バーの最大の長さ

	static bool         m_bRockCursorFlag;                   // カーソル画面中央で固定フラグ           
public:
	static HRESULT Init();
	static void Fin();
	static void Update();

	static bool GetKeyPress(int nKey);
	static bool GetKeyTrigger(int nKey);
	static bool GetKeyRelease(int nKey);
	static bool GetKeyRepeat(int nKey);

	static DWORD GetJoyCount();
	static JOYINFOEX *GetJoyState(DWORD dwJoy);
	static LONG GetJoyX(DWORD dwJoy);
	static LONG GetJoyY(DWORD dwJoy);
	static LONG GetJoyZ(DWORD dwJoy);
	static bool GetJoyButton(DWORD dwJoy, DWORD dwBtn);
	static bool GetJoyTrigger(DWORD dwJoy, DWORD dwBtn);
	static bool GetJoyRelease(DWORD dwJoy, DWORD dwBtn);

	static POINT *GetMousePosition();
	static long GetMouseMoveX();
	static long GetMouseMoveY();
	static float GetMouseSensitivityX();
	static float GetMouseSensitivityY();
	static float GetMouseSensitivityMax() { return m_ptMouseSensitivityMax; }
	static float GetMouseSensiValRate() { return m_ptMouseSensiValRate; }
	static void SetMouseSensitivityX(float fSensitiv) { m_ptMouseSensitivityX = fSensitiv; }
	static void SetMouseSensitivityY(float fSensitiv) { m_ptMouseSensitivityY = fSensitiv; }
	static float GetMaxBarSize() { return m_fMouseBarLenMax; }
	
	static bool GetMouseButton(DWORD dwBtn);
	static bool GetMouseTrigger(DWORD dwBtn);
	static bool GetMouseRelease(DWORD dwBtn);

	static bool GetRockCursorFlag() { return m_bRockCursorFlag; }
	static void SetRockCursorFlag(bool bFlag) { m_bRockCursorFlag = bFlag; }
private:
	static HRESULT InitKeyboard();
	static void UninitKeyboard();
	static HRESULT UpdateKeyboard();

	static HRESULT InitJoystick();
	static void UninitJoystick();
	static HRESULT UpdateJoystick();

	static HRESULT InitMouse();
	static void UninitMouse();
	static HRESULT UpdateMouse();
};

//=======================================================================================
//	End of File
//=======================================================================================