//=============================================================================
//
// �V�[�� �N���X���� [Scene.cpp]
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

// �ÓI�����o
CScene* CScene::m_pScene = nullptr;
CScene* CScene::m_pTop = nullptr;

CPause CScene::m_Pause;
Octree CScene::m_Octree;
int CScene::m_nSlowTimeCnt;

CSaveManager CScene::m_SaveManager;

// �R���X�g���N�^
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

// �f�X�g���N�^
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

// ������
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

// �I������
void CScene::Fin()
{
	m_pLight = nullptr;
}

// ������
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

// �I������
void CScene::FinAll()
{
	Change(SCENE_NONE);

	CScene* pNext;
	for (CScene* pScene = m_pTop; pScene; pScene = pNext) {
		pNext = pScene->m_pNext;
		delete pScene;
	}
}

// �X�V
void CScene::UpdateAll()
{
	if (m_pScene) {
		m_pScene->Update();
		if (m_pScene->GetCameraManager()->GetCamera()) {
			m_pScene->GetCameraManager()->GetCamera()->Update();
		}
	}
}

// �`��
void CScene::DrawAll()
{
	if (m_pScene) {
		if (m_pScene->GetCameraManager()->GetCamera()) {
			m_pScene->GetCameraManager()->GetCamera()->Clear();
		}
		m_pScene->Draw();
	}
}

// �ύX
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

// �I�u�W�F�N�g����
CGameObj* CScene::Find(int id, CGameObj* pStart)
{
	CGameObj* pObj = (pStart) ? pStart->GetNext() : m_pObj;
	for (; pObj; pObj = pObj->GetNext()) {
		if (id < 0 || pObj->GetTag() == id)
			return pObj;
	}
	return nullptr;
}

// �������`�揈���̂��ߎ��g�̏����������X�g�̌��Ɏ����Ă���
void CScene::ToEnd(CGameObj* pObj)
{
	if (!pObj)return;
	// �����|�C���^�擾
	CGameObj* pLastObj = m_pScene->GetLastObj();
	if (!pLastObj) return;
	
	// �擪�ɂ��鎞�̘A���|�C���^����
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

	// �����Ɉړ���̘A���|�C���^����
	pObj->SetNext(pLastObj->GetNext());// �K��null�ɂȂ�͂�
	pObj->SetBack(pLastObj);
	pLastObj->SetNext(pObj);

	m_pScene->SetLastObj(pObj);
}

// �����ݒ�
void CScene::SetLight(CLight* pLight)
{
	if (pLight) {
		m_pLight = pLight;
	} else {
		m_pLight = &m_Light;
	}
}
