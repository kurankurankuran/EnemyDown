#include"MGManager.h"
#include"Input.h"
#include"Sound.h"
#include"Texture.h"
#include"Polygon.h"
#include"EffekseerManager.h"
//#include "Camera.h"
#include"CameraManager.h"
namespace {
	const int g_nReLoadCnt = 120;
	const int g_nChangeNum = 100;
	const float g_fFireRate = 0.05f * 60.0f;
	const float g_fMoveDelCoefficient = 0.25f;              // �������ړI�n�܂Ŋ��炩�Ɉړ�����p�̌����W��
	const XMFLOAT3 g_fGeneratePos(100.0f, 100.0f, 0.0f); // �v���C���[��̌������΍��W
	const int g_nChargeRate = 15;                            // MP�����񕜃��[�g
}


//static TPartsInfo g_partsInit = 
//{
//  MODEL_MISSILE, MODEL_PLAYER_CANON, { 0.0f, 100.0f, 0.0f}, {0.0f, 0.0f, 0.0f},{ 1.0f, 1.0f, 1.0f}, true
//};


CMGManager::CMGManager(CScene* pScene) : CWeaponManager(pScene)
{
	
}

CMGManager::~CMGManager()
{

}

HRESULT CMGManager::Init(XMFLOAT4X4* pWorld)
{
	// �}�K�W����
	m_nMagazine = MAX_MACHINEGUN;
	// ���˃��[�g�����t���O
	m_bDelFireRate = false;
	// ���˃��[�g
	m_fFireRate = 0.0f;
	// ����`�F���W�\���Ԓ����p
	m_nChangeNum = g_nChangeNum;
	m_nChangeCnt = m_nChangeNum;
	// �e�I�u�W�F�N�g�̃��[���h�}�g���N�X
	m_pParentWorld = pWorld;
	// �����[�h�\���Ԓ����p
	m_nReloadCnt = g_nReLoadCnt;
	// �n�`�Ƃ̓����蔻��p
	m_pLand = (CLand*)m_pScene->Find(GOT_LAND);


	switch (m_pParent->GetTag())
	{
	case GOT_PLAYER:
		// ���ˌ�����
		m_nEffekseerHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::MACHINGUN_GENERATER_PLAYER_EFK), true, TIMER_MACHINGUN_GENERATER_PLAYER_EFK);
		break;
	case GOT_ENEMY:
		// ���ˌ�����
		m_nEffekseerHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::MACHINGUN_GENERATER_ENEMY_EFK), true, TIMER_MACHINGUN_GENERATER_ENEMY_EFK);
		break;
	case GOT_BOSS:
		// ���ˌ�����
		m_nEffekseerHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::MACHINGUN_GENERATER_ENEMY_EFK), true, TIMER_MACHINGUN_GENERATER_ENEMY_EFK);
		break;
	default:
		break;
	} 
	
	CEffekseerManager::SetMatrix(m_nEffekseerHandle, true, *m_pParentWorld);
	// �����ʒu
	m_vGeneratePos = XMFLOAT3(m_pParentWorld->_41, m_pParentWorld->_42, m_pParentWorld->_43);
	// MP�����񕜃��[�g
	m_nChargeCnt = 0;

	return true;
}

void CMGManager::Fin()
{
	CEffekseerManager::Stop(m_nEffekseerHandle, true);

	// ��������
	switch (m_pParent->GetTag())
	{
	case GOT_PLAYER:
		m_nEffekseerHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::MACHINGUN_GENERATER_PLAYER_EXPLOSION_EFK), false, TIMER_MACHINGUN_GENERATER_PLAYER_EXPLOSION_EFK);
		break;
	case GOT_ENEMY:
		m_nEffekseerHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::MACHINGUN_GENERATER_ENEMY_EXPLOSION_EFK), false, TIMER_MACHINGUN_GENERATER_ENEMY_EXPLOSION_EFK);
		break;
	case GOT_BOSS:
		m_nEffekseerHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::MACHINGUN_GENERATER_ENEMY_EXPLOSION_EFK), false, TIMER_MACHINGUN_GENERATER_ENEMY_EXPLOSION_EFK);
		break;
	default:
		break;
	}
	XMFLOAT4X4 mW = *m_pParentWorld;
	mW._41 = m_vGeneratePos.x;
	mW._42 = m_vGeneratePos.y;
	mW._43 = m_vGeneratePos.z;
	CEffekseerManager::SetMatrix(m_nEffekseerHandle, false, mW);
}

void CMGManager::Update()
{


	if (m_pParent) {
	
		if (!m_bVisible)return;

		// �����̈ʒu�X�V
		XMFLOAT4X4 mW = *m_pParentWorld;
		XMVECTOR vecX = XMVectorSet(mW._11, mW._12, mW._13, 1.0f);
		XMVECTOR vecY = XMVectorSet(mW._21, mW._22, mW._23, 1.0f);
		XMVECTOR vecZ = XMVectorSet(mW._31, mW._32, mW._33, 1.0f);
		XMVECTOR vecPos = XMVectorSet(mW._41, mW._42, mW._43, 1.0f);
		// �ړI�n�_
		XMVECTOR vecPurPosPos = vecPos + vecX * g_fGeneratePos.x + vecY * g_fGeneratePos.y + vecZ * g_fGeneratePos.z;
		// ���݈ʒu�X�V
		if (m_pScene->GetCameraManager()->GetMode() !=CCameraManager::ECameraMode::FPS_CAMERAMODE && m_eType == GOT_PLAYER)
		{
			XMStoreFloat3(&m_vGeneratePos, XMLoadFloat3(&m_vGeneratePos) + (vecPurPosPos - XMLoadFloat3(&m_vGeneratePos)) * g_fMoveDelCoefficient);
		}
		else
		{
			XMStoreFloat3(&m_vGeneratePos, XMLoadFloat3(&m_vGeneratePos) + (vecPurPosPos - XMLoadFloat3(&m_vGeneratePos)));
		}
		mW._41 = m_vGeneratePos.x;
		mW._42 = m_vGeneratePos.y;
		mW._43 = m_vGeneratePos.z;
		CEffekseerManager::SetMatrix(m_nEffekseerHandle, true, mW);

		// ����ύX�\���ԃJ�E���g�X�V
		if (m_nChangeCnt > 0) {
			--m_nChangeCnt;
		}
		// �e�̔��˃��[�g�X�V
		if (m_bDelFireRate)
		{
			--m_fFireRate;
			m_bDelFireRate = false;
			if (m_fFireRate < 0.0f)
			{
				m_fFireRate = g_fFireRate;
			}
		}
		else
		{
			m_fFireRate = 0.0f;
		}

		// MP��
		if (m_nMagazine < MAX_MACHINEGUN)
		{
			m_nChargeCnt++;
			if (g_nChargeRate < m_nChargeCnt)
			{
				m_nMagazine++;
				m_nChargeCnt = 0;
			}
		}
		else 
		{
			m_nChargeCnt = 0;
		}
	}



	
}

int CMGManager::Fire(int nDamage,int nTotalTime, int nDelay)
{
	// ���˃��[�g�����t���O�X�V
	m_bDelFireRate = true;
	// ���˃��[�g
	if (m_fFireRate > 0.0f)return 0;
	// ���˃��[�g���Z�b�g
	m_fFireRate = g_fFireRate;      
	// �}�K�W��������
	--m_nMagazine;
	if (m_nMagazine <= 0) 
	{
		m_nMagazine = 0;
		// �����[�h�\
		m_bCreate = true;
		return 0; 
	}

	// �e����
	CMachinegun* pMachinegun = new CMachinegun(m_pScene);
	// �����̒e���G�̒e���ݒ�
	pMachinegun->SetType(m_eType);
	// �n�`���f���̃|�C���^�Z�b�g�i�����蔻��p�j
	if(m_pLand)
		pMachinegun->SetLand(m_pLand);

	// ������
	pMachinegun->Init();
	// �_���[�W�ݒ�
	pMachinegun->SetDamage(nDamage);
	// �e�ݒ�
	pMachinegun->SetParent(m_pParent);
	// �����p�����[�^�ݒ�(�v���C���[�̑̂̂ǂ̈ʒu���甭�˂��邩���߂�ۂ͂�����ύX����)
	XMFLOAT3 vAngle, vScale, vPos;
	vAngle = XMFLOAT3(0, 0, 0);
	vScale = XMFLOAT3(1, 1, 1);
	vPos = XMFLOAT3(0, 0, 0);
	pMachinegun->GetTransform()->SetAngle(vAngle);
	pMachinegun->GetTransform()->SetScale(vScale);
	pMachinegun->GetTransform()->SetPos(vPos);
	pMachinegun->SetBSVisible((EBoundingShape)(m_pScene)->GetBound());

	// ���[���h�}�g���N�X�ɔ��f�����Ă���
	XMMATRIX mtxWorld, mtxScl, mtxRot, mtxTranslate;
	// ���[���h�}�g���N�X�̏�����
	mtxWorld = XMMatrixIdentity();
	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(vScale.x, vScale.y, vScale.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);
	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(vAngle.x),
		XMConvertToRadians(vAngle.y), XMConvertToRadians(vAngle.z));
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
	
	switch (m_eType)
	{
	case GOT_PLAYER:
	{
		// �J�����I�u�W�F�N�g�擾
		CCamera* pCamera = m_pScene->GetCameraManager()->GetCamera();
		if (!pCamera)return 0;
		// �J�����A���O���擾
		XMFLOAT3 vCameraAngle = pCamera->GetAngle();
		// �J�������[���h���W
		XMFLOAT4X4 mtxCameraWorld = pCamera->GetWorldMatrix();

		// �J�������[�h�ɂ���Č����o��������ς���
		if (m_pScene->GetCameraManager()->GetMode() != CCameraManager::ECameraMode::FPS_CAMERAMODE) {
			// �e�̃��[���h�s��ɂ������킹��
			mtxCameraWorld._41 = m_vGeneratePos.x;
			mtxCameraWorld._42 = m_vGeneratePos.y;
			mtxCameraWorld._43 = m_vGeneratePos.z;
			mtxWorld *= XMLoadFloat4x4(&mtxCameraWorld);

		}
		else {
			
			// �W�F�l���[�^�[���W����J�����̒����_�����ɔ��
			XMFLOAT4X4 mPlayerWorld = m_pParent->GetTransform()->GetWorld();
			mPlayerWorld._41 = mPlayerWorld._42 = mPlayerWorld._43 = 0.0f;


			// ���[���h�ϊ��s��ɔ��f
			mPlayerWorld._41 = m_vGeneratePos.x;
			mPlayerWorld._42 = m_vGeneratePos.y;
			mPlayerWorld._43 = m_vGeneratePos.z;

			// �i�s�����ݒ�
			mPlayerWorld._31 = mtxCameraWorld._31;
			mPlayerWorld._32 = mtxCameraWorld._32;
			mPlayerWorld._33 = mtxCameraWorld._33;

			// �e�̃��[���h�s��ɂ������킹��
			mtxWorld *= XMLoadFloat4x4(&mPlayerWorld);
		}

		// ���[���h�}�g���b�N�X�ݒ�
		XMStoreFloat4x4(&pMachinegun->GetTransform()->GetWorld(), mtxWorld);
		// �m�[�h�ɓo�^
		Octree* pOctree = CScene::GetOctree();
		pOctree->EraseAndPushObjInNode(pMachinegun->GetIndexWork(), pMachinegun);

		// �G�t�F�N�V�A����
		int nHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::MACHINGUN_PLAYER_EFK), true, TIMER_MACHINGUN_PLAYER_EFK);
		CEffekseerManager::SetMatrix(nHandle, true, pMachinegun->GetTransform()->GetWorld());
		pMachinegun->SetEfkHandle(nHandle);

		// ���ˉ�
		CSound::Play(SE_SHOT);
	}
		break;
	default:
	{
		// �W�F�l���[�^�[���W����e�̌����Ă�������ɔ��
		XMFLOAT4X4 mEnemyWorld = m_pParent->GetTransform()->GetWorld();
		mEnemyWorld._41 = m_vGeneratePos.x;
		mEnemyWorld._42 = m_vGeneratePos.y;
		mEnemyWorld._43 = m_vGeneratePos.z;

		// �e�I�u�W�F�N�g�̃��[���h�}�g���N�X�𔽉f
		mtxWorld *= XMLoadFloat4x4(&mEnemyWorld);
		// ���[���h�}�g���b�N�X�ݒ�
		XMStoreFloat4x4(&pMachinegun->GetTransform()->GetWorld(), mtxWorld);

		// �m�[�h�ɓo�^
		Octree* pOctree = CScene::GetOctree();
		pOctree->EraseAndPushObjInNode(pMachinegun->GetIndexWork(), pMachinegun);
		// �G�t�F�N�V�A����
		int nHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::MACHINGUN_ENEMY_EFK), true, TIMER_MACHINGUN_ENEMY_EFK);
		CEffekseerManager::SetMatrix(nHandle, true, pMachinegun->GetTransform()->GetWorld());
		pMachinegun->SetEfkHandle(nHandle);

		// ���ˉ�
		CSound::Play(SE_SHOT);
	}
		break;
	}

	return 1;
}

int CMGManager::Create( EGameObjTag Type, CModel* pEnemy)
{
	if (!m_pScene)
		return -1;

	if (!m_bCreate)
		return -1;
	// �����[�h�s�\(�e���g���؂�Ɖ\��)
	m_bCreate = false;
	// �}�K�W����[
	m_nMagazine = MAX_MACHINEGUN;
	// �G��������
	m_eType = Type;


	return 1;
}

