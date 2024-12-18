//=============================================================================
//
// �V�[�� �N���X��` [Scene.h]
// Author : KAZUKI KURA
//
//=============================================================================
#pragma once
#include"CameraManager.h"
#include"Light.h"
#include"Pause.h"
#include"SaveManager.h"
#include<vector>


// *******�萔��`*****
// �V�[���ԍ�
enum EScene {
	SCENE_NONE = 0,		// (�V�[����)
	SCENE_TITLE,		// �^�C�g�����
	SCENE_SELECT,		// �X�e�[�W�Z���N�g���
	SCENE_TUTORIAL,		// �`���[�g���A�����
	SCENE_GAME,			// �Q�[�����
	SCENE_GAMEOVER,     // �Q�[���I�[�o�[���
	SCENE_GAMECLEAR,    // �Q�[���N���A�[���
	SCENE_GAMEEDITOR,	// �G�f�B�^�[�V�[��

	MAX_SCENE
};

// �N���X��`
class CEffekseerManager;
class CGameObj;
class Octree;
class CScene {
protected:
	EScene m_id;
	CGameObj* m_pObj;// �A�����X�g�̍őO��
	CGameObj* m_pLastObj;// �A�����X�g�̍Ō��
	
	// �G�t�F�N�V�A�}�l�[�W���[
	CEffekseerManager*           m_pEffekseerManager;

	int m_nBound;       // ���݂̓����蔻��̎��
	static int m_nSlowTimeCnt; // �A�b�v�f�[�g���~����(���~��)�t���[�����J�E���g

	
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
