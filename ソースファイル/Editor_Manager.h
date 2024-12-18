//=============================================================================
//
// �G�f�B�^�[����Ǘ� �N���X��` [Editor_Manager.h]
// Author : KAZUKI KURA
//
//=============================================================================
#pragma once
#include<memory>
#include<map>
#include"ES_Base.h"
class CEditor_Manager
{
public:
	enum class EEditorState {// �G�f�B�^�[�̑����Ԃ̃X�e�[�g�i����X�e�[�g�j
		PutObj = 0,
		WaveSetting,
		MaxState
	};

public:
	CEditor_Manager();
	~CEditor_Manager();

	void Init();
	void Fin();
	void Update();
	void Draw();

	static bool GetStateSelectFlag() { return m_bStateSelect; }                           // �X�e�[�g�I����ԃt���O
	static void SetSelectFlag(bool bFlag) { m_bStateSelect = bFlag; }
	static void ChangeState(EEditorState State);                                          // �X�e�[�g�ύX
	static EEditorState GetState() { return m_eCurEditorState; }
private:
	static bool m_bStateSelect;                                                           // �X�e�[�g�I����ԃt���O
	static int m_nTextIndex;                                                              // �e�L�X�g�C���f�b�N�X   
	static std::map<EEditorState, std::string> m_empEditorStateToString;                  // �񋓂�string�ɕϊ�
	static EEditorState m_eCurEditorState;                                                // ���݂̑���X�e�[�g
	static std::shared_ptr<CES_Base> m_spCurES;                                           // ����X�e�[�g�C���X�^���X
	static std::map<EEditorState, std::shared_ptr<CES_Base>> m_empEditorStateToESClass;   // �񋓂��瑀��X�e�[�g�����o��
};
