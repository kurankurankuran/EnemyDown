// �ǔ��~�T�C�� [Machinegun.cpp]
#include "Machinegun.h"
#include "Scene.h"
#include "Enemy.h"
#include "Explosion.h"
//#include "Game.h"
//#include "EffectManager.h"
#include "Player.h"
#include "EffekseerManager.h"
#include "MGManager.h"
//#include "Texture.h"
//#include "Sound.h"
#include "Text.h"

// �萔
namespace {
	const float MOVE_SPEED = /*10*/270.0f;
	const float g_fSpeedEnemy = 25.0f;
	const int   MAX_LIFE =/* 30000*/300;
	const int   g_nDamage = 50;
	const int   g_nDamageEnemy = 10;

	const int DAMAGE_CNT = 100;
	const XMFLOAT2 DAMAGE_SCALE = XMFLOAT2(8.0f, 16.0f);
	const XMFLOAT4 DAMAGE_COLOR = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
}



// �R���X�g���N�^
CMachinegun::CMachinegun(CScene* pScene) : CWeapon(pScene)
{
	//SetTag(GOT_MACHINEGUN);
}

// �f�X�g���N�^
CMachinegun::~CMachinegun()
{
}

// ������
HRESULT CMachinegun::Init()
{
	HRESULT hr = S_OK;
	
	CGameObj::SetRadius(250.0f);
	CGameObj::SetBBox(XMFLOAT3(30.f, 30.f, 30.0f));
	m_pBBox->SetTrigger(true);
	m_vCenter = XMFLOAT3(0, 0, 0);

	// ���x�ݒ�
	if (m_Type == GOT_PLAYER)
	{
		m_fSpeed = MOVE_SPEED;
	}
	else
	{
		m_fSpeed = g_fSpeedEnemy;
	}
	// ���C�t�ݒ�
	m_Status.nHP = MAX_LIFE;
	
	return hr;
}

void CMachinegun::Fin()
{
	CEffekseerManager::Stop(m_nHandle, true);
	CModel::Fin();
}

// �X�V
void CMachinegun::Update()
{
	// �������ԍX�V
	--m_Status.nHP;
	if (m_Status.nHP <= 0) {
		Fin();
		delete this;
		return;

	}



	// ���[���h�ϊ��s��擾
	XMFLOAT4X4 mW = m_Transform.GetWorld();
	// ���f�����W���擾
	XMFLOAT3 vX = XMFLOAT3(mW._11, mW._12, mW._13);
	XMFLOAT3 vY = XMFLOAT3(mW._21, mW._22, mW._23);
	XMFLOAT3 vZ = XMFLOAT3(mW._31, mW._32, mW._33);
	// ���W�擾�A�N���A
	m_Transform.SetPos(XMFLOAT3(mW._41, mW._42, mW._43));
	mW._41 = mW._42 = mW._43 = 0.0f;
	m_Transform.SetVel(XMFLOAT3(0, 0, 0));

	// �O�i
	m_Transform.SetVel(
		XMFLOAT3(
			m_Transform.GetVel().x + vZ.x * m_fSpeed,
			m_Transform.GetVel().y + vZ.y * m_fSpeed,
			m_Transform.GetVel().z + vZ.z * m_fSpeed
		));

	//**********�����蔻��**********
	// �n�`�Ƃ̓����蔻��
	XMFLOAT3 X, N;
	if (m_pLand->Collision(
		this,
		XMFLOAT3(
			m_Transform.GetPos().x + m_Transform.GetVel().x,
			m_Transform.GetPos().y + m_Transform.GetVel().y + m_vBBox.y,
			m_Transform.GetPos().z + m_Transform.GetVel().z),// P0 ���C�̎n�_
		XMFLOAT3(
			m_Transform.GetPos().x + m_Transform.GetVel().x,
			m_Transform.GetPos().y + m_Transform.GetVel().y - m_vBBox.y,
			m_Transform.GetPos().z + m_Transform.GetVel().z),  // P1 ���C�̏I�_(�����n�ʂɂ߂荞�ނ܂Ń��C��L�΂��Ă���)
		&X, &N))
	{

		CSound::Play(SE_DEFEND);
		int Handle = 0;
		if (GOT_ENEMY == m_Type) {
			Handle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::HIT_EXPLOSION_ENEMY_EFK), false, TIMER_HIT_EXPLOSION_ENEMY_EFK);
		}
		else if (GOT_PLAYER == m_Type)
		{
			Handle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::HIT_EXPLOSION_PLAYER_EFK), false, TIMER_HIT_EXPLOSION_PLAYER_EFK);
		}
		else
		{
			Handle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::HIT_EXPLOSION_PLAYER_EFK), false, TIMER_HIT_EXPLOSION_PLAYER_EFK);
		}
		CEffekseerManager::SetMatrix(Handle, false, m_Transform.GetWorld());


		Fin();
		delete this;
		return;
	}




	// ���W�X�V
	// �ړ��x�N�g�����Z
	m_Transform.SetPos(
		XMFLOAT3(
			m_Transform.GetPos().x + m_Transform.GetVel().x,
			m_Transform.GetPos().y + m_Transform.GetVel().y,
			m_Transform.GetPos().z + m_Transform.GetVel().z
		));
	
	// ���[���h�ϊ��s��ɔ��f
	mW._41 = m_Transform.GetPos().x;
	mW._42 = m_Transform.GetPos().y;
	mW._43 = m_Transform.GetPos().z;
	m_Transform.SetWorld(mW);

	// �G�t�F�N�V�A�ɍ��W�𔽉f
	CEffekseerManager::SetMatrix(m_nHandle, true, m_Transform.GetWorld());

	// �m�[�h�ɓo�^
	Octree* pOctree = CScene::GetOctree();
	pOctree->EraseAndPushObjInNode(&m_NodeIndexWork, this);


	// �����̓o�^�m�[�h���ɂ���I�u�W�F�N�g�Ƃ̓����蔻��
	std::vector<CGameObj*> ColObjVector = CGameObj::CollisionOBB();
	if (ColObjVector.size() > 0)
	{
		switch (m_Type) {
			// �������v���C���[���甭�˂��ꂽ�e�Ȃ�
		case GOT_PLAYER:
			for (unsigned int i = 0; i < ColObjVector.size(); ++i)
			{
				// �Փˑ����ID���Ƃ̏���
				switch (ColObjVector[i]->GetTag())
				{
					// �G���{�X
				case GOT_ENEMY:
				case GOT_BOSS:
				{
					// ���肪����łȂ�
					if (ColObjVector[i]->GetStatus().nHP <= 0)continue;

					// ���C�t���炷����
					int nDamage = ColObjVector[i]->GetStatus().nRES - m_nDamage;
					if (nDamage < 0) {
						ColObjVector[i]->AddHP(nDamage);
						// �q�b�g�X�g�b�v
						ColObjVector[i]->SetHitStopTime(0.2f);
					}
					else
					{
						nDamage = 0;
					}

					// �_���[�W�e�L�X�g
					std::string DamegTest = std::to_string(-nDamage);
					CText::OutPutText(DamegTest, m_Transform.GetPos(), DAMAGE_SCALE, DAMAGE_COLOR, DAMAGE_CNT);

					// �T�E���h
					CSound::Play(SE_DEFEND);

					// �G�t�F�N�g
					int Handle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::HIT_EXPLOSION_PLAYER_EFK), false, TIMER_HIT_EXPLOSION_PLAYER_EFK);
					CEffekseerManager::SetMatrix(Handle, false, m_Transform.GetWorld());

					// �I������
					Fin();
					delete this;
					return;
				}
				// �v���C���[�Ȃ�X���[
				case GOT_PLAYER:

					break;
				// ���̃I�u�W�F�N�g�Ȃ玩��
				default:
				{
					// �G�t�F�N�g
					int Handle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::HIT_EXPLOSION_PLAYER_EFK), false, TIMER_HIT_EXPLOSION_PLAYER_EFK);
					CEffekseerManager::SetMatrix(Handle, false, m_Transform.GetWorld());

					// �I������
					Fin();
					delete this;
					return;
				}
				}

			}

			break;
			// �������G���{�X���甭�˂��ꂽ�e�Ȃ�
		case GOT_BOSS:
		case GOT_ENEMY:
			for (unsigned int i = 0; i < ColObjVector.size(); ++i)
			{
				// �Փˑ��肪�v���C���[�Ȃ�ȉ��̏���
				if (ColObjVector[i]->GetTag() == GOT_PLAYER) {
					// ���肪����łȂ�
					if (ColObjVector[i]->GetStatus().nHP <= 0)continue;
					// ���C�t���炷����
					int nDamage = ColObjVector[i]->GetStatus().nRES - m_nDamage;
					if (nDamage < 0) {
						ColObjVector[i]->AddHP(nDamage);
						// �q�b�g�X�g�b�v
						ColObjVector[i]->SetHitStopTime(0.2f);
					}
					else
					{
						nDamage = 0;
					}

					// �_���[�W�e�L�X�g
					std::string DamegTest = std::to_string(-nDamage);
					CText::OutPutText(DamegTest, m_Transform.GetPos(), DAMAGE_SCALE, DAMAGE_COLOR, DAMAGE_CNT);
					
					// �T�E���h
					CSound::Play(SE_PLAYER_DAMAGE);

					// �G�t�F�N�g
					int Handle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::HIT_EXPLOSION_ENEMY_EFK), false, TIMER_HIT_EXPLOSION_ENEMY_EFK);
					CEffekseerManager::SetMatrix(Handle, false, m_Transform.GetWorld());

					// �I������
					Fin();
					delete this;
					return;

				}
				// �e�I�u�W�F�N�g�ȊO�̃I�u�W�F�N�g�Ȃ玩��
				else if (ColObjVector[i] != m_pParent) {

					int Handle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::HIT_EXPLOSION_ENEMY_EFK), false, TIMER_HIT_EXPLOSION_ENEMY_EFK);
					CEffekseerManager::SetMatrix(Handle, false, m_Transform.GetWorld());

					// �I������
					Fin();
					delete this;
					return;
				}
			}
			break;
		}
	}



}

void CMachinegun::Draw()
{
	//if (m_bUnit)return;

	// �`��̓G�t�F�N�V�A�ōs��
}

// ����
int CMachinegun::Fire(XMFLOAT4X4* pWorld, EGameObjTag Type, CModel* pModel)
{
	CScene* pScene = CScene::GetCurrent();
	if (!pScene)
		return -1;
	CMachinegun* pMachinegun = new CMachinegun(pScene);
	pMachinegun->GetTransform()->SetWorld(*pWorld);
	pMachinegun->Init();
	pMachinegun->m_pTarget = pModel;
	pMachinegun->m_Type = Type;
	pMachinegun->SetBSVisible((EBoundingShape)(pScene)->GetBound());

	return 1;
}

