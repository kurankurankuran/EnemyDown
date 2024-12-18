// �v���C���[ [Player.cpp]
#include "Player.h"
#include "Input.h"
#include "DebugProc.h"
#include "Scene.h"
#include "MGManager.h"
#include "SwordAttackManager.h"
#include "Land.h"
#include "Sound.h"
#include "Fade.h"
#include "Polygon.h"
#include "Texture.h"
#include "EffekseerManager.h"
#include "Text.h"
#include "TextureData.h"

//**************************************************************************************************************
// �萔
//**************************************************************************************************************
namespace {
	// ********�X�e�[�^�X********
	const int STATUS_MAX_LIFE = 1000;
	const int STATUS_ATK = 1000;
	const int STATUS_DEF =  500;
	const int STATUS_INT =  250;
	const int STATUS_RES =  250;

	//  *********���C�t�p�萔*********
	// ���C�t�Q�[�W�̘g�\���T�C�Y
	const XMFLOAT2 SIZE_LIFEWAKU = XMFLOAT2(300.0f, 20.0f);
	// ���C�t�Q�[�W�̘g�\���ʒu(��ʍ��ォ��)
	const XMFLOAT2 POS_LIFEWAKU_OFFSET = XMFLOAT2(50.0f, 40.0f);
	// ���C�t�Q�[�W�̘g�ƕ���Q�[�W�̘g�̊Ԋu
	const float LIFETOWEAPONDISTANCE = 40.0f;
	// �g�̌���
	const XMFLOAT2 WAKUTHICKNESS = XMFLOAT2(10.0f, 5.0f);
	// �c��Q�[�W�̉��Əc�̑傫��
	const XMFLOAT2 LIFESIZEMAX = XMFLOAT2(SIZE_LIFEWAKU.x - WAKUTHICKNESS.x, SIZE_LIFEWAKU.y - WAKUTHICKNESS.y);
	
	
	//  *********�ړ��p�萔*********
	// �ړ����x���E
	const float LIMIT_VEL = 50.0f;
	// �������x
	const float WALKSPEED = 20.0f;
	// ���鑬�x
	const float DASHSPEED = 40.0f;
	// �_�b�V���\��
	const int DASH_NUM = 1;
	// �_�b�V���ړ���
	const float DASHVOL = 50.0f;
	// �_�b�V���ړ��ʌ����W��
	const float DASHVOLDEL = 0.95f;
	// �_�b�V���ړ��ʌ������E
	const float DASHVOLDELLIMIT = 10.0f;
	// �W�����v��
	const float JAUMPPOWER = 55.0f;

	// *********�A�j���[�V�����p�萔*********
	// �A�j���[�V�����Đ����x
	const double ANIM_SPEED = 0.03;
	// �A�j���[�V�����̎��
	const int ANIM_IDLE = 0;
	const int ANIM_WALK = 1;
	const int ANIM_RUN = 2;
	const int ANIM_JUMP = 3;
	const int ANIM_SLASH = 4;
	const int ANIM_HITBACK= 5;
	const int ANIM_DYING = 6;
	
	// *********���̑��萔***********
	// ���񂾂Ƃ��̎��Ԃ��������ɂ��鎞�ԁH�i�J�E���g�j
	const int TIME_SLOW_CNT = 30;

	// �e�L�X�g�̃T�C�Y
	const XMFLOAT2 TEXT_SIZE = XMFLOAT2(20.0f, 40.0f);
	// ���C�t�Q�[�W�ʒu�v�Z�p
	const XMFLOAT2 POS_LIFEWAKU_CALCULAT = XMFLOAT2(-SCREEN_CENTER_X + SIZE_LIFEWAKU.x / 2 + POS_LIFEWAKU_OFFSET.x/*��ʍ��[����̋���*/, SCREEN_CENTER_Y - POS_LIFEWAKU_OFFSET.y);
	const std::string TEXT_HP = std::string("HP");
	const XMFLOAT2 TEXT_HP_POS = XMFLOAT2(-SCREEN_WIDTH / 2 + TEXT_SIZE.x, POS_LIFEWAKU_CALCULAT.y + 5);
	const XMFLOAT2 TEXT_HP_SIZE = TEXT_SIZE;
	const XMFLOAT4 TEXT_HP_COLOR = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	// MP�Q�[�W
	const std::string TEXT_MP = std::string("MP");
	const XMFLOAT2 TEXT_MP_POS = XMFLOAT2(-SCREEN_WIDTH / 2 + TEXT_SIZE.x, POS_LIFEWAKU_CALCULAT.y - TEXT_HP_SIZE.y + 5);
	const XMFLOAT2 TEXT_MP_SIZE = TEXT_SIZE;
	const XMFLOAT4 TEXT_MP_COLOR = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	const int TIME_CNT = 100; // ��ʘg�Ԃ�����悤�J�E���g�i���j
}


//**************************************************************************************************************
// �R���X�g���N�^
//**************************************************************************************************************
CPlayer::CPlayer(CScene* pScene) : CModel(pScene)
{
	SetTag(GOT_PLAYER);
	SetObjID(EObjID::OI_PLAYER);
	m_pLand = nullptr;
	m_pCamera = nullptr;
	m_nAnimStack = ANIM_IDLE;
	m_nNextAnimStack = m_nAnimStack;
	m_dAnimTime = 0.0;
	
}
//**************************************************************************************************************
// �f�X�g���N�^
//**************************************************************************************************************
CPlayer::~CPlayer()
{
}
//**************************************************************************************************************
// ������
//**************************************************************************************************************
HRESULT CPlayer::Init()
{
	m_nTimeCnt = 0;   // ��ʘg��Ԃ�����p�i���j
	m_nPlusCnt = 1;   // ��ʘg��Ԃ�����p�i���j
	// **********�X�e�[�^�X�֌W**********
	// ���f���Ɠ����蔻��ݒ�
	SetModel(MT_PALADIN);
	m_fRadius = PLAYER_BS_RAD;
	m_vBBox = XMFLOAT3(50.f, m_vCenter.y, 30.0f);
	
	m_vCenter.x = 0;
	m_vCenter.z = 0;

	// ���݂̃X�e�[�g
	SetState(Wait);
	// �n�ʐݒu�t���O
	m_bGround = false;
	// �_�b�V���\��
	m_nDashNum = DASH_NUM;
	// �_�b�V���ړ���
	m_fDashVol = 0.0f;
	
	// �X�e�[�^�X
	m_Status.nMaxHP = STATUS_MAX_LIFE;
	m_Status.nHP = m_Status.nMaxHP;
	m_Status.nATK = STATUS_ATK;
	m_Status.nDEF = STATUS_DEF;
	m_Status.nINT = STATUS_INT;
	m_Status.nRES = STATUS_RES;
	
	// �m�[�h�ɓo�^
	Octree* pOctree = CScene::GetOctree();
	pOctree->EraseAndPushObjInNode(&m_NodeIndexWork, this);

	// **********�X�e�[�^�X�֌W�����܂�**********
	// **********�A�j���[�V�����p�ݒ�**********
	// �A�j���[�V������������
	m_dAnimTime = 0.0;
	// ���݂̃A�j���[�V�����ԍ�
	m_nAnimStack = ANIM_IDLE;
	// ���A�j���[�V�����̔ԍ�
	m_nNextAnimStack = m_nAnimStack;
	// **********�A�j���[�V�����p�ݒ肱���܂�**********

	// **********�Q�Ɨp�|�C���^�ݒ�**********
	// �J�����|�C���^
	m_pCamera = m_pScene->GetCameraManager()->GetCamera();
	// �n�`���f���|�C���^
	m_pLand = (CLand*)m_pScene->Find(GOT_LAND);
	// **********�Q�Ɨp�|�C���^�ݒ肱���܂�**********
	
	CText::OutPutText(TEXT_HP, TEXT_HP_POS, TEXT_HP_SIZE, TEXT_HP_COLOR);
	CText::OutPutText(TEXT_MP, TEXT_MP_POS, TEXT_MP_SIZE, TEXT_MP_COLOR);
	
	HRESULT hr = CModel::Init();

	// �q�̏�����
	// ����
	m_uqpAttackMethod = std::make_unique<CAttackMethod>(this);

	return hr;
}

// �I������
void CPlayer::Fin()
{
	// ����I������
	m_uqpAttackMethod->Fin();

	CModel::Fin();
}
//**************************************************************************************************************
// �X�V
//**************************************************************************************************************
void CPlayer::Update()
{
	// ���S���o
	if (m_nDethEffectTimer) {
		if (m_nDethEffectTimer) {
			--m_nDethEffectTimer;
			if (!m_nDethEffectTimer) {
				// �V�[���J��
				CFade::Out(SCENE_GAMEOVER);
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

	// �̂����菈��
	if (CharaState::HitBack == m_State)
	{
		m_nNextAnimStack = ANIM_HITBACK;

		// ���S����
		if (m_Status.nHP <= 0)
		{
			// ���C�t0
			m_Status.nHP = 0;
			// �A�j���X�^�b�N�ݒ�
			m_nNextAnimStack = ANIM_DYING;
			// �X�e�[�g�ύX
			SetState(CharaState::Dying);
			// ����I������
			m_uqpAttackMethod->Fin();
			// ���~�߉��o
			m_pScene->SetTimeSlowCnt(TIME_SLOW_CNT);
			// ���S��
			CSound::Play(SE_PLAYER_DIE);
		}
	}



	do {
		if (!m_bVisible)
			break;

		if (Dying == m_State)
			break;

		if (CharaState::HitBack == m_State)
			break;

		// ���[���h�ϊ��s��擾
		XMFLOAT4X4& mW = m_Transform.GetWorld();
		// ���f�����W���擾
		XMFLOAT3 vX = XMFLOAT3(mW._11, mW._12, mW._13);
		XMFLOAT3 vY = XMFLOAT3(mW._21, mW._22, mW._23);
		XMFLOAT3 vZ = XMFLOAT3(mW._31, mW._32, mW._33);
		// ���W�擾�A�N���A
		m_Transform.SetPos(XMFLOAT3(mW._41, mW._42, mW._43));
		mW._41 = mW._42 = mW._43 = 0.0f;

		// �ړ����x�x�N�g��(XMFLOAT3)
		m_Transform.SetVel(XMFLOAT3(0.0f, 0.0f, 0.0f));
		
		// �f�t�H���g�X�e�[�g�̐ݒ�
		// �U�����łȂ�
		if (CharaState::HandRangeAttack != m_State) {

			// �����[�h���łȂ�
			if (CharaState::Reload != m_State &&
				CharaState::ReloadWalk != m_State) {

				// �_�b�V�����łȂ�
				if (CharaState::Dash != m_State && CharaState::DashJump != m_State) {

					// �W�����v���łȂ�
					if (Jump != m_State) {
						// �f�t�H���g�X�e�[�g�ݒ�
						SetState(Wait);
						// �f�t�H���g�A�j���[�V�����i�ҋ@�j
						m_nNextAnimStack = ANIM_IDLE;
					}
				}
			}
		    // ********�ړ�********
			MouseMove(m_Transform.GetVel(), mW);
		}


		// �W�����v����
		//�ڒn���Ă���Ȃ�W�����v�\
		if (m_bGround) {
			// �ҋ@�A�����A�_�b�V����Ԃ���J�ډ\
			if ((
				CharaState::Wait == m_State ||
				CharaState::Walk == m_State ||
				CharaState::Dash == m_State ||
				CharaState::HandRangeAttack != m_State 
				) &&
				CInput::GetKeyTrigger(VK_SPACE))
			{
				// Y�������̉����x
				m_Transform.SetCurAccel(
					XMFLOAT3(
						m_Transform.GetCurAccel().x,
						m_Transform.GetCurAccel().y + JAUMPPOWER,
						m_Transform.GetCurAccel().z));

				// �_�b�V�����Ȃ�_�b�V���W�����v��ԂɑJ��
				if (CharaState::Dash != m_State) {
					// �X�e�[�g�ݒ�
					SetState(CharaState::Jump);
				}
				else {
					SetState(CharaState::DashJump);
				}
				// �A�j���[�V�����ԍ��ݒ�
				m_nNextAnimStack = ANIM_JUMP;// �W�����v
			}
		}
		
		if (!m_bGround)
		{
			// �㉺�����̉����x�X�V
			// �d�͂�������
			m_Transform.SetCurAccel(
				XMFLOAT3(
					m_Transform.GetCurAccel().x,
					m_Transform.GetCurAccel().y + -GRAVITY_SCALE,
					m_Transform.GetCurAccel().z));
		}


		// �ړ��x�N�g���ɉ��Z
		m_Transform.SetVel(
			XMFLOAT3(
				m_Transform.GetVel().x + m_Transform.GetCurAccel().x, 
				m_Transform.GetVel().y + m_Transform.GetCurAccel().y, 
				m_Transform.GetVel().z + m_Transform.GetCurAccel().z));
		if (m_Transform.GetVel().y < -LIMIT_VEL)
		{
			m_Transform.SetVel(
				XMFLOAT3(
					m_Transform.GetVel().x + m_Transform.GetCurAccel().x,
					-LIMIT_VEL,
					m_Transform.GetVel().z + m_Transform.GetCurAccel().z));
		}

		//**********�ړ������܂�**********

		//**********�U������**********
		m_uqpAttackMethod->AttackMethod();
		if (CharaState::HandRangeAttack == m_State)
		{
			// �A�j���[�V�����ԍ��ݒ�
			m_nNextAnimStack = ANIM_SLASH;// �����U��
		}
		DrawCurrentState(m_State);
		

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

		// ���[���h�s��ݒ�
		//SetWorld(mW);

		// �o�^�m�[�h�X�V
		Octree* pOctree = CScene::GetOctree();
		pOctree->EraseAndPushObjInNode(&m_NodeIndexWork, this);

		// **********�����蔻��**********
		Collision();
		//**********�����蔻�肱���܂�**********
		if (m_bGround)
		{
			if (CharaState::DashJump == m_State)
			{
				m_fDashVol = 0;
			}

			// �_�b�V���\����
			if (m_nDashNum <= 0)
			{
				m_nDashNum = DASH_NUM;
			}
		}

		
		// TODO ���C�t�֌W�̏������S�ẴL�����N�^���ʂȂ̂Ŋ��N���X�Ȃǂōs���ׂ�
		if (m_Status.nHP <= 0)
		{
			// �X�e�[�g�X�V
			SetState(Dying);
			// �A�j���X�^�b�N�ݒ�
			m_nNextAnimStack = ANIM_DYING;
			// ����I������
			m_uqpAttackMethod->Fin();
			// HP�͂O
			m_Status.nHP = 0;

			// ���~�߉��o
			CScene::SetTimeSlowCnt(TIME_SLOW_CNT);
			// ���S��
			CSound::Play(SE_PLAYER_DIE);
		}

		// ********** ����X�V **********
		m_uqpAttackMethod->Update();
	
	} while (0);

	// **********�A�j���[�V�����X�V**********
	AnimationUpdate();
	
	// ��ʘg�p�J�E���g�X�V�i���j
	if (m_Status.nHP < m_Status.nMaxHP / 3)
	{
		m_nTimeCnt += m_nPlusCnt;
		if (m_nTimeCnt < 0 || TIME_CNT <= m_nTimeCnt)
		{
			m_nPlusCnt *= -1;
		}
	}
	else
	{
		m_nTimeCnt = 0;
	}

	// �f�o�b�O�\��
	CDebugProc::Print("*** ��ڲ԰ ***\n");
	CDebugProc::Print("Dash:[SPACE]\n");
	CDebugProc::Print("Pitch:[W][S]\n");
	CDebugProc::Print("Roll:[A][D]\n");
	CDebugProc::Print("Fire  Missile:[L]\n");
	CDebugProc::Print("Reload  Missile:[J]\n");
	CDebugProc::Print("AngleToF Missile:[I]\n");
	CDebugProc::Print("AngleToB Missile:[K]\n");
	CDebugProc::Print("Pos :X[%f] Y[%f] Z[%f]\n", m_Transform.GetPos().x, m_Transform.GetPos().y, m_Transform.GetPos().z);
	CDebugProc::Print("Accel :X[%f] Y[%f] Z[%f]\n", m_Transform.GetCurAccel().x, m_Transform.GetCurAccel().y, m_Transform.GetCurAccel().z);
	CDebugProc::Print("\n");
}


//**************************************************************************************************************
// �`��
//**************************************************************************************************************
void CPlayer::Draw()
{
	if (!m_bVisible) return;
	
	SetAnimIndex(m_nAnimStack);
	SetAnimTime(m_dAnimTime);
	CModel::Draw();

	SetShadowRender();
	m_pBBox->SetWorld(&m_Transform.GetWorld());
	m_pBBox->SetColor(XMFLOAT3(0.0f, 0.0f, 1.0f));
	m_pBBox->DepthDraw();
	SetDefaultRender();

}


//***********
// UI�`��
//*********** 
void CPlayer::DrawUI()
{
	if (!m_bVisible) return;

	ID3D11DeviceContext* pDC = GetDeviceContext();

	SetBlendState(BS_ALPHABLEND);	// �A���t�@�u�����h�L��
	SetZWrite(false);				// Z�o�b�t�@�X�V���Ȃ�

	// ���C�t�o�[
	// ���C�t�g
	CPolygon::SetTexture(CTextureData::GetTextureResource(GAGE_WAKU));
	CPolygon::SetPos(POS_LIFEWAKU_CALCULAT.x, POS_LIFEWAKU_CALCULAT.y);
	CPolygon::SetSize(SIZE_LIFEWAKU.x, SIZE_LIFEWAKU.y);
	CPolygon::SetUV(1.0f, 1.0f);
	CPolygon::SetColor(1.0f, 1.0f, 1.0f);
	CPolygon::SetAlpha(1.0f);
	CPolygon::Draw(pDC);
	// ���C�t�w�i
	CPolygon::SetTexture(CTextureData::GetTextureResource(GAGE_HAIKEI));
	CPolygon::SetPos(POS_LIFEWAKU_CALCULAT.x, POS_LIFEWAKU_CALCULAT.y);
	CPolygon::SetSize(LIFESIZEMAX.x, LIFESIZEMAX.y);
	CPolygon::SetColor(1.0f, 1.0f, 1.0f);
	CPolygon::SetAlpha(1.0f);
	CPolygon::Draw(pDC);

	
	// �c�胉�C�t
	float fRateWork; // ���C�t�ƕ��킻�ꂼ��̌��ݒl/�ő�l����
	fRateWork = (float)m_Status.nHP / (float)STATUS_MAX_LIFE;
	if (m_Status.nHP < m_Status.nMaxHP / 3)
	{
		CPolygon::SetTexture(CTextureData::GetTextureResource(LIFEGAGE_RED));
	}
	else
	{
		CPolygon::SetTexture(CTextureData::GetTextureResource(LIFEGAGE_GREEN));
	}
	CPolygon::SetPos(POS_LIFEWAKU_CALCULAT.x - (LIFESIZEMAX.x / 2.0f) * (1.0f - fRateWork), POS_LIFEWAKU_CALCULAT.y);
	CPolygon::SetSize(LIFESIZEMAX.x * fRateWork, LIFESIZEMAX.y);
	CPolygon::SetUV(1.0f/*SIZE_LIFEWAKU.x * fRateWork*/, 1.0f);
	CPolygon::SetColor(1.0f, 1.0f, 1.0f);
	CPolygon::SetAlpha(1.0f);
	CPolygon::Draw(pDC);

	// ����Q�[�W
	// ����g
	CPolygon::SetTexture(CTextureData::GetTextureResource(GAGE_WAKU));
	CPolygon::SetPos(POS_LIFEWAKU_CALCULAT.x, POS_LIFEWAKU_CALCULAT.y - LIFETOWEAPONDISTANCE);
	CPolygon::SetSize(SIZE_LIFEWAKU.x, SIZE_LIFEWAKU.y);
	CPolygon::SetUV(1.0f, 1.0f);
	CPolygon::SetColor(1.0f, 1.0f, 1.0f);
	CPolygon::SetAlpha(1.0f);
	CPolygon::Draw(pDC);
	// ����w�i
	CPolygon::SetTexture(CTextureData::GetTextureResource(GAGE_HAIKEI));
	CPolygon::SetPos(POS_LIFEWAKU_CALCULAT.x, POS_LIFEWAKU_CALCULAT.y - LIFETOWEAPONDISTANCE);
	CPolygon::SetSize(LIFESIZEMAX.x, LIFESIZEMAX.y);
	CPolygon::SetColor(1.0f, 1.0f, 1.0f);
	CPolygon::SetAlpha(1.0f);
	CPolygon::Draw(pDC);
	// �c�蕐��Q�[�W
	int nMagazineNum = m_uqpAttackMethod->GetLongWeapon()->GetMagazineNum();
	fRateWork = (float)nMagazineNum / (float)MAX_MACHINEGUN;
	CPolygon::SetTexture(CTextureData::GetTextureResource(MP_GAGE));
	CPolygon::SetPos(POS_LIFEWAKU_CALCULAT.x - (LIFESIZEMAX.x / 2.0f) * (1.0f - fRateWork), POS_LIFEWAKU_CALCULAT.y - LIFETOWEAPONDISTANCE);
	CPolygon::SetSize(LIFESIZEMAX.x * fRateWork, LIFESIZEMAX.y);
	CPolygon::SetUV(1.0f/*SIZE_LIFEWAKU.x * fRateWork*/, 1.0f);
	CPolygon::SetColor(1.0f, 1.0f, 1.0f);
	CPolygon::SetAlpha(1.0f);
	CPolygon::Draw(pDC);

	if (m_Status.nHP < m_Status.nMaxHP / 3)
	{
		// �s���`���̉�ʒ[��Ԃ�����
		CPolygon::SetTexture(CTextureData::GetTextureResource(PINCHI_WAKU_RED));
		CPolygon::SetPos(0.0f, 0.0f);
		CPolygon::SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		CPolygon::SetUV(1.0f, 1.0f);
		CPolygon::SetColor(1.0f, 1.0f, 1.0f);
		CPolygon::SetAlpha((1.0f / TIME_CNT) * m_nTimeCnt);
		CPolygon::Draw(pDC);
	}

	CPolygon::SetUV(1.0f, 1.0f);
	CPolygon::SetColor(0.0f, 0.0f, 0.0f);

	SetZWrite(true);							// Z�o�b�t�@�X�V����
	SetBlendState(BS_NONE);		                // �A���t�@�u�����h����
}

//**************************************************************************************************************
// ���ύX�t���O
//**************************************************************************************************************
void CPlayer::SetVisible(bool bVisible)
{ 
	m_bVisible = bVisible; 
	//m_pHandWeaponManager->SetVisible(bVisible);
	//m_pLongRangeWeaponManager->SetVisible(bVisible);
}

//=============================================================================
// �ړ������܂Ƃ� : �L�[�{�[�h�ƃ}�E�X����
//=============================================================================
void CPlayer::MouseMove(XMFLOAT3& vMove, XMFLOAT4X4& mW)
{

	// �ړ��x�N�g��(XMVECTOR)
	XMVECTOR vecMove = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	// �J�������[���h�ϊ��s��擾
	XMFLOAT4X4& mtxCameraWorld = m_pCamera->GetWorldMatrix();
	// ���f�����W���擾
	XMFLOAT3 vX = XMFLOAT3(mtxCameraWorld._11, mtxCameraWorld._12, mtxCameraWorld._13);
	XMFLOAT3 vY = XMFLOAT3(mtxCameraWorld._21, mtxCameraWorld._22, mtxCameraWorld._23);
	XMFLOAT3 vZ = XMFLOAT3(mtxCameraWorld._31, mtxCameraWorld._32, mtxCameraWorld._33);
	// **********�ړ�**********
	// �L�[���͂ɂ��ړ���������p�x�N�g��(�ʂ͊܂܂Ȃ�)
	XMVECTOR InputMove = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	if (CInput::GetKeyPress(VK_W)) {
		// �P�ʉ����ĉ��Z
 		InputMove -= XMVector3Normalize(XMVector3Cross(
			XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f),
			XMVectorSet(
				vX.x,
				0.0f,
				vX.z,
				1.0f)));
		// �P�ʉ�
		InputMove = XMVector3Normalize(InputMove);

		// �X�e�[�g�ݒ�
		if (CharaState::Dash != m_State  && CharaState::DashJump != m_State) {
			if (CharaState::Reload == m_State || CharaState::ReloadWalk == m_State)
			{
				// �X�e�[�g�ύX
				SetState(CharaState::ReloadWalk);
			}
			else
			{
				// �X�e�[�g�ύX
				SetState(CharaState::Walk);
			}
		}
		// �A�j���[�V�����ԍ��ݒ�
		m_nNextAnimStack = ANIM_RUN;
	}
	if (CInput::GetKeyPress(VK_S)) {
		// �P�ʉ����ĉ��Z
		InputMove += XMVector3Normalize(XMVector3Cross(
			XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f),
			XMVectorSet(
				vX.x,
				0.0f,
				vX.z,
				1.0f)));
		// �P�ʉ�
		InputMove = XMVector3Normalize(InputMove);

		// �X�e�[�g�ݒ�
		if (CharaState::Dash != m_State  && CharaState::DashJump != m_State) {
			if (CharaState::Reload == m_State || CharaState::ReloadWalk == m_State)
			{
				// �X�e�[�g�ύX
				SetState(CharaState::ReloadWalk);
			}
			else
			{
				// �X�e�[�g�ύX
				SetState(CharaState::Walk);
			}
		}

		// �A�j���[�V�����ԍ��ݒ�
		m_nNextAnimStack = ANIM_RUN;
	}
	if (CInput::GetKeyPress(VK_A)) {
		// �P�ʉ����ĉ��Z
		InputMove -= XMVector3Normalize(XMVectorSet(
			vX.x,
			0.0f,
			vX.z,
			1.0f));
		// �P�ʉ�
		InputMove = XMVector3Normalize(InputMove);

		// �X�e�[�g�ݒ�
		if (CharaState::Dash != m_State && CharaState::DashJump != m_State) {
			if (CharaState::Reload == m_State || CharaState::ReloadWalk == m_State)
			{
				// �X�e�[�g�ύX
				SetState(CharaState::ReloadWalk);
			}
			else
			{
				// �X�e�[�g�ύX
				SetState(CharaState::Walk);
			}
		}

		// �A�j���[�V�����ԍ��ݒ�
		m_nNextAnimStack = ANIM_RUN;
	}
	if (CInput::GetKeyPress(VK_D)) {
		// �P�ʉ����ĉ��Z
		InputMove += XMVector3Normalize(XMVectorSet(
			vX.x,
			0.0f,
			vX.z,
			1.0f));
		// �P�ʉ�
		InputMove = XMVector3Normalize(InputMove);

		// �X�e�[�g�ݒ�
		if (CharaState::Dash != m_State && CharaState::DashJump != m_State) {
			if (CharaState::Reload == m_State || CharaState::ReloadWalk == m_State)
			{
				// �X�e�[�g�ύX
				SetState(CharaState::ReloadWalk);
			}
			else
			{
				// �X�e�[�g�ύX
				SetState(CharaState::Walk);
			}
		}

		// �A�j���[�V�����ԍ��ݒ�
		m_nNextAnimStack = ANIM_RUN;
	}

	//*********************
	// �ړ������̌���
	//*********************
	// ���f���P�ʉ�:XZ���ʂ̈ړ���������
	vecMove += InputMove;
	vecMove = XMVector3Normalize(vecMove);
	// ���f���̌���(�ړ������Ɍ�����)
	m_Transform.RotationModel(XMFLOAT3(XMVectorGetX(vecMove), XMVectorGetY(vecMove), XMVectorGetZ(vecMove)));

	//*********************
	// �ړ��ʂ̌���
	//*********************
	// �X�e�[�g��������ԂȂ�ړ��ʐݒ�
	if (CharaState::Walk == m_State || CharaState::ReloadWalk == m_State)
	{
		XMStoreFloat3(&vMove, vecMove * WALKSPEED);
	}

	if (0 < m_nDashNum)
	{
		// �_�b�V���X�e�[�g�ݒ菈��(�ҋ@��Ԃ�������Ԃ���J�ډ\)
		if ((CharaState::Wait == m_State || CharaState::Walk == m_State) &&
			CInput::GetKeyTrigger(VK_LSHIFT))
		{
			// �n��_�b�V��
			if (m_bGround) {
				m_fDashVol = DASHVOL;
				// �_�b�V���񐔌���
				--m_nDashNum;
				// �X�e�[�g�ύX
				SetState(Dash);
				m_nNextAnimStack = ANIM_RUN;
			}
		}
	}
	if ((CharaState::Dash == m_State) &&
		CInput::GetKeyRelease(VK_LSHIFT))
	{
		// �X�e�[�g�ύX
		SetState(CharaState::Wait);
		// �_�b�V���ړ��ʃ��Z�b�g
		m_fDashVol = 0;
	}
	
	// �X�e�[�g���_�b�V���Ȃ�ړ����xUp
	if (CharaState::Dash == m_State || CharaState::DashJump == m_State)
	{
		// ���݌����Ă�������̃x�N�g���Ɍ��݂̃_�b�V���ړ��ʂ������A�����x�ɉ�����
		XMStoreFloat3(&vMove, XMVector3Normalize(XMVectorSet(mW._31, mW._32, mW._33, 1.0f)) * m_fDashVol);

		if (CharaState::DashJump != m_State) {
			// �_�b�V���ړ��ʌ���(0.5�ȉ��Ȃ�0�ɂ��ăX�e�[�g��ҋ@��Ԃ�)
			m_fDashVol *= DASHVOLDEL;
		}
		if (m_fDashVol < DASHVOLDELLIMIT)
		{
			m_fDashVol = 0;
			// �X�e�[�g�ύX
			SetState(CharaState::Wait);
		}
	}
}

//=============================================================================
// ���݂̃X�e�[�g�`��
//=============================================================================
void CPlayer::DrawCurrentState(CharaState state)
{
	switch (m_State)
	{
	case CharaState::Wait:
		CDebugProc::Print("State : Wait\n");
		break;
	case CharaState::Walk:
		CDebugProc::Print("State : Walk\n");
		break;
	case CharaState::Dash:
		CDebugProc::Print("State : Dash\n");
		break;
	case CharaState::Jump:
		CDebugProc::Print("State : Jump\n");
		break;
	case CharaState::DashJump:
		CDebugProc::Print("State : DashJump\n");
		break;
	case CharaState::HandRangeAttack:
		CDebugProc::Print("State : Attack\n");
		break;
	case CharaState::LongRangeAttack:
		CDebugProc::Print("State : LongRangeAttack\n");
		break;
	case CharaState::Reload:
		CDebugProc::Print("State : Reload\n");
		break;
	case CharaState::ReloadWalk:
		CDebugProc::Print("State : ReloadWalk\n");
		break;
	case CharaState::Dying:
		CDebugProc::Print("State : Dying\n");
		break;
	default:
		break;
	}
}

//=============================================================================
// �A�j���[�V�����X�V�����܂Ƃ�
//=============================================================================
void CPlayer::AnimationUpdate()
{
	// TODO : �A�j���[�V�����X�V(�S�L�����A�j���[�V��������̂Ŋ��N���X�ōs���ׂ�)
	// �A�j���[�V�����ԍ��X�V
	switch (m_nAnimStack)
	{
	// �������͂���Ă��Ȃ���Αҋ@�A�j���[�V�����ɑJ�ڂ���
	case ANIM_IDLE:
	case ANIM_WALK:
	case ANIM_RUN:
	case ANIM_SLASH:
	case ANIM_HITBACK:
		if (m_nAnimStack != m_nNextAnimStack) {
			m_nAnimStack = m_nNextAnimStack;
			m_dAnimTime = 0.0;
		}
		break;
	case ANIM_JUMP:
		if (m_nAnimStack != m_nNextAnimStack) {
			if (ANIM_DYING == m_nNextAnimStack) {
				m_nAnimStack = m_nNextAnimStack;
				m_dAnimTime = 0.0;
			}
		}
		break;
	default:
		break;
	}

	switch (m_nAnimStack)
	{
	case ANIM_JUMP:
		// �ڒn���Ă��Ȃ�
		if (!m_bGround)
		{
			// ���݂̃A�j���[�V�������Ԃ����A�j���[�V�������Ԃ𒴂��Ă��Ȃ�
			if (m_dAnimTime <= GetAnimDuration(m_nAnimStack))
			{
				// ���݂̃A�j���[�V�������Ԃ��󒆃|�[�Y�t���[���ȉ�
				if (m_dAnimTime < GetAnimDuration(ANIM_JUMP) / 2)
				{
					// �A�j���[�V�������ԍX�V
					if (CharaState::Dash == m_State || CharaState::DashJump == m_State)
					{
						m_dAnimTime += ANIM_SPEED + ANIM_SPEED * (m_fDashVol / DASHVOL);
					}
					else
					{
						m_dAnimTime += ANIM_SPEED;
					}
				}
				else
				{
					// ���ʒ��Ȃ�ĂуA�j���[�V�������ԍX�V
					if (m_Transform.GetCurAccel().y < 0)
					{
						m_dAnimTime += ANIM_SPEED;
					}
				}
			}
		}
		else// �ڒn���Ă���
		{
			// �X�e�[�g�ݒ�
			SetState(Wait);
			// �A�j���[�V�����ԍ���ҋ@�A�j���[�V�����ԍ���
			m_nAnimStack = ANIM_IDLE;
			m_dAnimTime = 0.0;
		}

		break;
	default:
		if (CharaState::Dash == m_State || CharaState::DashJump == m_State )
		{
			m_dAnimTime += ANIM_SPEED + ANIM_SPEED * (m_fDashVol / DASHVOL);
		}
		else
		{
			m_dAnimTime += ANIM_SPEED;
		}
		break;
	}
	// �A�j���[�V�������ԍX�V
	// �A�j���[�V�����J�n���Ă���̎��Ԃ����A�j���[�V�������Ԃ𒴂��Ă���ƁA�f�t�H���g�̃A�j���[�V�����ɑJ�ڂ���
	double Double = GetAnimDuration(m_nAnimStack);
	if (m_dAnimTime >= GetAnimDuration(m_nAnimStack)) 
	{
		switch (m_nAnimStack) {
		case !ANIM_JUMP:
			// �A�j���[�V�������Ԃ����Z�b�g
			m_dAnimTime = 0.0;
		case ANIM_SLASH:
		case ANIM_HITBACK:
			// �X�e�[�g�ݒ�
			SetState(CharaState::Wait);
			// �A�j���[�V�����ԍ���ҋ@�A�j���[�V�����ԍ���
			m_nAnimStack = ANIM_IDLE;
			// �čU���\
			m_uqpAttackMethod->GetHandWeapon();
			break;
		case ANIM_DYING:
			// ���̌�̔����ł����s���ɂȂ�
			if (m_bVisible) {
				// �G�t�F�N�V�A
				StartDeath();

				// �ްѵ��ް���̏���
				CSound::Play(SE_EXPLOSION);

			}
			break;
		}
	}
}