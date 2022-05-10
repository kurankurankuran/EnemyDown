#include "SwordAttackManager.h"
#include "Model.h"
#include "Scene.h"
#include "Sound.h"
#include <DirectXMath.h>
#include "TextureData.h"


namespace {
	// �S���[�V��������
	const int ALLMOTION_COUNT_PLAYER = 60 * 3;
	
	const XMFLOAT3 GENERATE_POS_PLAYER =   XMFLOAT3(-10.0f, 0.0f,  -90.0f);
	const XMFLOAT3 GENERATE_SCALE_PLAYER = XMFLOAT3(3.0f, 3.0f, 17.5f);
	const XMFLOAT3 GENERATE_ANGLE_PLAYER = XMFLOAT3(0.0f,   15.0f,   0.0f);
	const std::string BONE_NAME_PLAYER = std::string("Sword_joint");

	const XMFLOAT3 GENERATE_POS_MUTANT = XMFLOAT3(125.0f, 0.0f, -20.0f);
	const XMFLOAT3 GENERATE_SCALE_MUTANT = XMFLOAT3(15.0f, 3.0f, 3.0f);
	const XMFLOAT3 GENERATE_ANGLE_MUTANT = XMFLOAT3(0.0f, 40.0f,  0.0f);
	const std::string BONE_NAME_MUTANT = std::string("Mutant:LeftForeArm");

	const XMFLOAT3 GENERATE_POS_MARIA   = XMFLOAT3(-40.0f,10.0f, -70.0f);
	const XMFLOAT3 GENERATE_SCALE_MARIA = XMFLOAT3(  3.0f, 3.0f, 30.0f);
	const XMFLOAT3 GENERATE_ANGLE_MARIA = XMFLOAT3(  5.0f, 30.0f, 0.0f);
	const std::string BONE_NAME_MARIA = std::string("RightHand");
}

CSwordAttackManager::CSwordAttackManager(CScene* pScene) : CWeaponManager(pScene)
{
	
}

CSwordAttackManager::~CSwordAttackManager()
{

}

HRESULT CSwordAttackManager::Init(XMFLOAT4X4* pWorld)
{
	m_nDamage = 0;
	m_bVisible = false;
	m_bAttackflg = false;
	m_pParentWorld = pWorld;
	m_nTime = 0;
	m_nTimeCnt = 0;
	m_nStartTime = 0;

	m_vPos = XMFLOAT3(0, 0, 0);
	m_vAngle = XMFLOAT3(0, 0, 0);
	m_vScale = XMFLOAT3(0, 0, 0);

	
	return true;
}

void CSwordAttackManager::Fin()
{
	for (unsigned int i = 0; i < m_pWeapon.size(); ++i) {
		if (m_pWeapon[i]) {
			delete m_pWeapon[i];
			m_pWeapon[i] = nullptr;
		}
	}
	m_pWeapon.clear();
}

void CSwordAttackManager::Update()
{
	if (m_pParent) {
		m_bOldVisible = m_bVisible;
		m_bVisible = m_pParent->GetVisible();
		// �e���s���Ȃ玩�����s���ɂȂ�
		if (m_bVisible != m_bOldVisible) {
			if (m_bVisible) {
				SetVisible(true);
			}
			else
			{
				SetVisible(false);
			}
		}

		// �e���q�b�g�o�b�N��ԂɂȂ����炷���ɍU����������
		CModel* pParentModel = (CModel*)m_pParent;
		if (CharaState::HitBack == pParentModel->GetState())
		{
			m_bAttackflg = false;
			m_nTimeCnt = m_nTime;
			Create(m_eType);
		}

		if (m_bAttackflg)
		{
			m_nTimeCnt--;
			if (m_nTimeCnt < 0)
			{
				m_nTimeCnt = 0;
			}

			if (m_nTimeCnt == m_nStartTime)
			{
				// �}�K�W��������
				--m_nMagazine;
				if (m_nMagazine <= 0)
				{
					m_nMagazine = 0;
					// �����[�h�\
					m_bCreate = true;
					//return 0;
				}
				
				// �e����
				CSwordAttack* pSwordAttack = new CSwordAttack(m_pScene);
			
				// �����̒e���G�̒e���ݒ�
				pSwordAttack->SetType(m_eType);
				// �e�N�X�`���ݒ�
				pSwordAttack->SetTexture(m_TexName);
				// ������
				pSwordAttack->Init();
				// �����p�����[�^�ݒ�(�{�[���ʒu��̃��[�J���g�����X�t�H�[��)
				pSwordAttack->GetTransform()->SetAngle(m_vAngle);
				pSwordAttack->GetTransform()->SetScale(m_vScale);
				pSwordAttack->GetTransform()->SetPos(m_vPos);
				// ���̋O�Ղ̒�����ݒ�
				pSwordAttack->SetTraceScale(m_vScale);
				// �_���[�W�ݒ�
				pSwordAttack->SetDamage(m_nDamage);
				// ����̎�������
				pSwordAttack->SetMaxHP(m_nTimeCnt);
				pSwordAttack->SetHP(m_nTimeCnt);
				// �����蔻���
				pSwordAttack->SetBSVisible((EBoundingShape)(m_pScene)->GetBound());
				// �����n�_�̊�ƂȂ�{�[���̖��O
				std::string strBoneName = m_strBoneName;
				pSwordAttack->SetBoneName(strBoneName);

				// ���[���h�}�g���N�X�ɔ��f�����Ă���
				XMMATRIX mtxWorld, mtxScl, mtxRot, mtxTranslate;
				// ���[���h�}�g���N�X�̏�����
				mtxWorld = XMMatrixIdentity();
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(m_pParentWorld));

				// �����ʒu����{�[���̈ʒu�ɍ��킹��
				CModel* pParentModel = (CModel*)m_pParent;
				pParentModel->SetAnimTime(pParentModel->GetAnimTime());
				XMFLOAT4X4 AnimGlobalMat = pParentModel->GetBoneGlobalMatrix(strBoneName);
				mtxWorld = XMLoadFloat4x4(&AnimGlobalMat) * mtxWorld;
				// �e�I�u�W�F�N�g�̃|�C���^��n��
				pSwordAttack->SetParentObj(m_pParent);

				// ���̓����蔻��̃��[�J���F�ʒu��]�傫����ݒ�
				XMMATRIX mtxLocalWorld;
				mtxLocalWorld = XMMatrixIdentity();
				// �X�P�[���𔽉f
				mtxScl = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
				mtxLocalWorld = XMMatrixMultiply(mtxLocalWorld, mtxScl);
				// ��]�𔽉f
				mtxRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_vAngle.x),
					XMConvertToRadians(m_vAngle.y), XMConvertToRadians(m_vAngle.z));
				mtxLocalWorld = XMMatrixMultiply(mtxLocalWorld, mtxRot);
				// �ړ��𔽉f
				mtxTranslate = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);
				mtxLocalWorld = XMMatrixMultiply(mtxLocalWorld, mtxTranslate);

				mtxWorld = mtxLocalWorld * mtxWorld;

				// ���[���h�}�g���b�N�X�ݒ�
				XMStoreFloat4x4(&pSwordAttack->GetTransform()->GetWorld(), mtxWorld);
				// �m�[�h�ɓo�^
				Octree* pOctree = CScene::GetOctree();
				pOctree->EraseAndPushObjInNode(pSwordAttack->GetIndexWork(), pSwordAttack);

				CSound::Play(SE_SWORD_SLASH);
			}
			else if (m_nTimeCnt <= 0)
			{
				m_bAttackflg = false;
				m_nTimeCnt = m_nTime;
				Create(m_eType);
			}
		}

		

	}
}

int CSwordAttackManager::Fire(int nDamage, int nTotalTime,int nDelay)
{
	if (!m_bAttackflg)
	{
		m_bAttackflg = true;

		m_nDamage = nDamage;

		if (nTotalTime) 
		{
			m_nTime = nTotalTime;
			m_nTimeCnt = m_nTime;
		}
		if (nDelay)
		{
			m_nStartTime = m_nTime - nDelay;
		}
		return 1;
	}
	else {
		return 0;
	}
}


int CSwordAttackManager::Create(EGameObjTag Type, CModel* pEnemy)
{
	if (!m_pScene)
		return -1;

	if (!m_bCreate)
		return -1;
	// �����[�h�s�\(�e���g���؂�Ɖ\��)
	m_bCreate = false;
	// �U�����t���O��false
	m_bAttackflg = false;
	// �}�K�W����[
	m_nMagazine = MAX_SWORDATTACK;
	// �G��������
	m_eType = Type;

	
	switch (m_eType)
	{
	case GOT_PLAYER:
		m_vPos = GENERATE_POS_PLAYER;
		m_vAngle = GENERATE_ANGLE_PLAYER;
		m_vScale = GENERATE_SCALE_PLAYER;
		m_strBoneName = BONE_NAME_PLAYER.c_str();
		m_TexName = GAGE_WAKU;
		break;
	case GOT_ENEMY:
	
		m_vPos = GENERATE_POS_MUTANT;
		m_vAngle = GENERATE_ANGLE_MUTANT;
		m_vScale = GENERATE_SCALE_MUTANT;
		m_strBoneName = BONE_NAME_MUTANT.c_str();
		m_TexName = GAGE_HAIKEI;
		break;
	case GOT_BOSS:
		m_vPos = GENERATE_POS_MARIA;
		m_vAngle = GENERATE_ANGLE_MARIA;
		m_vScale = GENERATE_SCALE_MARIA;
		m_strBoneName = BONE_NAME_MARIA.c_str();
		m_TexName = GAGE_HAIKEI;
		break;

	default:
		m_vPos = GENERATE_POS_PLAYER;
		m_vAngle = GENERATE_ANGLE_PLAYER;
		m_vScale = GENERATE_SCALE_PLAYER;

		m_TexName = GAGE_HAIKEI;
		break;
	}
	
	
	return 1;
}

void CSwordAttackManager::SetVisible(bool bVisible)
{
	if (m_pParent)
	{
		for (unsigned int i = 0; i < m_pWeapon.size(); ++i)
		{
			if (m_pWeapon[i]) {
				m_pWeapon[i]->SetVisible(bVisible);
			}
		}
	}
}
