// �}���A�i�{�X�j[Enemy_Maria.cpp]
#include "Enemy_Maria.h"
#include "Sound.h"
#include "EffekseerManager.h"
#include "Explosion.h"
#include "MGManager.h"
#include "SwordAttackManager.h"
#include "DebugProc.h"
// �萔
namespace {
	// ********�X�e�[�^�X********
	const int STATUS_MAX_LIFE = 10000;  // �ő僉�C�t
	const int STATUS_ATK = 1000;		// �����U����
	const int STATUS_DEF = 400;			// �����h���
	const int STATUS_INT = 200;			// ���@�U����
	const int STATUS_RES = 100;			// ���@�h���

	// *********�ړ��p�ϐ�*********
	const float MOVE_ACCEL = 0.2f;      // �����x
	const float VEL_MAX = 2.5f;         // �ō����x
	const float ROLLSPEED = 2.0f;       // �U��������x�@�P��:�x
	const float SEACH_RAD = 5000.0f;    // ���G�͈�

	// *********�����蔻��*********
	const float COL_SPHERE_RAD = 50.0f;
	const XMFLOAT3 COL_BBOX = XMFLOAT3(50.0f, 120.0f, 60.0f);

	// *********�U���p�萔*********
	const float ATTACK_BS_RAD = 20.0f;          // �U���͈�
	const int ALLMOTION_HIGHSPINATTACK = 60;    // ���U�����[�V��������
	const int ALLMOTION_GRATESWORDSLASH = 190;  // ���U�����[�V��������
	const int START_TIME_HIGHSPINATTACK = 10;   // �U������o������(�����J�n�ォ��̌o�ߎ���)
	const int START_TIME_GRATESWORDSLASH = 90;  // �U������o������(�����J�n�ォ��̌o�ߎ���)
	const float TECRATE_HIGHSPINATTACK = 0.7f;  // �Z���Ƃ̃_���[�W�{��
	const float TECRATE_GRATESWORDSLASH = 0.9f;	// �Z���Ƃ̃_���[�W�{��


	// *********�A�j���[�V�����p�萔*********
	const double ANIM_SPEED = 0.015;       // �A�j���[�V�����Đ����x
	// �A�j���[�V�����̎��
	const int ANIM_IDLE            = 0;    // �ҋ@
	const int ANIM_SERCH           = 1;    // ���G
	const int ANIM_RUN             = 2;    // ����
	const int ANIM_HIGHSPINATTACK  = 3;    // �n�C�X���b�V��
	const int ANIM_GRATESWORDSLASH = 4;    // �O���[�g�X���b�V��
	const int ANIM_DYING           = 5;    // ��
	const int ANIM_HITBACK         = 6;    // �q�b�g�o�b�N

	// *********���̑��萔***********
	// ���񂾂Ƃ��̎��Ԃ��������ɂ��鎞�ԁH�i�J�E���g�j
	const int TIME_SLOW_CNT = 30;
}

// �R���X�g���N�^
CEnemy_Maria::CEnemy_Maria(CScene* pScene) : CEnemy(pScene)
{
	m_pWeaponManager = nullptr;
	SetTag(GOT_BOSS);
	SetObjID(EObjID::OI_ENEMY_MARIA);
	m_Transform.SetMoveAccel(MOVE_ACCEL);
	m_Transform.SetVelMax(VEL_MAX);
	m_Transform.SetRollSpeed(ROLLSPEED);
	m_fAtkRad = ATTACK_BS_RAD;
	m_fSeachRad = SEACH_RAD;
	m_empEBaseAnimationToInt[IDLE_BA] = ANIM_IDLE;
	m_empEBaseAnimationToInt[WALK_BA] = ANIM_RUN;
	m_empEBaseAnimationToInt[DYING_BA] = ANIM_DYING;
	m_empEBaseAnimationToInt[HITBACK_BA] = ANIM_HITBACK;
}

// �f�X�g���N�^
CEnemy_Maria::~CEnemy_Maria()
{
}

// ������
HRESULT CEnemy_Maria::Init()
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
	SetModel(MT_MARIA);
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
void CEnemy_Maria::Fin()
{
	SAFE_DELETE(m_pWeaponManager);
	CEnemy::Fin();
}

// �X�V
void CEnemy_Maria::Update()
{
	CEnemy::Update();
}


void CEnemy_Maria::Draw()
{
	// �A�j���[�V�����ݒ�
	SetAnimIndex(m_nAnimStack);
	SetAnimTime(m_dAnimTime);

	CEnemy::Draw();
}

void CEnemy_Maria::NearAttack()
{
	// �U���͈͓��Ƀv���C���[�����邩
	if (BS(m_Transform.GetPos(), m_pTarget->GetTransform()->GetPos(), ATTACK_BS_RAD, m_pTarget->GetRadius())) {
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
			m_pWeaponManager->Fire((int)(m_Status.nATK * TECRATE_GRATESWORDSLASH), ALLMOTION_GRATESWORDSLASH, START_TIME_GRATESWORDSLASH);
			// �X�e�[�g�ύX
			SetState(CharaState::HandRangeAttack);
			// �A�j���[�V�����ύX
			m_nNextAnimStack = ANIM_GRATESWORDSLASH;
		}
		else
		{
			// �U���J�n
			m_pWeaponManager->Fire((int)(m_Status.nATK * TECRATE_HIGHSPINATTACK), ALLMOTION_HIGHSPINATTACK, START_TIME_HIGHSPINATTACK);
			// �X�e�[�g�ύX
			SetState(CharaState::HandRangeAttack);
			// �A�j���[�V�����ύX
			m_nNextAnimStack = ANIM_HIGHSPINATTACK;
		}
	}
}

void CEnemy_Maria::AnimeUpdate()
{


	// �A�j���[�V�����ԍ��X�V
	switch (m_nAnimStack)
	{
	case ANIM_IDLE:
	case ANIM_SERCH:
	case ANIM_RUN:
	case ANIM_DYING:
	case ANIM_HIGHSPINATTACK:
	case ANIM_GRATESWORDSLASH:
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
	case ANIM_HIGHSPINATTACK:
	case ANIM_GRATESWORDSLASH:
	case ANIM_DYING:
		m_dAnimTime += ANIM_SPEED * 1.5;
		break;
	case ANIM_HITBACK:
		m_dAnimTime += ANIM_SPEED * 2.0;
		break;
	default:
		m_dAnimTime += ANIM_SPEED;
		break;
	}

	// �A�j���[�V�����J�n���Ă���̎��Ԃ����A�j���[�V�������Ԃ𒴂��Ă������̏���
	if (m_dAnimTime >= GetAnimDuration(m_nAnimStack))
	{
		switch (m_nAnimStack) {
		case ANIM_IDLE:
		case ANIM_SERCH:
		case ANIM_RUN:
			// �A�j���[�V�����ԍ����J��Ԃ�
			m_dAnimTime = 0.0;
			break;
		case ANIM_HIGHSPINATTACK:
		case ANIM_GRATESWORDSLASH:
		case ANIM_HITBACK:
			// �A�j���[�V�����ԍ���ҋ@�A�j���[�V�����ԍ���
			m_dAnimTime = 0.0;
			// �čU���\��
			m_pWeaponManager->Create(GOT_BOSS);
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

