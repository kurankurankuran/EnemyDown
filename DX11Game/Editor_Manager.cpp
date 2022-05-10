#include"Editor_Manager.h"
#include"ES_EWSetting.h"
#include"ES_PutObj.h"
#include"Input.h"
#include"main.h"
#include"Text.h"
// 静的変数
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
	// ステートごとのインスタンス生成
	m_empEditorStateToESClass[EEditorState::PutObj] = std::make_shared<CES_PutObj>();
	m_empEditorStateToESClass[EEditorState::WaveSetting] = std::make_shared <CES_EWSetting>();
}

CEditor_Manager::~CEditor_Manager()
{
	// 画面インスタンス消去
	m_empEditorStateToESClass.clear();
}

void CEditor_Manager::Init()
{
	// ステート初期化
	m_eCurEditorState = EEditorState::PutObj;
	// 機能させるインスタンス設定・初期化
	m_spCurES = m_empEditorStateToESClass[m_eCurEditorState];
	m_spCurES->Init();
}



void CEditor_Manager::Fin()
{
	// インスタンス終了・消去
	m_spCurES->Fin();
}

void CEditor_Manager::Update()
{
	

	if (!m_bStateSelect) {
		// インスタンス更新
		m_spCurES->Update();

		if (CInput::GetKeyTrigger(VK_Q))
		{
			// フラグ切替
			m_bStateSelect = !m_bStateSelect;
			// 現在ステート名表示
			m_nTextIndex = CText::OutPutText(TEXT_CURSTATE_WORD + m_empEditorStateToString[m_eCurEditorState], TEXT_CURSTATE_POS, TEXT_SCALE);
		}
	}
	else
	{
		// 操作ステート切替
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
			// フラグ切替
			m_bStateSelect = !m_bStateSelect;
			// ステート名表示消去
			CText::DeleteText(m_nTextIndex);
		}
	}
}

void CEditor_Manager::Draw()
{
	// インスタンス描画
	m_spCurES->Draw();
}



void CEditor_Manager::ChangeState(EEditorState State)
{
	// 現在のインスタンス終了処理
	m_spCurES->Fin();
	// ステート変更
	m_eCurEditorState = State;
	// 表示テキスト削除
	CText::DeleteText(m_nTextIndex);
	m_nTextIndex = CText::OutPutText(TEXT_CURSTATE_WORD + m_empEditorStateToString[m_eCurEditorState], TEXT_CURSTATE_POS, TEXT_SCALE);
	// インスタンス変更・初期化
	m_spCurES = m_empEditorStateToESClass[m_eCurEditorState];
	m_spCurES->Init();
}