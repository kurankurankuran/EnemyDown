//�G�iЭ���āj [Enemy_Mutant.cpp]
#include "Enemy_Mutant.h"
#include "Sound.h"
#include "EffekseerManager.h"
#include "Explosion.h"
#include "MGManager.h"
#include "SwordAttackManager.h"
#include "DebugProc.h"
// �萔
namespace {
	
	// ********�X�e�[�^�X********
	const int STATUS_MAX_LIFE = 2500;   // �ő僉�C�t
	const int STATUS_ATK = 500;         // �����U����
	const int STATUS_DEF = 500;         // �����h���
	const int STATUS_INT =  50;         // ���@�U����
	const int STATUS_RES =  25;         // ���@�h���

	// *********�ړ��p�ϐ�*********
	const float MOVE_ACCEL = 0.2f;      // �����x
	const float VEL_MAX = 2.5f;         // �ō����x
	const float ROLLSPEED = 0.5f;       // �U��������x�@�P��:�x
	const float SEACH_RAD = 5000.0f;    // ���G�͈�

	// *********�����蔻��*********
	const float COL_SPHERE_RAD = 50.0f;
	const XMFLOAT3 COL_BBOX = XMFLOAT3(50.0f, 120.0f, 60.0f);

	// *********�U���p�萔*********
	const float ATTACK_BS_RAD = 20.0f;  // �U���͈�
	const int ALLMOTION_ATTACK = 30;    // ���U�����[�V��������
	const int START_TIME_ATTACK = 18;   // �U������o������(�����J�n�ォ��̌o�ߎ���)
	const float TECRATE_NAILATK = 1.3f; // �Z���Ƃ̃_���[�W�{��

	// *********�A�j���[�V�����p�萔*********
	const double ANIM_SPEED = 0.015;    // �A�j���[�V�����Đ����x
	// �A�j���[�V�����̎��
	const int ANIM_IDLE       = 0;      // �ҋ@
	const int ANIM_WALK       = 1;      // ���s
	const int ANIM_DYING      = 2;      // ��
	const int ANIM_WAIT_HIMA  = 3;      // �ҋ@�Q
	const int ANIM_ATTACK     = 4;      // �U��
	const int ANIM_HITBACK    = 5;      // ��_���[�W
}

// �R���X�g���N�^
CEnemy_Mutant::CEnemy_Mutant(CScene* pScene) : CEnemy(pScene)
{
	m_pWeaponManager = nullptr;
	SetObjID(EObjID::OI_ENEMY_MUTANT);
	m_Transform.SetMoveAccel(MOVE_ACCEL);
	m_Transform.SetVelMax(VEL_MAX);
	m_Transform.SetRollSpeed(ROLLSPEED);
	m_fAtkRad = ATTACK_BS_RAD;
	m_fSeachRad = SEACH_RAD;
	m_empEBaseAnimationToInt[IDLE_BA] = ANIM_IDLE;
	m_empEBaseAnimationToInt[WALK_BA] = ANIM_WALK;
	m_empEBaseAnimationToInt[DYING_BA] = ANIM_DYING;
	m_empEBaseAnimationToInt[HITBACK_BA] = ANIM_HITBACK;

	
}

// �f�X�g���N�^
CEnemy_Mutant::~CEnemy_Mutant()
{
}

// ������
HRESULT CEnemy_Mutant::Init()
{
	HRESULT hr = CEnemy::Init();
	// �X�e�[�^�X
	m_Status.nMaxHP = STATUS_MAX_LIFE;
	m_Status.nHP = m_Status.nMaxHP;
	m_Status.nATK = STATUS_ATK;
	m_Status.nDEF = STATUS_DEF;
	m_Status.nINT = STATUS_INT;
	m_Status.nRES = STATUS_RES;
	// ���f���ݒ�
	SetModel(MT_MUTANT);
	m_fRadius = COL_SPHERE_RAD;
	m_vBBox = COL_BBOX;
	m_vCenter.x = 0;
	m_vCenter.z = 0;
	// ���폈��
	if (m_pWeaponManager) {
		m_pWeaponManager->AllDelete();
		delete m_pWeaponManager;
	}
	// ��������
	m_pWeaponManager = new CSwordAttackManager(m_pScene);
	m_pWeaponManager->SetParent(this);


	return hr;
}

// �I������
void CEnemy_Mutant::Fin()
{
	SAFE_DELETE(m_pWeaponManager);
	CEnemy::Fin();
}

// �X�V
void CEnemy_Mutant::Update()
{
	CEnemy::Update();	
}


void CEnemy_Mutant::Draw()
{
	// �A�j���[�V�����ݒ�
	SetAnimIndex(m_nAnimStack);
	SetAnimTime(m_dAnimTime);

	CEnemy::Draw();
}

void CEnemy_Mutant::NearAttack()
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
			m_pWeaponManager->Fire((int)(m_Status.nATK * TECRATE_NAILATK), ALLMOTION_ATTACK, START_TIME_ATTACK);
			// �X�e�[�g�ύX
			SetState(CharaState::HandRangeAttack);
			// �A�j���[�V�����ύX
			m_nNextAnimStack = ANIM_ATTACK;
		}
	}
}


void CEnemy_Mutant::AnimeUpdate()
{
	

	// �A�j���[�V�����ԍ��X�V
	switch (m_nAnimStack)
	{
	case ANIM_WAIT_HIMA:
	case ANIM_IDLE:
	case ANIM_WALK:
	case ANIM_DYING:
	case ANIM_ATTACK:
	case ANIM_HITBACK:
		if (m_nAnimStack != m_nNextAnimStack) {
			m_nAnimStack = m_nNextAnimStack;
			m_dAnimTime = 0.0;
		}
		break;
	default:
		break;
	}

	// �A�j���[�V�������ԍX�V
	switch (m_nAnimStack)
	{
	case ANIM_DYING:
		m_dAnimTime += ANIM_SPEED * 1.5;
		break;
	default:
		m_dAnimTime += ANIM_SPEED;
		break;
	}
	
	// �A�j���[�V�����I�����̏���
	if (m_dAnimTime >= GetAnimDuration(m_nAnimStack))
	{
		switch (m_nAnimStack) {
		case ANIM_WALK:
		case ANIM_WAIT_HIMA:
		case ANIM_IDLE:
			// �A�j���[�V�����ԍ����J��Ԃ�
			m_dAnimTime = 0.0;
			break;
		case ANIM_ATTACK:
		case ANIM_HITBACK:
			// �A�j���[�V�����ԍ���ҋ@�A�j���[�V�����ԍ���
			m_dAnimTime = 0.0;
			// �čU���\��
			m_pWeaponManager->Create(GOT_ENEMY);
			// �X�e�[�g�ύX
			SetState(CharaState::Wait);
			break;
		case ANIM_DYING:
			// ���̌�̔����ł����s���ɂȂ�
			if (m_bVisible) {
				// �s��
				m_bVisible = false;
				// �G�t�F�N�V�A
				StartDeath();
			}
		}

	}

	
}

