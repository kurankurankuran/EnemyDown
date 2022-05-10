//// ゲーム開始、終わりイベントフラグ管理クラス EventManager_StartEnd.h
//#pragma once
//#include "main.h"
//#include "EventManager.h"
//#include "Camera.h"
//
//typedef enum {
//	START_STARTEND = 0,
//	WAVE_STARTEND,
//	END_STARTEND,
//	
//	MAX_STARTEND
//}STARTEND;
//
//class CEnemyManager;
//class CEventManager_StartEnd :public CEventManager
//{
//private:
//	ID3D11ShaderResourceView*	m_pTex[MAX_STARTEND];	// ゲームスタート、ゲームクリアーテクスチャ
//	ID3D11ShaderResourceView*	m_pTextureNum;	// 数字テクスチャ
//	float m_fAlpha;
//	bool m_bCreate;
//	CEnemyManager* m_pEnemyManager;
//	CCamera* m_pCamera;
//	bool m_bSetCameraMode; // カメラモード変更フラグ
//	
//protected:	
//	
//
//public:
//	CEventManager_StartEnd(CScene*);
//	~CEventManager_StartEnd();
//
//	HRESULT Init();
//	void Fin();
//	void Update();
//	void Draw();
//};
//
