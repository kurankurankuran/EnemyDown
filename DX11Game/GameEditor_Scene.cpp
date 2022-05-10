//=============================================================================
//
// �Q�[���V�[���G�f�B�^�[�N���X���� [CGameEditor_Scene.cpp]
// Author : KAZUKI KURA
//
//=============================================================================
#include "GameEditor_Scene.h"
#include "DebugProc.h"
#include "Input.h"
#include "GameObj.h"
#include "Sound.h"
#include "Fade.h"
#include "EffekseerManager.h"
#include "Land.h"
#include "EnemyManager.h"
#include "DeathCube.h"
#include "Polygon.h"
#include "CreateObj.h"

// �R���X�g���N�^
CGameEditor_Scene::CGameEditor_Scene() : CScene()
{
	m_id = SCENE_GAMEEDITOR;
	m_nBound = 0;
}

// �f�X�g���N�^
CGameEditor_Scene::~CGameEditor_Scene()
{
}

// ������
bool CGameEditor_Scene::Init()
{
	GetCameraManager()->AddCamera(CCameraManager::ECameraMode::EDITOR_CAMERAMODE);
	GetCameraManager()->ChangeCamera(CCameraManager::ECameraMode::EDITOR_CAMERAMODE);
	
	// ���V�[��������
	if (!CScene::Init()) {
		return false;
	}

	// �G�f�B�^�[�C���X�^���X����������
	m_upEM = std::make_unique<CEditor_Manager>();
	m_upEM->Init();

	// �G�t�F�N�V�A������
	m_pEffekseerManager = new CEffekseerManager(this);
	m_pEffekseerManager->Init();
	m_pEffekseerManager->SetCamera(GetCameraManager()->GetCamera());

	// ***************** 
	// �I�u�W�F�N�g����
	// *****************
	CGameObj* pChangeStartTransFormGameObj;
	// �v���C���[
	CScene::GetSaveManager()->AddSaveData(pChangeStartTransFormGameObj = CCreateObj::CreateObj(OI_PLAYER));
	// �g�����X�t�H�[��
	pChangeStartTransFormGameObj->SetStartPos(XMFLOAT3(0, 0, 0));
	pChangeStartTransFormGameObj->SetStartAngle(XMFLOAT3(0, 0, 0));
	pChangeStartTransFormGameObj->SetStartScale(XMFLOAT3(1, 1, 1));

	// �G�l�~�[�}�l�[�W���[
	CScene::GetSaveManager()->AddSaveData(new CEnemyManager(this));
	// �n�`���f��
	CScene::GetSaveManager()->AddSaveData(CCreateObj::CreateObj(OI_LAND));
	// �R���f��
	const int nCliff = 4;
	CGameObj* pCliff[nCliff];
	for (int i = 0; i < nCliff; ++i)
	{
		CScene::GetSaveManager()->AddSaveData(pCliff[i] = CCreateObj::CreateObj(OI_CLIFF));
		//pCliff[i]->SetModel(EModelType::MT_CLIFF);
		pCliff[i]->SetObjID(EObjID::OI_CLIFF);
		pCliff[i]->SetCenter(XMFLOAT3(0.0f, 0.0f, 0.0f));
		pCliff[i]->SetBBox(XMFLOAT3(300.f, 2000.f, 5000.0f));
		pCliff[i]->SetStartScale(XMFLOAT3(2, 2, 2));
	}
	pCliff[0]->SetStartPos(XMFLOAT3(2700.0f, -500.0f, 0.0f));
	pCliff[0]->SetStartAngle(XMFLOAT3(0.0f, 0.0f, 0.0f));
	
	pCliff[1]->SetStartPos(XMFLOAT3(-2700.0f, -500.0f, 0.0f));
	pCliff[1]->SetStartAngle(XMFLOAT3(0.0f, 180.0f, 0.0f));

	pCliff[2]->SetStartPos(XMFLOAT3(0.0f, -500.0f, 2700.0f));
	pCliff[2]->SetStartAngle(XMFLOAT3(0.0f,-90.0f, 0.0f));

	pCliff[3]->SetStartPos(XMFLOAT3(0.0f, -500.0f, -2700.0f));
	pCliff[3]->SetStartAngle(XMFLOAT3(0.0f, 90.0f, 0.0f));

	// ���E�̕Ǘp�⃂�f��
	const int nRockNum = 3;// ���
	CGameObj* pWallRock[nRockNum];
	for (int i = 0; i < nRockNum; ++i)
	{
		CScene::GetSaveManager()->AddSaveData(pWallRock[i] = CCreateObj::CreateObj(OI_ROCK));
		//pWallRock[i]->SetModel(EModelType::MT_ROCK);
		pWallRock[i]->SetObjID(EObjID::OI_ROCK);
		pWallRock[i]->SetBBox(XMFLOAT3(150.f, 150.f, 150.0f));
		pWallRock[i]->SetStartAngle(XMFLOAT3(0.0f, 0.0f, 0.0f));
		pWallRock[i]->SetStartScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
		pWallRock[i]->SetStartPos(XMFLOAT3(0.0f, 0.0f, 0.0f));

	}
	pWallRock[0]->SetStartAngle(XMFLOAT3(0.0f, 0.0f, 0.0f));
	pWallRock[0]->SetStartScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	pWallRock[0]->SetStartPos(XMFLOAT3(1000.0f, 0.0f, -1000.0f));

	pWallRock[1]->SetStartAngle(XMFLOAT3(10.0f, 10.0f, 0.0f));
	pWallRock[1]->SetStartScale(XMFLOAT3(3.0f, 3.0f, 3.0f));
	pWallRock[1]->SetStartPos(XMFLOAT3(0.0f, 0.0f, -1300.0f));

	pWallRock[2]->SetStartAngle(XMFLOAT3(20.0f, 0.0f, 20.0f));
	pWallRock[2]->SetStartScale(XMFLOAT3(2.0f, 2.0f, 2.0f));
	pWallRock[2]->SetStartPos(XMFLOAT3(-1500.0f, 0.0f, 1000.0f));

	// �G�ꂽ�玀�ʃL���[�u
	CScene::GetSaveManager()->AddSaveData(pChangeStartTransFormGameObj = CCreateObj::CreateObj(OI_DEATHOBJ));
	pChangeStartTransFormGameObj->SetStartPos(XMFLOAT3(0.0f, -2000.0f, 0.0f));
	pChangeStartTransFormGameObj->SetStartAngle(XMFLOAT3(0.0f, 0.0f, 0.0f));
	pChangeStartTransFormGameObj->SetStartScale(XMFLOAT3(10000.0f, 50.0f, 10000.0f));

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
	CGameObj* pTrgetObj = CScene::Find(GOT_PLAYER);
	GetCameraManager()->GetCamera()->SetTargetObj(pTrgetObj);
	//GetMiniMapCamera()->SetTargetObj(pTrgetObj);
	GetCameraManager()->GetCamera()->SetSkyDome(CModel::GetAssimp(MT_SKY));


	// BGM�Đ��J�n
	CSound::Play(BGM_BATTLE);

	// ��ʒ����Ƀ}�E�X�J�[�\�����Œ�
	POINT ptCenter = GetScreenCenter();
	SetCursorPos(ptCenter.x, ptCenter.y);
	// �}�E�X�J�[�\���̔�\��
	//ShowCursor(FALSE);
	
	return true;
}

// �I������
void CGameEditor_Scene::Fin()
{
	// �I�����_�̃g�����X�t�H�[���ϐ���ۑ�(�{���̓G�f�B�^�[�V�[���ł̃Z�[�u���Ɍ��肳���ׂ�)
	CGameObj* pNext;
	for (CGameObj* pObj = m_pObj; pObj; pObj = pNext) {
		pNext = pObj->GetNext();
		pObj->SetStartPos(pObj->GetTransform()->GetPos());
		pObj->SetStartAngle(pObj->GetTransform()->GetAngle());
		pObj->SetStartScale(pObj->GetTransform()->GetScale());
	}
	

	// �Z�[�u
	CScene::GetSaveManager()->Save("data/SaveData/SaveData.txt");
	CScene::GetSaveManager()->ResetSaveData();

	// BGM�Đ���~
	CSound::Stop(BGM_BATTLE);

	// �S�I�u�W�F�N�g�I������
	CGameObj::FinAll(m_pObj);

	m_meshLine.Fin();
	
	// �G�t�F�N�V�A�I��
	m_pEffekseerManager->Uninit();
	SAFE_DELETE(m_pEffekseerManager);

	// �G�f�B�^�[�I��
	m_upEM->Fin();
	// �|�[�Y�I��
	//GetPause()->Fin();
}

// �X�V
void CGameEditor_Scene::Update()
{
	// �G�f�B�^�[�X�V
	m_upEM->Update();
	
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
		//CGameObj::UpdateAll(m_pObj);
		// �G�t�F�N�V�A�X�V
		m_pEffekseerManager->Update();
		
		GetLight()->Update();
		m_fRotValue += LIGHT_DIR_ROTVALUE;
		
	}	

	if (CInput::GetKeyTrigger(VK_N))
	{
		// �V�[���`�F���W
		CFade::Out(SCENE_GAME);
	}
}

// �`��
void CGameEditor_Scene::Draw()
{
	// �G�f�B�^�[�`��
	m_upEM->Draw();
	// �S�L�����`��
	CGameObj::DrawAll(m_pObj);
	// �G�t�F�N�V�A�`��
	m_pEffekseerManager->Draw();
}
