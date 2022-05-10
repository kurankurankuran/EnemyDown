// イベントフラグ管理クラス(抽象クラス) EventManager.h
#pragma once
#include "main.h"
//#include "Scene.h"
#include <vector>

// イベントID
enum Event_ID{
	TUTORIAL_EVENT_ID = 0,
	STARTEND_EVENT_ID,
	WAVESTART_EVENT_ID,

	MAX_EVENT_ID
};

// テクスチャフェード状態
enum TEX_FADE {
	FADE_TEXNONE = 0,
	FADE_TEXIN,
	FADE_TEXKEEP,
	FADE_TEXOUT,

	MAX_TEXFADE
};

class CScene;
class CEventManager
{
private:
	Event_ID m_Event_ID;

protected:
	CScene* m_pScene;		// 所属シーン

	int m_nCurEventType; // 現在の進行中イベント
	//int m_nStopTime[MAX_TUTORIAL];
	std::vector<int> m_nStopTime;
	int m_nStopCnt;
	bool m_bEventPause; // イベント発生中ポーズのフラグ 

	//bool m_bFadeIn;  // イベント開始時にフェードインアウトする画像などに使うフラグ
	//bool m_bFadeOut;
	int m_nTexFade; // テクスチャフェード状態 
	int m_nTexDrawCnt; // テクスチャ描画時間カウンター

public:
	CEventManager(CScene*);
	~CEventManager();

	virtual HRESULT Init() = 0;
	virtual void Fin();
	virtual void Update();
	virtual void Draw();

	void StartEventType(int nType);
	int GetEventType(void) { return m_nCurEventType; }
	bool GetPause() { return m_bEventPause; }

	void SetEvent_ID(Event_ID Id) { m_Event_ID = Id; }
	Event_ID GetID() { return m_Event_ID; }

	void StartPause(int nStopCnt);
};

