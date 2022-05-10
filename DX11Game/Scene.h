//=============================================================================
//
// シーン クラス定義 [Scene.h]
// Author : KAZUKI KURA
//
//=============================================================================
#pragma once
#include"CameraManager.h"
#include"Light.h"
#include"Pause.h"
#include"SaveManager.h"
#include<vector>


// *******定数定義*****
// シーン番号
enum EScene {
	SCENE_NONE = 0,		// (シーン無)
	SCENE_TITLE,		// タイトル画面
	SCENE_SELECT,		// ステージセレクト画面
	SCENE_TUTORIAL,		// チュートリアル画面
	SCENE_GAME,			// ゲーム画面
	SCENE_GAMEOVER,     // ゲームオーバー画面
	SCENE_GAMECLEAR,    // ゲームクリアー画面
	SCENE_GAMEEDITOR,	// エディターシーン

	MAX_SCENE
};

// クラス定義
class CEffekseerManager;
class CGameObj;
class Octree;
class CScene {
protected:
	EScene m_id;
	CGameObj* m_pObj;// 連結リストの最前列
	CGameObj* m_pLastObj;// 連結リストの最後尾
	
	// エフェクシアマネージャー
	CEffekseerManager*           m_pEffekseerManager;

	int m_nBound;       // 現在の当たり判定の種類
	static int m_nSlowTimeCnt; // アップデートを停止する(時止め)フレーム数カウント

	
private:
	static CScene* m_pScene;
	static CScene* m_pTop;
	CScene* m_pBack;
	CScene* m_pNext;
	CCameraManager m_CameraManager;
	CLight* m_pLight;
	CLight  m_Light;

	static CPause m_Pause;
	static Octree m_Octree;
	static CSaveManager m_SaveManager;
public:
	CScene();
	virtual ~CScene();

	virtual bool Init();
	virtual void Fin();
	virtual void Update() = 0;
	virtual void Draw() = 0;

	static bool InitAll();
	static void FinAll();
	static void UpdateAll();
	static void DrawAll();

	static void Change(EScene scene);

	CGameObj* GetObj() { return m_pObj; }
	void SetObj(CGameObj* pObj) { m_pObj = pObj; }
	CGameObj* GetLastObj() { return m_pLastObj; }
	void SetLastObj(CGameObj* pObj) { m_pLastObj = pObj; }
	CGameObj* Find(int id = -1, CGameObj* pStart = nullptr);
	void ToEnd(CGameObj* pObj);
	CLight* GetLight() { return m_pLight; }
	void SetLight(CLight* pLight);
	CCameraManager* GetCameraManager() { return &m_CameraManager; }
	EScene GetID() { return m_id; }
	int GetBound() { return m_nBound; }
	static CScene* GetCurrent() { return m_pScene; }
	static CPause* GetPause() { return &m_Pause; }
	static Octree* GetOctree() { return &m_Octree; }
	static int GetTimeSlowCnt() { return m_nSlowTimeCnt; }
	static void SetTimeSlowCnt(int nStopTime) { m_nSlowTimeCnt = nStopTime; }
	static CSaveManager* GetSaveManager() { return &m_SaveManager; }
};
