//=============================================================================
//
// �^�C�g����ʃN���X��` [Title.h]
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

// �N���X��`
class CTitle : public CScene
{
private:
	// ��ʕ\���X�e�[�g��
	enum TitleState {
		Start = 0,
		Menue,
		Option,
		Input,
		Sound,
	};

	CTitleUI_Manager m_UIManager;

	// ��ʕ\���X�e�[�g
	TitleState m_State;
	// �^�C�g�����摜
	//ID3D11ShaderResourceView* m_pTexTitle;
	// �X�^�[�g���j���[��ʃX�e�[�g
	Title_Menue m_nCurMenue;
	// �I�v�V�������j���[�X�e�[�g
	Option_Menue m_nCurOption;
	// ���x�������j���[�X�e�[�g
	Input_Menu m_nCurInput;
	// �T�E���h���j���[�X�e�[�g
	Sound_Menu m_nCurSound;
	// �g�����N���X
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
		float Sx,  // �X�N���[��X���W
		float Sy,  // �X�N���[��Y���W
		float fZ,  // �ˉe��Ԃł�Z�l�i0�`1�j
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
