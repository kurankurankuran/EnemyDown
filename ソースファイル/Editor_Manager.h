//=============================================================================
//
// エディター操作管理 クラス定義 [Editor_Manager.h]
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
	enum class EEditorState {// エディターの操作状態のステート（操作ステート）
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

	static bool GetStateSelectFlag() { return m_bStateSelect; }                           // ステート選択状態フラグ
	static void SetSelectFlag(bool bFlag) { m_bStateSelect = bFlag; }
	static void ChangeState(EEditorState State);                                          // ステート変更
	static EEditorState GetState() { return m_eCurEditorState; }
private:
	static bool m_bStateSelect;                                                           // ステート選択状態フラグ
	static int m_nTextIndex;                                                              // テキストインデックス   
	static std::map<EEditorState, std::string> m_empEditorStateToString;                  // 列挙をstringに変換
	static EEditorState m_eCurEditorState;                                                // 現在の操作ステート
	static std::shared_ptr<CES_Base> m_spCurES;                                           // 操作ステートインスタンス
	static std::map<EEditorState, std::shared_ptr<CES_Base>> m_empEditorStateToESClass;   // 列挙から操作ステートを取り出す
};
