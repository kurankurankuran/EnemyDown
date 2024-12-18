#include "TitleUI_Manager.h"
#include "TitleUI_Start.h"
#include "TitleUI_Menu.h"
#include "TitleUI_Option.h"
#include "TitleUI_Input.h"
#include "TitleUI_Sound.h"
#include "TextureData.h"
#include "Polygon.h"
#include "Scene.h"

// �ÓI�����o
CTitleUI_Manager::TitleUIState                                      CTitleUI_Manager::m_CurTitleState;
std::shared_ptr<CTitleUI>                                           CTitleUI_Manager::m_pCurTitleUI;
std::map<CTitleUI_Manager::TitleUIState, std::shared_ptr<CTitleUI>> CTitleUI_Manager::m_empTitleStateToUI;


CTitleUI_Manager::CTitleUI_Manager()
{
	// �X�e�[�g���Ƃ̉�ʃC���X�^���X����
	m_empTitleStateToUI[TitleUIState::Start] = std::make_shared<CTitleUI_Start>();
	m_empTitleStateToUI[TitleUIState::Menu] = std::make_shared <CTitleUI_Menu>();
	m_empTitleStateToUI[TitleUIState::Option] = std::make_shared <CTitleUI_Option>();
	m_empTitleStateToUI[TitleUIState::Input] = std::make_shared <CTitleUI_Input>();
	m_empTitleStateToUI[TitleUIState::Sound] = std::make_shared <CTitleUI_Sound>();
}

CTitleUI_Manager::~CTitleUI_Manager()
{
	// ��ʃC���X�^���X����
	m_empTitleStateToUI.clear();
}

void CTitleUI_Manager::Init()
{
	// �X�e�[�g������
	m_CurTitleState = TitleUIState::Start;
	// �@�\�������ʃC���X�^���X�ݒ�E������
	m_pCurTitleUI = m_empTitleStateToUI[m_CurTitleState];
	m_pCurTitleUI->Init();
}



void CTitleUI_Manager::Fin()
{
	// ��ʃC���X�^���X�I���E����
	m_pCurTitleUI->Fin();
}

void CTitleUI_Manager::Update()
{
	// ��ʃC���X�^���X�X�V
	m_pCurTitleUI->Update();
}

void CTitleUI_Manager::Draw()
{
	// ��ʃC���X�^���X�`��
	m_pCurTitleUI->Draw();
}

void CTitleUI_Manager::ChangeState(TitleUIState State)
{
	// ���݂̉�ʃC���X�^���X�I������
	m_pCurTitleUI->Fin();
	// �X�e�[�g�ύX
	m_CurTitleState = State;
	// ��ʃC���X�^���X�ύX�E������
	m_pCurTitleUI = m_empTitleStateToUI[m_CurTitleState];
	m_pCurTitleUI->Init();
}