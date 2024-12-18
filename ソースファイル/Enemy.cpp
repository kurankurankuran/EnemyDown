// �G�@ [Enemy.cpp]
#include "Enemy.h"
#include "Input.h"
#include "Player.h"
#include "Sound.h"
//#include "EffectManager.h"
#include "EffekseerManager.h"
#include "Polygon.h"
//#include "Texture.h"
#include "TextureData.h"
#include "EnemyManager.h"

// �萔
namespace {
	// ********�X�e�[�^�X********
	const int STATUS_MAX_LIFE = 2500;   // �ő僉�C�t
	const int STATUS_ATK = 500;		    // �����U����
	const int STATUS_DEF = 500;		    // �����h���
	const int STATUS_INT = 50;		    // ���@�U����
	const int STATUS_RES = 25;		    // ���@�h���

	// *********�ړ��p�ϐ�*********
	const float MOVE_ACCEL = 0.2f;      // �����x
	const float VEL_MAX = 2.5f;         // �ō����x
	const float ROLLSPEED = 0.5f;       // �U��������x�@�P��:�x
	const float SEACH_RAD = 5000.0f;    // ���G�͈�

	// *********���C�tUI�p*********
	const XMFLOAT2 POS_LIFE_OFFSET = XMFLOAT2(0.0f, 15.0f);  // ���C�t�Q�[�W�̍��W(�G�)
	const XMFLOAT2 SIZE_LIFEWAKU = XMFLOAT2(40.0f, 7.5f);    // ���C�t�Q�[�W�̘g�\���T�C�Y
	const XMFLOAT2 WAKUTHICKNESS = XMFLOAT2(2.5f, 2.5f);     // �g�̌���
	const XMFLOAT2 LIFESIZEMAX =                             // �c��Q�[�W�̉��Əc�̑傫��
		XMFLOAT2(SIZE_LIFEWAKU.x - WAKUTHICKNESS.x, SIZE_LIFEWAKU.y - WAKUTHICKNESS.y);


	// *********�U���p�萔*********
	const float ATTACK_BS_RAD = 20.0f;  // �U���͈�
	const int ALLMOTION_ATTACK = 30;    // ���U�����[�V��������
	const int START_TIME_ATTACK = 18;   // �U������o������(�����J�n�ォ��̌o�ߎ���)
	const float TECRATE_DEFAULT = 1.0f; // �Z���Ƃ̃_���[�W�{��

	// *********�A�j���[�V�����p�萔*********
	const double ANIM_SPEED = 0.015;    // �A�j���[�V�����Đ����x
	// �A�j���[�V�����̎��
	const int ANIM_IDLE     = 0;		// �ҋ@
	const int ANIM_WALK     = 1;		// ���s
	const int ANIM_DYING    = 2;		// ��
	const int ANIM_ATTACK   = 4;		// �U��
	const int ANIM_HITBACK  = 5;        // ��_���[�W
										
}

// �R���X�g���N�^
CEnemy::CEnemy(CScene* pScene) : CModel(pScene)
{
	SetTag(GOT_ENEMY);
	m_pTarget = nullptr;
	m_pLand = nullptr;
	m_Transform.SetMoveAccel(MOVE_ACCEL);
	m_Transform.SetVelMax(VEL_MAX);
	m_fAtkRad = ATTACK_BS_RAD;
	m_fSeachRad = SEACH_RAD;
	m_empEBaseAnimationToInt[IDLE_BA] = ANIM_IDLE;
	m_empEBaseAnimationToInt[WALK_BA] = ANIM_WALK;
	m_empEBaseAnimationToInt[DYING_BA] = ANIM_DYING;
	m_empEBaseAnimationToInt[HITBACK_BA] = ANIM_HITBACK;

}

// �f�X�g���N�^
CEnemy::~CEnemy()
{
	

}

// ������
HRESULT CEnemy::Init()
{
	HRESULT hr = CModel::Init();
	// �s��
	m_bVisible = false;
	// �����G�t�F�N�g�J�n
	m_nCreateEffectTimer = TIMER_ENEMY_CREATE_EFK;
	m_nEfkID = CEffekseerManager::Play(CEffekseerManager::GetPath(ENEMY_CREATE_EFK), false, TIMER_ENEMY_CREATE_EFK);
	CEffekseerManager::SetMatrix(m_nEfkID, false, m_Transform.GetWorld());
	CEffekseerManager::SetScale(m_nEfkID, false, XMFLOAT3(
		m_Transform.GetScale().x * SCALE_ENEMY_CREATE_EFK,
		m_Transform.GetScale().y * SCALE_ENEMY_CREATE_EFK,
		m_Transform.GetScale().z * SCALE_ENEMY_CREATE_EFK));
	// **********�A�j���[�V�����p�ݒ�**********
	// �A�j���[�V������������
	m_dAnimTime = 0.0;
	// ���݂̃A�j���[�V�����ԍ�
	m_nAnimStack = m_empEBaseAnimationToInt[IDLE_BA];
	// ���A�j���[�V�����̔ԍ�
	m_nNextAnimStack = m_nAnimStack;
	// **********�A�j���[�V�����p�ݒ肱���܂�**********
	return hr;
}

// �I������
void CEnemy::Fin()
{
	CEnemyManager::DelEnemyCnt();
	CModel::Fin();
}

// �X�V
void CEnemy::Update()
{
	// �������G�t�F�N�g�^�C�}�[�X�V
	if (m_nCreateEffectTimer)
	{
		--m_nCreateEffectTimer;
		CEffekseerManager::SetMatrix(m_nEfkID, false, m_Transform.GetWorld());
		CEffekseerManager::SetScale(m_nEfkID, false, XMFLOAT3(
			m_Transform.GetScale().x * SCALE_ENEMY_CREATE_EFK,
			m_Transform.GetScale().y * SCALE_ENEMY_CREATE_EFK,
			m_Transform.GetScale().z * SCALE_ENEMY_CREATE_EFK));
		if (!m_bVisible && m_nCreateEffectTimer < TIMER_ENEMY_CREATE_EFK * 0.25f)
		{
			m_bVisible = true;
			m_nCreateEffectTimer = 0;
			// ���평����
			m_pWeaponManager->Init(&m_Transform.GetWorld());
			m_pWeaponManager->Create(m_Tag);
		}
	}

	// ���S���̏���
	if (m_nDethEffectTimer) {
		if (m_nDethEffectTimer) {
			--m_nDethEffectTimer;
			if (!m_nDethEffectTimer) {
				Fin();
				delete(this);
				return;
			}
		}
	}

	// ���G���ԍX�V
	if (0 < m_nInvTime)
	{
		--m_nInvTime;
		if (m_nInvTime < 0)
		{
			m_nInvTime = 0;
		}
	}

	
	// �����x���Z�b�g
	m_Transform.SetCurAccel(XMFLOAT3(0, 0, 0));
	
	if (!m_bVisible)return;
		
	// �����C���̂��߃��[���h�s�񂩂���W�����ꎞ�����o���ۑ�
	// ���[���h�ϊ��s��擾
	XMFLOAT4X4 mW = m_Transform.GetWorld();
	// ���f�����W���擾
	XMFLOAT3 vX = XMFLOAT3(mW._11, mW._12, mW._13);
	XMFLOAT3 vY = XMFLOAT3(mW._21, mW._22, mW._23);
	XMFLOAT3 vZ = XMFLOAT3(mW._31, mW._32, mW._33);
	// ���W�擾�A�N���A
	m_Transform.SetPos(XMFLOAT3(mW._41, mW._42, mW._43));
	mW._41 = mW._42 = mW._43 = 0.0f;


	// �͈͓��̃v���C���[�����Ɍ�����ς���
	if (CharaState::HandRangeAttack != GetState() &&
		CharaState::HitBack != GetState() &&
		CharaState::Dying != GetState())
	{
		if (m_pTarget)
		{
			if (BS(m_Transform.GetPos(), m_pTarget->GetTransform()->GetPos(), m_fSeachRad, m_pTarget->GetRadius())) 
			{
				if (m_pTarget->GetVisible()) 
				{
					 //�v���C���[�ւ̃x�N�g�����擾
					XMFLOAT3& vPlayerPos = m_pTarget->GetTransform()->GetPos();
					XMFLOAT3 vPlayer;
					vPlayer.x = vPlayerPos.x - m_Transform.GetPos().x;
					vPlayer.y = vPlayerPos.y - m_Transform.GetPos().y;
					vPlayer.z = vPlayerPos.z - m_Transform.GetPos().z;
					XMVECTOR vecPlayer;
					vecPlayer = XMVector3Normalize(XMLoadFloat3(&vPlayer));

					// �^�[�Q�b�g�����E�ǂ��炩
					XMMATRIX mR = XMMatrixIdentity();

					float fRight = vX.x * vPlayer.x + vX.y * vPlayer.y + vX.z * vPlayer.z;	// ����
					XMStoreFloat(&fRight, XMVector3Dot(XMLoadFloat3(&vX), vecPlayer));

					if (fRight > 0.2f)		// �E
						mR *= XMMatrixRotationAxis(XMLoadFloat3(&vY), XMConvertToRadians(m_Transform.GetRollSpeed()));
					else if (fRight < -0.2f)	// ��
						mR *= XMMatrixRotationAxis(XMLoadFloat3(&vY), XMConvertToRadians(-m_Transform.GetRollSpeed()));

					XMStoreFloat4x4(&mW, XMMatrixMultiply(XMLoadFloat4x4(&mW), mR));

					// �����x�ݒ�
					m_Transform.SetCurAccel(
						XMFLOAT3(
							mW._31 * m_Transform.GetMoveAccel(),
							mW._32 * m_Transform.GetMoveAccel(),
							mW._33 * m_Transform.GetMoveAccel()));

					XMMATRIX worldTestWorld = m_Transform.ExtractRotation(XMLoadFloat4x4(&m_Transform.GetWorld()));

					XMFLOAT4X4 mTestWorld;
					XMStoreFloat4x4(&mTestWorld, worldTestWorld);

					// �A�j���[�V�����ύX
					m_nNextAnimStack = m_empEBaseAnimationToInt[WALK_BA];
					// �X�e�[�g�ύX
					SetState(CharaState::Walk);

					//********�U������******
					if (m_pWeaponManager)
					{
						NearAttack();
					}
				}
				else
				{
					// �X�e�[�g�ύX
					SetState(CharaState::Wait);
					m_nNextAnimStack = m_empEBaseAnimationToInt[IDLE_BA];
				}
			}
			else
			{
				// �X�e�[�g�ύX
				SetState(CharaState::Wait);
				m_nNextAnimStack = m_empEBaseAnimationToInt[IDLE_BA];
			}
		}
		else
		{
			// �X�e�[�g�ύX
			SetState(CharaState::Wait);
			m_nNextAnimStack = m_empEBaseAnimationToInt[IDLE_BA];
		}
	}

	if (!m_bGround)
	{
		// �㉺�����̉����x�X�V
		// �d�͂�������
		m_Transform.SetCurAccel(
			XMFLOAT3(
				m_Transform.GetCurAccel().x,
				m_Transform.GetCurAccel().y + -GRAVITY_SCALE * m_Transform.GetGravityScale(),
				m_Transform.GetCurAccel().z
			));
	}
	

	mW._41 = m_Transform.GetPos().x;
	mW._42 = m_Transform.GetPos().y;
	mW._43 = m_Transform.GetPos().z;
	m_Transform.SetWorld(mW);


	CModel::Update();

	// �o�^�m�[�h�X�V
	Octree* pOctree = CScene::GetOctree();
	pOctree->EraseAndPushObjInNode(&m_NodeIndexWork, this);

	// �����蔻��
	Collision();

	m_pWeaponManager->Update();


	// �̂����菈��
	if (CharaState::HitBack == GetState())
	{
		m_nNextAnimStack = m_empEBaseAnimationToInt[HITBACK_BA];
	}
	// ���S����
	if (m_Status.nHP <= 0 && GetState() != CharaState::Dying)
	{
		// ���C�t0
		m_Status.nHP = 0;
		// �A�j���X�^�b�N�ݒ�
		m_nNextAnimStack = m_empEBaseAnimationToInt[DYING_BA];
		// �X�e�[�g�ύX
		SetState(CharaState::Dying);
		// ����I������
		m_pWeaponManager->Fin();
		// ���S��
		CSound::Play(SE_ENEMY_DIE);
	}

	AnimeUpdate();
}


void CEnemy::Draw()
{
	if (!m_bVisible)return;

	SetShadowRender();
	m_pBBox->SetWorld(&m_Transform.GetWorld());
	m_pBBox->SetColor(XMFLOAT3(1.0f, 0.0f, 0.0f));
	m_pBBox->DepthDraw();
	SetDefaultRender();

	CModel::Draw();
	
}
//********************
// UI�`��
//********************
void CEnemy::DrawUI()
{
	if (!m_bVisible)return;
	ID3D11DeviceContext* pDC = GetDeviceContext();
	SetBlendState(BS_ALPHABLEND);	// �A���t�@�u�����h�L��
	SetZWrite(false);				// Z�o�b�t�@�X�V���Ȃ�

	// ���C�t�o�[
	// ���C�t�g
	// 3D��2D�ϊ�
	// �\���ʒu��OBB�̏�ӂ̈ʒu
	XMFLOAT3 vDrawPos = m_Transform.GetPos();
	vDrawPos.x += m_vCenter.x + m_vBBox.x;
	vDrawPos.y += m_vCenter.y + m_vBBox.y;
	vDrawPos.z += m_vCenter.z + m_vBBox.z;

	CCamera* pCamera = m_pScene->GetCameraManager()->GetCamera();
	XMMATRIX mProj = XMLoadFloat4x4(&pCamera->GetProjMatrix());
	XMMATRIX mView = XMLoadFloat4x4(&pCamera->GetViewMatrix());
	XMMATRIX mWorld = XMMatrixIdentity();
	XMVECTOR vecPos = XMVector3Project(
		XMLoadFloat3(&vDrawPos),
		0.0f, 0.0f,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		0.0f, 1.0f,
		mProj,
		mView,
		mWorld);

	XMFLOAT3 vScreenPos;
	XMStoreFloat3(&vScreenPos, vecPos);
	vScreenPos.x -= SCREEN_CENTER_X;
	vScreenPos.y -= SCREEN_CENTER_Y;

	if (vScreenPos.z <= 1.0f)
	{
		// ���C�t�g
		CPolygon::SetTexture(CTextureData::GetTextureResource(GAGE_WAKU)/*m_pTextureUI[UITEX_GAGE_WAKU]*/);
		CPolygon::SetPos(vScreenPos.x + POS_LIFE_OFFSET.x, -vScreenPos.y + POS_LIFE_OFFSET.y);
		CPolygon::SetSize(SIZE_LIFEWAKU.x, SIZE_LIFEWAKU.y);
		CPolygon::SetUV(1.0f, 1.0f);
		CPolygon::SetColor(1.0f, 1.0f, 1.0f);
		CPolygon::SetAlpha(1.0f);
		CPolygon::Draw(pDC);


		// ���C�t�w�i
		CPolygon::SetTexture(CTextureData::GetTextureResource(GAGE_HAIKEI)/*m_pTextureUI[UITEX_GAGE_HAIKEI]*/);
		CPolygon::SetPos(vScreenPos.x + POS_LIFE_OFFSET.x, -vScreenPos.y + POS_LIFE_OFFSET.y);
		CPolygon::SetSize(LIFESIZEMAX.x, LIFESIZEMAX.y);
		CPolygon::SetColor(1.0f, 1.0f, 1.0f);
		CPolygon::SetAlpha(1.0f);
		CPolygon::Draw(pDC);

		// �c�胉�C�t
		float fRateWork; // ���C�t�ƕ��킻�ꂼ��̌��ݒl/�ő�l����
		fRateWork = (float)m_Status.nHP / (float)m_Status.nMaxHP;
		if (m_Status.nHP < m_Status.nMaxHP / 3)
		{
			CPolygon::SetTexture(CTextureData::GetTextureResource(LIFEGAGE_RED));
		}
		else
		{
			CPolygon::SetTexture(CTextureData::GetTextureResource(LIFEGAGE_GREEN));
		}
		CPolygon::SetPos((vScreenPos.x + POS_LIFE_OFFSET.x) - (LIFESIZEMAX.x / 2.0f) * (1.0f - fRateWork), -vScreenPos.y + POS_LIFE_OFFSET.y);
		CPolygon::SetSize(LIFESIZEMAX.x * fRateWork, LIFESIZEMAX.y);
		CPolygon::SetUV(1.0f/*SIZE_LIFEWAKU.x * fRateWork*/, 1.0f);
		CPolygon::SetColor(1.0f, 1.0f, 1.0f);
		CPolygon::SetAlpha(1.0f);
		CPolygon::Draw(pDC);
	}

	CPolygon::SetUV(1.0f, 1.0f);
	CPolygon::SetColor(0.0f, 0.0f, 0.0f);

	SetZWrite(true);							// Z�o�b�t�@�X�V����
	SetBlendState(BS_NONE);		                // �A���t�@�u�����h����
}

void CEnemy::NearAttack()  // �ߋ����U��
{
	// �U���͈͓��Ƀv���C���[�����邩
	if (BS(m_Transform.GetPos(), m_pTarget->GetTransform()->GetPos(), m_fAtkRad, m_pTarget->GetRadius())) {

		// �v���C���[�ւ̃x�N�g�����擾
		XMFLOAT3& vPlayerPos = m_pTarget->GetTransform()->GetPos();
		XMFLOAT3 vPlayer;
		vPlayer.x = vPlayerPos.x - m_Transform.GetPos().x;
		vPlayer.y = vPlayerPos.y - m_Transform.GetPos().y;
		vPlayer.z = vPlayerPos.z - m_Transform.GetPos().z;
		// �O�����x�N�g��
		XMFLOAT4X4 mW = m_Transform.GetWorld();
		XMFLOAT3 vZ = XMFLOAT3(mW._31, mW._32, mW._33);
		// �����̍U���ˊp�͈͓̔��ɂ���ƍU��
		float fAttackAngle;
		XMStoreFloat(&fAttackAngle, XMVector3AngleBetweenNormals(XMVector3Normalize(XMLoadFloat3(&vZ)), XMVector3Normalize(XMLoadFloat3(&vPlayer))));
		if (fAttackAngle < XMConvertToRadians(45.0f))
		{
			// �U���J�n
			m_pWeaponManager->Fire((int)(m_Status.nATK * TECRATE_DEFAULT), ALLMOTION_ATTACK, START_TIME_ATTACK);
			// �X�e�[�g�ύX
			SetState(CharaState::HandRangeAttack);
			// �A�j���[�V�����ύX
			m_nNextAnimStack = ANIM_ATTACK;
		}
	}
}


// �o�E���f�B�{�b�N�X
bool CEnemy::BS(XMFLOAT3 vA, XMFLOAT3 vB, float RadA, float RadB)
{
	// �Փ˔���
	float dx = vA.x - vB.x;
	float dy = vA.y - vB.y;
	float dz = vA.z - vB.z;
	float dr = RadA + RadB;
	return dx * dx + dy * dy + dz * dz <= dr * dr;
}
