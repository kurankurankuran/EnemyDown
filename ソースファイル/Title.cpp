//=============================================================================
//
// �^�C�g����ʃN���X���� [Title.cpp]
// Author : KAZUKI KURA
//
//=============================================================================
#include"Title.h"
#include"Sound.h"
#include"Polygon.h"
#include"Fade.h"
#include"EffekseerManager.h"
#include"Text.h"
#include"CameraManager.h"

// �萔��`
namespace {
	
	
}

// �R���X�g���N�^
CTitle::CTitle() : CScene()
{
	m_id = SCENE_TITLE;

	m_nCurMenue = STAGESELECT_MENUE;
	m_nCurOption = SOUND_OPTION;
	m_nCurInput = MOUSESENSI_X_INPUT;
	m_nCurSound = MASTER_SOUND;
	
	m_nBound = 0;

	m_pWave = nullptr;
}

// �f�X�g���N�^
CTitle::~CTitle()
{
}

// ������
bool CTitle::Init()
{
	// �J�����ݒ�
	GetCameraManager()->AddCamera(CCameraManager::ECameraMode::STATIC_CAMERAMODE);
	GetCameraManager()->ChangeCamera(CCameraManager::ECameraMode::STATIC_CAMERAMODE);
	GetCameraManager()->GetCamera()->SetPos(200.0f, 300.0f, -200.0f);
	GetCameraManager()->GetCamera()->SetTargetObj(nullptr);
	GetCameraManager()->GetCamera()->SetTarget(0, 100, 0);

	// ���V�[��������
	if (!CScene::Init()) {
		return false;
	}
	
	// ***************** 
	// �I�u�W�F�N�g����
	// *****************

	// �X�J�C�h�[���ݒ�
	GetCameraManager()->GetCamera()->SetSkyDome(CModel::GetAssimp(MT_SKY));
	
	// �G�t�F�N�V�A������
	m_pEffekseerManager = new CEffekseerManager(this);
	m_pEffekseerManager->Init();
	m_pEffekseerManager->SetCamera(GetCameraManager()->GetCamera());

	
	// ���E�̕Ǘp�⃂�f��
	const int nRockNum = 1;// ���
	CModel* pWallRockModel[nRockNum];
	for (int i = 0; i < nRockNum; ++i)
	{
		pWallRockModel[i] = new CModel(this);
		pWallRockModel[i]->SetModel(MT_ROCK);
	}
	pWallRockModel[0]->SetStartPos(XMFLOAT3(-200.0f, -150.0f, 0.0f));
	pWallRockModel[0]->SetStartAngle(XMFLOAT3(0.0f, 0.0f, 0.0f));
	pWallRockModel[0]->SetStartScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	


	CModel* pPlayerModel = new CModel(this);
	pPlayerModel->SetModel(MT_PALADIN);
	pPlayerModel->SetStartPos(XMFLOAT3(-200.0f, 0.0f, 0.0f));
	pPlayerModel->SetStartAngle(XMFLOAT3(0.0f, 90.0f, 0.0f));
	pPlayerModel->SetStartScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	pPlayerModel->SetCamera(GetCameraManager()->GetCamera());
	pPlayerModel->SetLight(GetLight());
	pPlayerModel->SetAnimIndex(0);
	pPlayerModel->SetAnimTime(0);

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
	
	// ���ʐ���,������
	m_pWave = new CWave(this);
	m_pWave->Init();


	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	// ��ʃX�e�[�g
	m_State = TitleState::Start;

	// ���UI�C���X�^���X������
	m_UIManager.Init();

	// BGM�Đ��J�n
	CSound::Play(BGM_TITLE);
	// �}�E�X�J�[�\���̔�\��
	ShowCursor(FALSE);
	
	return true;
}

// �I������
void CTitle::Fin()
{
	
	// BGM�Đ���~
	CSound::Stop(BGM_TITLE);

	// ���UI�C���X�^���X�I��
	m_UIManager.Fin();

	// �S�I�u�W�F�N�g�I������
	CGameObj::FinAll(m_pObj);

	// ���ʏI��
	m_pWave->Fin();
	SAFE_DELETE(m_pWave);

	
	// �G�t�F�N�V�A�I��
	m_pEffekseerManager->Uninit();
	delete m_pEffekseerManager;
	
	CScene::Fin();
}

// �X�V
void CTitle::Update()
{
	// �S�L�����X�V
	CGameObj::UpdateAll(m_pObj);

	// ���ʍX�V
	m_pWave->Update();

	// ���UI�C���X�^���X�X�V
	m_UIManager.Update();

	// �G�t�F�N�V�A�X�V
	m_pEffekseerManager->Update();

}

// �`��
void CTitle::Draw()
{
	GetLight()->SetEnable(true);
	// �S�L�����`��
	CGameObj::DrawAll(m_pObj);

	// ���ʕ`��
	m_pWave->Draw();

	// ���UI�C���X�^���X�`��
	m_UIManager.Draw();

	// �G�t�F�N�V�A�`��
	m_pEffekseerManager->Draw();

}
