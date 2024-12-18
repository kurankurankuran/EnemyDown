#include "TitleUI_Manager.h"
#include "TitleUI_Start.h"
#include "TitleUI_Menu.h"
#include "TitleUI_Option.h"
#include "TitleUI_Input.h"
#include "TitleUI_Sound.h"
#include "TextureData.h"
#include "Polygon.h"
#include "Scene.h"

// 静的メンバ
CTitleUI_Manager::TitleUIState                                      CTitleUI_Manager::m_CurTitleState;
std::shared_ptr<CTitleUI>                                           CTitleUI_Manager::m_pCurTitleUI;
std::map<CTitleUI_Manager::TitleUIState, std::shared_ptr<CTitleUI>> CTitleUI_Manager::m_empTitleStateToUI;


CTitleUI_Manager::CTitleUI_Manager()
{
	// ステートごとの画面インスタンス生成
	m_empTitleStateToUI[TitleUIState::Start] = std::make_shared<CTitleUI_Start>();
	m_empTitleStateToUI[TitleUIState::Menu] = std::make_shared <CTitleUI_Menu>();
	m_empTitleStateToUI[TitleUIState::Option] = std::make_shared <CTitleUI_Option>();
	m_empTitleStateToUI[TitleUIState::Input] = std::make_shared <CTitleUI_Input>();
	m_empTitleStateToUI[TitleUIState::Sound] = std::make_shared <CTitleUI_Sound>();
}

CTitleUI_Manager::~CTitleUI_Manager()
{
	// 画面インスタンス消去
	m_empTitleStateToUI.clear();
}

void CTitleUI_Manager::Init()
{
	// ステート初期化
	m_CurTitleState = TitleUIState::Start;
	// 機能させる画面インスタンス設定・初期化
	m_pCurTitleUI = m_empTitleStateToUI[m_CurTitleState];
	m_pCurTitleUI->Init();
}



void CTitleUI_Manager::Fin()
{
	// 画面インスタンス終了・消去
	m_pCurTitleUI->Fin();
}

void CTitleUI_Manager::Update()
{
	// 画面インスタンス更新
	m_pCurTitleUI->Update();
}

void CTitleUI_Manager::Draw()
{
	// 画面インスタンス描画
	m_pCurTitleUI->Draw();
}

void CTitleUI_Manager::ChangeState(TitleUIState State)
{
	// 現在の画面インスタンス終了処理
	m_pCurTitleUI->Fin();
	// ステート変更
	m_CurTitleState = State;
	// 画面インスタンス変更・初期化
	m_pCurTitleUI = m_empTitleStateToUI[m_CurTitleState];
	m_pCurTitleUI->Init();
}