//=============================================================================
//
// �G�f�B�^�[�V�[�� �N���X���� [CEditor_Scene.cpp]
// Author : KAZUKI KURA
//
//=============================================================================
#include"Game_Scene.h"
#include"DebugProc.h"
#include"Input.h"
#include"GameObj.h"
#include"Sound.h"
#include"Fade.h"
#include"Pause.h"
#include"EffekseerManager.h"
#include"Polygon.h"
#include"CameraManager.h"


// �R���X�g���N�^
CGame_Scene::CGame_Scene() : CScene()
{
	m_id = SCENE_GAME;

	m_nBound = 0;
}

// �f�X�g���N�^
CGame_Scene::~CGame_Scene()
{
}

// ������
bool CGame_Scene::Init()
{
	// �g�p����J������ǉ�
	GetCameraManager()->AddCamera(CCameraManager::ECameraMode::TPS_CAMERAMODE);
	GetCameraManager()->AddCamera(CCameraManager::ECameraMode::FPS_CAMERAMODE);
	GetCameraManager()->AddCamera(CCameraManager::ECameraMode::MINIMAP_CAMERAMODE);
	GetCameraManager()->ChangeCamera(CCameraManager::ECameraMode::TPS_CAMERAMODE);
	// ���V�[��������
	if (!CScene::Init()) {
		return false;
	}

	// �|�[�Y������
	GetPause()->Init();
	GetPause()->SetCamera(GetCameraManager()->GetCamera());

	// �G�t�F�N�V�A������
	m_pEffekseerManager = new CEffekseerManager(this);
	m_pEffekseerManager->Init();
	m_pEffekseerManager->SetCamera(GetCameraManager()->GetCamera());

	// ***************** 
	// �I�u�W�F�N�g����
	// *****************
	CScene::GetSaveManager()->Load("data/SaveData/SaveData.txt");
	
	// ************************* 
	// �I�u�W�F�N�g���������܂�
	// *************************

	// �ŏ��̑S�I�u�W�F�N�g������
	if (FAILED(CGameObj::AwakeAll(m_pObj))) {
		return false;
	}
	
	
	// �S�I�u�W�F�N�g������
	if (FAILED(CGameObj::InitAll(m_pObj))) {
		return false;
	}
	
	
	// �e��J�����Ƀ^�[�Q�b�g�I�u�W�F�N�g��ݒ�
	CGameObj* pTargetObj = CScene::Find(GOT_PLAYER);
	GetCameraManager()->GetCamera()->SetTargetObj(pTargetObj);
	GetCameraManager()->GetMiniMapCamera()->Init();
	GetCameraManager()->GetMiniMapCamera()->SetTargetObj(pTargetObj);
	//GetMiniMapCamera()->SetTargetObj(pTrgetObj);
	// �X�J�C�h�[���ݒ�
	//GetCameraManager()->GetCamera()->SetSkyDome(CModel::GetAssimp(MT_SKY));
	GetCameraManager()->GetMiniMapCamera()->SetSkyDome(CModel::GetAssimp(MT_SKY));
	//SetMiniMapCamera(&m_MiniMapCamera);

	// BGM�Đ��J�n
	CSound::Play(BGM_BATTLE);

	// ��ʒ����Ƀ}�E�X�J�[�\�����Œ�
	CInput::SetRockCursorFlag(true);
	POINT ptCenter = GetScreenCenter();
	SetCursorPos(ptCenter.x, ptCenter.y);
	// �}�E�X�J�[�\���̔�\��
	ShowCursor(FALSE);
	
	return true;
}

// �I������
void CGame_Scene::Fin()
{
	// BGM�Đ���~
	CSound::Stop(BGM_BATTLE);

	// �S�I�u�W�F�N�g�I������
	CGameObj::FinAll(m_pObj);

	// �G�t�F�N�V�A�I��
	m_pEffekseerManager->Uninit();
	SAFE_DELETE(m_pEffekseerManager);

	// �|�[�Y�I��
	GetPause()->Fin();

	// �~�j�}�b�v�I��
	//m_MiniMapCamera.Fin();
	//SetMiniMapCamera(nullptr);
}

// �X�V
void CGame_Scene::Update()
{
	GetPause()->Update();
	
	// ���E��/���E�{�b�N�X�\���ؑ�
	if (CInput::GetKeyTrigger(VK_B)) {
		CGameObj* pObj = Find();
		m_nBound = (m_nBound + MAX_BOUND - 1) % MAX_BOUND;
		while (pObj) {
			switch (pObj->GetTag()) {
			case GOT_LAND:
				// �`�悵�Ȃ�
				break;
			default:
				pObj->SetBSVisible((EBoundingShape)m_nBound);
				break;
			}
			pObj = Find(-1, pObj);
		}
	}

	if (!GetPause()->GetPauseFlg() && CFade::Get() == FADE_NONE) {
		// �S�L�����X�V
		CGameObj::UpdateAll(m_pObj);
		// �G�t�F�N�V�A�X�V
		m_pEffekseerManager->Update();
		
		//m_light.Update();
		GetLight()->Update();
		
	}

	GetCameraManager()->GetMiniMapCamera()->Update();
	//m_MiniMapCamera.Update();

	// �f�o�b�O�\��
	CDebugProc::Print("*** ����� ˮ��� ***\n");
	CDebugProc::Print("None/Sphere/AABB/OBB:[B]\n");
	CDebugProc::Print("\n");
}

// �`��
void CGame_Scene::Draw()
{
	// �S�L�����`��
	CGameObj::DrawAll(m_pObj);
	// �G�t�F�N�V�A�`��
	m_pEffekseerManager->Draw();
	// �S�L����UI�`��
	CGameObj::DrawUIAll(m_pObj);

	// �~�j�}�b�v�`��
	GetCameraManager()->GetMiniMapCamera()->Clear();
	GetCameraManager()->GetMiniMapCamera()->Draw();
	/*m_MiniMapCamera.Clear();
	m_MiniMapCamera.Draw();*/
	//GetMiniMapCamera()->Draw();

	// �|�[�Y�`��
	GetPause()->Draw();
}
