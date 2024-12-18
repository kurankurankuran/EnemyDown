//=============================================================================
//
// シーン クラス実装 [Scene.cpp]
// Author : KAZUKI KURA
//
//=============================================================================
#include "Scene.h"
#include "Title.h"
#include "StageSelect.h"
#include "GameObj.h"
#include "GameOver.h"
#include "GameClear.h"
#include "Tutorial.h"
#include "Game_Scene.h"
#include "GameEditor_Scene.h"
#include <memory>
#include "Text.h"

// 静的メンバ
CScene* CScene::m_pScene = nullptr;
CScene* CScene::m_pTop = nullptr;

CPause CScene::m_Pause;
Octree CScene::m_Octree;
int CScene::m_nSlowTimeCnt;

CSaveManager CScene::m_SaveManager;

// コンストラクタ
CScene::CScene()
{
	m_id = SCENE_NONE;
	m_pObj = nullptr;
	m_pLastObj = nullptr;

	m_pBack = nullptr;
	m_pNext = m_pTop;
	if (m_pTop) {
		m_pTop->m_pBack = this;
	}
	m_pTop = this;
	m_pLight = nullptr;
	m_pEffekseerManager = nullptr;
}

// デストラクタ
CScene::~CScene()
{
	if (m_pNext) {
		m_pNext->m_pBack = m_pBack;
	}
	if (m_pBack) {
		m_pBack->m_pNext = m_pNext;
	} else {
		m_pTop = m_pNext;
	}
	m_pBack = m_pNext = nullptr;
}

// 初期化
bool CScene::Init()
{
	bool bHr;
	bHr = m_CameraManager.Init();
	CText::SetCamera(m_CameraManager.GetCamera());
	m_Light.Init();
	SetLight(&m_Light);
	m_nSlowTimeCnt = 0;
	return bHr;
}

// 終了処理
void CScene::Fin()
{
	m_pLight = nullptr;
}

// 初期化
bool CScene::InitAll()
{
	new CTitle;
	new CGameOver;
	new CGameClear;
	new CGame_Scene;
	new CGameEditor_Scene;
	//Change(SCENE_GAMEEDITOR);
	//Change(SCENE_GAME);
	Change(SCENE_TITLE);
	return true;
}

// 終了処理
void CScene::FinAll()
{
	Change(SCENE_NONE);

	CScene* pNext;
	for (CScene* pScene = m_pTop; pScene; pScene = pNext) {
		pNext = pScene->m_pNext;
		delete pScene;
	}
}

// 更新
void CScene::UpdateAll()
{
	if (m_pScene) {
		m_pScene->Update();
		if (m_pScene->GetCameraManager()->GetCamera()) {
			m_pScene->GetCameraManager()->GetCamera()->Update();
		}
	}
}

// 描画
void CScene::DrawAll()
{
	if (m_pScene) {
		if (m_pScene->GetCameraManager()->GetCamera()) {
			m_pScene->GetCameraManager()->GetCamera()->Clear();
		}
		m_pScene->Draw();
	}
}

// 変更
void CScene::Change(EScene scene)
{
	if (m_pScene) {
		m_pScene->Fin();
		m_pScene = nullptr;
		CText::DeleteTextAll();
	}
	if (scene) {
		for (CScene* pScene = m_pTop; pScene; pScene = pScene->m_pNext) {
			if (pScene->m_id == scene) {
				m_pScene = pScene;
				break;
			}
		}
		if (m_pScene) {
			m_pScene->Init();
		}
	}
}

// オブジェクト検索
CGameObj* CScene::Find(int id, CGameObj* pStart)
{
	CGameObj* pObj = (pStart) ? pStart->GetNext() : m_pObj;
	for (; pObj; pObj = pObj->GetNext()) {
		if (id < 0 || pObj->GetTag() == id)
			return pObj;
	}
	return nullptr;
}

// 半透明描画処理のため自身の処理順をリストの後ろに持っていく
void CScene::ToEnd(CGameObj* pObj)
{
	if (!pObj)return;
	// 末尾ポインタ取得
	CGameObj* pLastObj = m_pScene->GetLastObj();
	if (!pLastObj) return;
	
	// 先頭にいる時の連結ポインタ処理
	if (pObj->GetNext()) {
		pObj->GetNext()->SetBack(pObj->GetBack());
	}
	if (pObj->GetBack()) {
		pObj->GetBack()->SetNext(pObj->GetNext());
	}
	else
	{
		m_pScene->SetObj(pObj->GetNext());
	}

	// 末尾に移動後の連結ポインタ処理
	pObj->SetNext(pLastObj->GetNext());// 必ずnullになるはず
	pObj->SetBack(pLastObj);
	pLastObj->SetNext(pObj);

	m_pScene->SetLastObj(pObj);
}

// 光源設定
void CScene::SetLight(CLight* pLight)
{
	if (pLight) {
		m_pLight = pLight;
	} else {
		m_pLight = &m_Light;
	}
}
