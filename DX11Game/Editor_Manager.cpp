#include"Editor_Manager.h"
#include"ES_EWSetting.h"
#include"ES_PutObj.h"
#include"Input.h"
#include"main.h"
#include"Text.h"
// �ÓI�ϐ�
bool CEditor_Manager::m_bStateSelect = false;
int CEditor_Manager::m_nTextIndex;
std::map<CEditor_Manager::EEditorState, std::string> CEditor_Manager::m_empEditorStateToString;
CEditor_Manager::EEditorState CEditor_Manager::m_eCurEditorState;
std::shared_ptr<CES_Base> CEditor_Manager::m_spCurES;
std::map<CEditor_Manager::EEditorState, std::shared_ptr<CES_Base>> CEditor_Manager::m_empEditorStateToESClass;

namespace {
	const std::string TEXT_CURSTATE_WORD = std::string("CurState ");
	const XMFLOAT2 TEXT_SCALE = XMFLOAT2(10, 20);
	const XMFLOAT2 TEXT_CURSTATE_POS = XMFLOAT2(-SCREEN_CENTER_X + TEXT_SCALE.x, SCREEN_CENTER_Y - TEXT_SCALE.y);
}

CEditor_Manager::CEditor_Manager()
{
	m_empEditorStateToString[EEditorState::PutObj] = std::string("PutObj");
	m_empEditorStateToString[EEditorState::WaveSetting] = std::string("WaveSetting");
	m_bStateSelect = false;
	// �X�e�[�g���Ƃ̃C���X�^���X����
	m_empEditorStateToESClass[EEditorState::PutObj] = std::make_shared<CES_PutObj>();
	m_empEditorStateToESClass[EEditorState::WaveSetting] = std::make_shared <CES_EWSetting>();
}

CEditor_Manager::~CEditor_Manager()
{
	// ��ʃC���X�^���X����
	m_empEditorStateToESClass.clear();
}

void CEditor_Manager::Init()
{
	// �X�e�[�g������
	m_eCurEditorState = EEditorState::PutObj;
	// �@�\������C���X�^���X�ݒ�E������
	m_spCurES = m_empEditorStateToESClass[m_eCurEditorState];
	m_spCurES->Init();
}



void CEditor_Manager::Fin()
{
	// �C���X�^���X�I���E����
	m_spCurES->Fin();
}

void CEditor_Manager::Update()
{
	

	if (!m_bStateSelect) {
		// �C���X�^���X�X�V
		m_spCurES->Update();

		if (CInput::GetKeyTrigger(VK_Q))
		{
			// �t���O�ؑ�
			m_bStateSelect = !m_bStateSelect;
			// ���݃X�e�[�g���\��
			m_nTextIndex = CText::OutPutText(TEXT_CURSTATE_WORD + m_empEditorStateToString[m_eCurEditorState], TEXT_CURSTATE_POS, TEXT_SCALE);
		}
	}
	else
	{
		// ����X�e�[�g�ؑ�
		if (CInput::GetKeyTrigger(VK_W))
		{
			int nNextState = (int)m_eCurEditorState + 1;
			if ((int)EEditorState::MaxState <= nNextState) {
				nNextState = 0;
			}
			ChangeState((EEditorState)(nNextState));
		}
		else if (CInput::GetKeyTrigger(VK_S)) {
			int nNextState = (int)m_eCurEditorState - 1;
			if (nNextState < 0) {
				nNextState = (int)EEditorState::MaxState - 1;
			}
			ChangeState((EEditorState)(nNextState));
		}
		if (CInput::GetKeyTrigger(VK_Q))
		{
			// �t���O�ؑ�
			m_bStateSelect = !m_bStateSelect;
			// �X�e�[�g���\������
			CText::DeleteText(m_nTextIndex);
		}
	}
}

void CEditor_Manager::Draw()
{
	// �C���X�^���X�`��
	m_spCurES->Draw();
}



void CEditor_Manager::ChangeState(EEditorState State)
{
	// ���݂̃C���X�^���X�I������
	m_spCurES->Fin();
	// �X�e�[�g�ύX
	m_eCurEditorState = State;
	// �\���e�L�X�g�폜
	CText::DeleteText(m_nTextIndex);
	m_nTextIndex = CText::OutPutText(TEXT_CURSTATE_WORD + m_empEditorStateToString[m_eCurEditorState], TEXT_CURSTATE_POS, TEXT_SCALE);
	// �C���X�^���X�ύX�E������
	m_spCurES = m_empEditorStateToESClass[m_eCurEditorState];
	m_spCurES->Init();
}