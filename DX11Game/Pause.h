//=============================================================================
//
// �ꎞ��~���� [pause.h]
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

// �|�[�Y���j���[
typedef enum 
{
	PAUSE_MENU_CONTINUE = 0,		    // �R���e�B�j���[
	PAUSE_MENU_RETRY,					// ���g���C
	PAUSE_MENU_INPUT,                   // �}�E�X���x
	PAUSE_MENU_SOUND,                   // �T�E���h
	PAUSE_MENU_QUIT,				    // �N�C�b�g

	PAUSE_MENU_MAX
} Pause_Menu;


//*****************************************************************************
// �v���g�^�C�v�錾
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



	
	//ID3D11ShaderResourceView*	m_pTextureControl;	// �R���g���[���L�[�e�N�X�`��
	//ID3D11ShaderResourceView*	m_pTextureMenue[PAUSE_MENU_MAX];	// �|�[�Y���j���[�e�N�X�`��

	bool m_bPause;              // �|�[�Y�t���O
	PauseState m_State;
	Pause_Menu m_nSelectMenu;	// �I�����j���[
	Input_Menu m_nCurInput;
	Sound_Menu m_nCurSound;
	ID3D11ShaderResourceView* m_pTexSound[MAX_SOUND]; // �X�e�[�W�ԍ�����
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
		float Sx,  // �X�N���[��X���W
		float Sy,  // �X�N���[��Y���W
		float fZ,  // �ˉe��Ԃł�Z�l�i0�`1�j
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