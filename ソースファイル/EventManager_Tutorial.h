//// チュートリアルイベントフラグ管理クラス EventManager_Tutorial.h
//#pragma once
//#include "main.h"
//#include "EventManager.h"
//#include "Camera.h"
//
//typedef enum {
//	MOVE_TUTORIAL = 0,
//	ATTACK_TUTORIAL,
//	FIGHT_TUTORIAL,
//
//	MAX_TUTORIAL
//}TUTORIAL_TYPE;
//
//namespace {
//	const int MAX_TEXTURE = 3;
//}
//
//class CEnemyManager;
//class CEventManager_Tutorial :public CEventManager
//{
//private:
//	ID3D11ShaderResourceView*	m_pTex[MAX_TEXTURE];	// ポーズメニューテクスチャ
//	ID3D11ShaderResourceView*	m_pTexMoveTutorial;    
//	ID3D11ShaderResourceView*	m_pTexWeaponTutorial;
//	ID3D11ShaderResourceView*	m_pTexPauseTutorial;
//	float m_fAlpha/*[MAX_TEXTURE]*/;
//	CCamera* m_pCamera;
//	CEnemyManager* m_pEnemyManager;
//	bool m_bSetCameraMode; // カメラモード変更フラグ
//	bool m_bEvent;         // イベント進行フラグ
//protected:	
//	
//public:
//	CEventManager_Tutorial(CScene*);
//	~CEventManager_Tutorial();
//
//	HRESULT Init();
//	void Fin();
//	void Update();
//	void Draw();
//};
//
