// �G���@�g�� [Enemy_Wizard.cpp]
#include "Enemy_Wizard.h"
#include "Sound.h"
#include "EffekseerManager.h"
#include "MGManager.h"
#include "SwordAttackManager.h"
#include "DebugProc.h"
// �萔
namespace {
	// ********�X�e�[�^�X********
	const int STATUS_MAX_LIFE = 1000;   // �ő僉�C�t
	const int STATUS_ATK = 100;			// �����U����
	const int STATUS_DEF = 50;			// �����h���
	const int STATUS_INT = 300;			// ���@�U����
	const int STATUS_RES = 300;			// ���@�h���

	// *********�ړ��p�ϐ�*********
	const float MOVE_ACCEL = 0.5f;      // �����x
	const float VEL_MAX = 1.0f;         // �ō����x
	const float ROLLSPEED = 12.0f;      // �U��������x�@�P��:�x
	const float SEACH_RAD = 5000.0f;    // ���G�͈�
	
	// *********�����蔻��*********
	const float COL_SPHERE_RAD = 50.0f;
	const XMFLOAT3 COL_BBOX = XMFLOAT3(50.0f, 120.0f, 60.0f);

	// *********�U���p�萔*********
	const float ATTACK_BS_RAD = 3000.0f;    // �U���͈�
	const int ALLMOTION_ATTACK = 30;        // ���U�����[�V��������
	const int START_TIME_ATTACK = 18;       // �U������o������(�����J�n�ォ��̌o�ߎ���)
	const float TECRATE_NAILATK = 1.3f;     // �Z���Ƃ̃_���[�W�{��
	const float TECRATE_TRIPLEMAGIC = 1.2f; // �Z���Ƃ̃_���[�W�{��

	// *********�A�j���[�V�����p�萔*********
	const double ANIM_SPEED = 0.015;    // �A�j���[�V�����Đ����x
	const int ANIM_IDLE     = 0;        // �ҋ@
	const int ANIM_WALK     = 1;        // ���s
	const int ANIM_ATTACK   = 2;        // �U��
	const int ANIM_DYING    = 3;        // ��
	const int ANIM_SERCH    = 4;        // ���G
	const int ANIM_HITBACK  = 5;        // �q�b�g�o�b�N
}

// �R���X�g���N�^
CEnemy_Wizard::CEnemy_Wizard(CScene* pScene) : CEnemy(pScene)
{
	m_pWeaponManager = nullptr;
	SetObjID(EObjID::OI_ENEMY_WIZARD);
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
CEnemy_Wizard::~CEnemy_Wizard()
{
}

// ������
HRESULT CEnemy_Wizard::Init()
{
	HRESULT hr = CEnemy::Init();
	// �U���J�E���g
	m_nAttackCnt = 0;
	// �X�e�[�^�X
	m_Status.nMaxHP = STATUS_MAX_LIFE;
	m_Status.nHP = m_Status.nMaxHP;
	m_Status.nATK = STATUS_ATK;
	m_Status.nDEF = STATUS_DEF;
	m_Status.nINT = STATUS_INT;
	m_Status.nRES = STATUS_RES;
	// ���f���ݒ�
	SetModel(MT_WIZARD);
	m_fRadius = 50.0f;
	m_vBBox = XMFLOAT3(50.0f, m_vCenter.y, 60.0f);
	m_vCenter.x = 0;
	m_vCenter.z = 0;
	// ���폈��
	if (m_pWeaponManager) {
		m_pWeaponManager->AllDelete();
		delete m_pWeaponManager;
	}
	// ����ݒ�
	m_pWeaponManager = new CMGManager(m_pScene);
	m_pWeaponManager->SetParent(this);

	return hr;
}

// �I������
void CEnemy_Wizard::Fin()
{
	SAFE_DELETE(m_pWeaponManager);
	CEnemy::Fin();
}

// �X�V
void CEnemy_Wizard::Update()
{
	CEnemy::Update();
	//// �̂����菈��
	//if (CharaState::HitBack == m_State)
	//{
	//	m_nNextAnimStack = ANIM_HITBACK;

	//	// ���S����
	//	if (m_Status.nHP <= 0)
	//	{
	//		// ���C�t0
	//		m_Status.nHP = 0;
	//		// �A�j���X�^�b�N�ݒ�
	//		m_nNextAnimStack = ANIM_DYING;
	//		// �X�e�[�g�ύX
	//		SetState(CharaState::Dying);
	//		// ����I������
	//		m_pWeaponManager->Fin();
	//		// ���S��
	//		CSound::Play(SE_ENEMY_DIE);
	//	}
	//}
	//

	//do {
	//	if (!m_bVisible)
	//		break;

	//	if (Dying == m_State)
	//		break;

	//	if (CharaState::HitBack == m_State)
	//		break;

	//	// ���[���h�ϊ��s��擾
	//	XMFLOAT4X4 mW = m_Transform.GetWorld();
	//	// ���f�����W���擾
	//	XMFLOAT3 vX = XMFLOAT3(mW._11, mW._12, mW._13);
	//	XMFLOAT3 vY = XMFLOAT3(mW._21, mW._22, mW._23);
	//	XMFLOAT3 vZ = XMFLOAT3(mW._31, mW._32, mW._33);
	//	// ���W�擾�A�N���A
	//	m_Transform.SetPos(XMFLOAT3(mW._41, mW._42, mW._43));
	//	mW._41 = mW._42 = mW._43 = 0.0f;

	//	m_Transform.SetVel(XMFLOAT3(0, 0, 0));

	//	if (CharaState::HandRangeAttack != m_State)
	//	{
	//		if (m_pTarget) {
	//			// �ǔ��͈͓��Ƀv���C���[�����邩
	//			if (BS(m_Transform.GetPos(), m_pTarget->GetTransform()->GetPos(), SERCH_BS_RAD, m_pTarget->GetRadius())) {
	//				// �v���C���[�͉���Ԃ�
	//				if (m_pTarget->GetVisible()) {
	//					// �v���C���[�ւ̃x�N�g�����擾
	//					XMFLOAT3& vPlayerPos = m_pTarget->GetTransform()->GetPos();
	//					XMFLOAT3 vPlayer;
	//					vPlayer.x = vPlayerPos.x - m_Transform.GetPos().x;
	//					vPlayer.y = vPlayerPos.y - m_Transform.GetPos().y;
	//					vPlayer.z = vPlayerPos.z - m_Transform.GetPos().z;
	//					XMVECTOR vecPlayer;
	//					vecPlayer = XMVector3Normalize(XMLoadFloat3(&vPlayer));

	//					// �^�[�Q�b�g�����E�ǂ��炩
	//					XMMATRIX mR = XMMatrixIdentity();

	//					float fRight = vX.x * vPlayer.x + vX.y * vPlayer.y + vX.z * vPlayer.z;	// ����
	//					XMStoreFloat(&fRight, XMVector3Dot(XMLoadFloat3(&vX), vecPlayer));

	//					if (fRight > 0.0f)		// �E
	//						mR *= XMMatrixRotationAxis(XMLoadFloat3(&vY), XMConvertToRadians(ROLLSPEED));
	//					else if (fRight < 0.0f)	// ��
	//						mR *= XMMatrixRotationAxis(XMLoadFloat3(&vY), XMConvertToRadians(-ROLLSPEED));

	//					XMStoreFloat4x4(&mW, XMMatrixMultiply(XMLoadFloat4x4(&mW), mR));


	//					//********�ړ�����******
	//						// �����Ă�������Ɉړ�		
	//					m_Transform.SetVel(
	//						XMFLOAT3(
	//							m_Transform.GetVel().x + mW._31 * MOVE_SPEED,
	//							m_Transform.GetVel().y + mW._32 * MOVE_SPEED,
	//							m_Transform.GetVel().z + mW._33 * MOVE_SPEED
	//						));

	//					// �A�j���[�V�����ύX
	//					m_nNextAnimStack = ANIM_WALK;
	//					// �X�e�[�g�ύX
	//					SetState(CharaState::Walk);

	//					//********�U������******
	//					// �U���͈͓��Ƀv���C���[�����邩
	//					if (BS(m_Transform.GetPos(), m_pTarget->GetTransform()->GetPos(), ATTACK_BS_RAD, m_pTarget->GetRadius())) {

	//						// �����̍U���ˊp�͈͓̔��ɂ���ƍU��
	//						float fAttackAngle;

	//						XMStoreFloat(&fAttackAngle, XMVector3AngleBetweenNormals(XMVector3Normalize(XMLoadFloat3(&vZ)), XMVector3Normalize(XMLoadFloat3(&vPlayer))));
	//						if (fAttackAngle < XMConvertToRadians(5.0f))
	//						{
	//							
	//							// �X�e�[�g�ύX
	//							SetState(CharaState::HandRangeAttack);
	//							// �A�j���[�V�����ύX
	//							m_nNextAnimStack = ANIM_ATTACK;
	//						}
	//					}
	//				}
	//				else
	//				{
	//					// �X�e�[�g�ύX
	//					SetState(CharaState::Wait);
	//					m_nNextAnimStack = ANIM_SERCH;
	//				}
	//			}
	//			else
	//			{
	//				// �X�e�[�g�ύX
	//				SetState(CharaState::Wait);
	//				m_nNextAnimStack = ANIM_SERCH;
	//			}
	//		}
	//		else
	//		{
	//		    // �X�e�[�g�ύX
	//		    SetState(CharaState::Wait);
	//		    m_nNextAnimStack = ANIM_SERCH;
	//		}
	//	}
	//	
	//	
	//	if (!m_bGround)
	//	{
	//		// �㉺�����̉����x�X�V
	//		// �d�͂�������
	//		m_Transform.SetCurAccel(
	//			XMFLOAT3(
	//				m_Transform.GetCurAccel().x + -GRAVITY_SCALE,
	//				m_Transform.GetCurAccel().y + -GRAVITY_SCALE,
	//				m_Transform.GetCurAccel().z + -GRAVITY_SCALE
	//			));
	//	}
	//	
	//	// �ړ��x�N�g���ɉ��Z
	//	m_Transform.SetVel(
	//		XMFLOAT3(
	//			m_Transform.GetVel().x,
	//			m_Transform.GetVel().y + m_Transform.GetCurAccel().y,
	//			m_Transform.GetVel().z
	//		));
	//	if (m_Transform.GetVel().y < -LIMIT_VEL)
	//	{
	//		m_Transform.SetVel(
	//			XMFLOAT3(
	//				m_Transform.GetVel().x,
	//				m_Transform.GetVel().y + -LIMIT_VEL,
	//				m_Transform.GetVel().z
	//			));
	//	}
	//	// �ړ��x�N�g�����Z
	//	m_Transform.SetPos(
	//		XMFLOAT3(
	//			m_Transform.GetPos().x + m_Transform.GetVel().x,
	//			m_Transform.GetPos().y + m_Transform.GetVel().y,
	//			m_Transform.GetPos().z + m_Transform.GetVel().z
	//		));

	//	// ���[���h�ϊ��s��ɔ��f
	//	mW._41 = m_Transform.GetPos().x;
	//	mW._42 = m_Transform.GetPos().y;
	//	mW._43 = m_Transform.GetPos().z;
	//	m_Transform.SetWorld(mW);

	//	// �o�^�m�[�h�X�V
	//	Octree* pOctree = CScene::GetOctree();
	//	pOctree->EraseAndPushObjInNode(&m_NodeIndexWork, this);
	//	
	//	// �����蔻��
	//	Collision();

	//	m_pWeaponManager->Update();



	//	// ���S����
	//	if (m_Status.nHP <= 0)
	//	{
	//		// ���C�t0
	//		m_Status.nHP = 0;
	//		// �A�j���X�^�b�N�ݒ�
	//		m_nNextAnimStack = ANIM_DYING;
	//		// �X�e�[�g�ύX
	//		SetState(CharaState::Dying);
	//		// ����I������
	//		m_pWeaponManager->Fin();
	//		// ���S��
	//		CSound::Play(SE_ENEMY_DIE);
	//	}



	//} while (0);


	//// �A�j���[�V�����X�V
	//AnimeUpdate();
}


void CEnemy_Wizard::Draw()
{
	// �A�j���[�V�����ݒ�
	SetAnimIndex(m_nAnimStack);
	SetAnimTime(m_dAnimTime);

	CEnemy::Draw();
}

void CEnemy_Wizard::NearAttack()
{
	// �U���͈͓��Ƀv���C���[�����邩
	if (BS(m_Transform.GetPos(), m_pTarget->GetTransform()->GetPos(), ATTACK_BS_RAD, m_pTarget->GetRadius())) 
	{
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
		if (fAttackAngle < XMConvertToRadians(5.0f))
		{
			// �X�e�[�g�ύX
			SetState(CharaState::HandRangeAttack);
			// �A�j���[�V�����ύX
			m_nNextAnimStack = ANIM_ATTACK;
		}
	}
}

void CEnemy_Wizard::AnimeUpdate()
{
	

	// �A�j���[�V�����ԍ��X�V
	switch (m_nAnimStack)
	{
	case ANIM_IDLE:
	case ANIM_SERCH:
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

	// �A�j���[�V�������Ԓ��ɍs������
	switch (m_nAnimStack)
	{
	case ANIM_ATTACK:
		m_nAttackCnt++;
		if (m_nAttackCnt == 25 )
		{
			// �U���J�n
			m_pWeaponManager->Fire((int)(m_Status.nINT * TECRATE_TRIPLEMAGIC));
		}
		if (m_nAttackCnt == 30)
		{
			// �U���J�n
			m_pWeaponManager->Fire((int)(m_Status.nINT * TECRATE_TRIPLEMAGIC));
		}
		if (m_nAttackCnt == 35)
		{
			// �U���J�n
			m_pWeaponManager->Fire((int)(m_Status.nINT * TECRATE_TRIPLEMAGIC));
		}
		break;
	default:
		break;
	}
	
	// �A�j���[�V�����J�n���Ă���̎��Ԃ����A�j���[�V�������Ԃ𒴂��Ă������̏���
	//double Double = GetAnimDuration(m_nAnimStack);
	if (m_dAnimTime >= GetAnimDuration(m_nAnimStack))
	{
		switch (m_nAnimStack) {
		case ANIM_IDLE:
		case ANIM_SERCH:
		case ANIM_WALK:
			// �A�j���[�V�����ԍ���ҋ@�A�j���[�V�����ԍ���
			m_dAnimTime = 0.0;
			break;
		case ANIM_ATTACK:
		case ANIM_HITBACK:
			// �A�j���[�V�����ԍ���ҋ@�A�j���[�V�����ԍ���
			m_dAnimTime = 0.0;
			// �čU���\��
			m_pWeaponManager->Create(GOT_ENEMY);
			m_nAttackCnt = 0;
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

