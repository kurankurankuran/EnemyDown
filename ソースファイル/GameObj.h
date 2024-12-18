//=============================================================================
//
// �I�u�W�F�N�g �N���X��` [GameObj.h]
// Author : KAZUKI KURA
//
//=============================================================================
#pragma once
#include "main.h"
#include "Transform.h"
#include "Box.h"		  // �f�o�b�O�\���p
#include "Sphere.h"		  // �f�o�b�O�\���p
#include "Octree.h"       // �I�N�c���[�\���̗p
#include "SaveObjBase.h"  // �f�[�^�ۑ��p�N���X 


// ********�񋓌^********
// �^�O
enum EGameObjTag {		// �I�u�W�F�N�g�^
	GOT_GAMEOBJ = 0,	// �^����
	GOT_PLAYER,			// �v���C���[
	GOT_ENEMYMANAGER,   // �G�l�~�[�Ǘ�
	GOT_ENEMY,			// �G
	GOT_BOSS,			// �{�X
	GOT_STAGESPHERE,    // �X�e�[�W���E��
	GOT_LAND,			// �n��
	GOT_DEATHOBJ,       // ���S����I�u�W�F�N�g

	MAX_GAMEOBJTAG
};

// �o�E���f�B�̎��
enum EBoundingShape { // �f�o�b�O�\���p���E�`��
	BOUND_NONE = 0, // ����
	BOUND_SPHERE, // ���E��
	BOUND_AABB, // ���EBOX(AABB)
	BOUND_OBB, // ���EBOX(OBB)
	MAX_BOUND
};

// �X�e�[�g��
enum CharaState {
	Wait = 0,
	Walk,
	Dash,
	Jump,
	DashJump,
	HandRangeAttack,
	LongRangeAttack,
	Reload,
	ReloadWalk,
	HitBack,
	Dying
};

// *********�\���̒�`********
// �X�e�[�^�X
typedef struct
{
	int nMaxHP;// �ő�̗�
	int nHP;   // ���ݑ̗�
	int nATK;  // �����U����
	int nDEF;  // �����h���
	int nINT;  // ���@�U����
	int nRES;  // ���@�h���
}CharaStatus;




// �N���X��`
class CScene;
class CGameObj : public CSaveObjBase
{
private:
	// �A�����X�g�p�|�C���^
	CGameObj* m_pBack;		                     // �O�̃I�u�W�F�N�g
	CGameObj* m_pNext;		                     // ���̃I�u�W�F�N�g
	static DATARECORD m_gDataRecord[];           // �ۑ�����f�[�^�z��
protected:
	// �Z�[�u�p�����o�ϐ�
	XMFLOAT3 m_vStartPos;                        // �X�|�[���ʒu
	XMFLOAT3 m_vStartAngle;                      // ��]��
	XMFLOAT3 m_vStartScale;                      // �g�k


	CScene* m_pScene;		                     // �����V�[��
	EGameObjTag m_Tag;		                     // �I�u�W�F�N�g�^
	CTransform m_Transform;                      // �g�����X�t�H�[��
	bool m_bGround;                              // �n�ʐݒu�t���O
	bool m_bVisible;		                     // �\���t���O
	CharaState m_State;                          // �X�e�[�g
	CharaStatus m_Status;                        // �X�e�[�^�X
	float m_fHitStopTime;                        // �q�b�g�X�g�b�v����
	int m_nInvTime;                              // ���G����
	XMFLOAT3 m_vCenter;		                     // ���E�{�b�N�X(/��)���S���W
	XMFLOAT3 m_vBBox;		                     // ���E�{�b�N�X
	CBox* m_pBBox;			                     // ���E�{�b�N�X�\���p���b�V��
	float m_fRadius;		                     // ���E�����a
	CSphere* m_pBSphere;	                     // ���E���\���p���b�V��
	EBoundingShape m_eBS;                        // ���E�v��\���p�t���O
	NODEINDEXWORK m_NodeIndexWork;               // �����m�[�h�ԍ��i�[�p
public:
	CGameObj(CScene* pScene);                    // �R���X�g���N�^
	virtual ~CGameObj(void);                     // �f�X�g���N�^

	// *******�S�又���֌W*****
	virtual HRESULT Awake();                     // �ŏ��̏�����
	virtual HRESULT Init();		                 // ������
	virtual void Update();		                 // �X�V
	virtual void Draw();		                 // �s���������`��
	virtual void DrawAlpha();	                 // �����������`��
	virtual void DrawUI();                       // UI�`��
	virtual void Fin();			                 // �I������

	// *******�A�����X�g���g�p�����I�u�W�F�N�g�S�Ăɑ΂��鏈��******
	static HRESULT AwakeAll(CGameObj* pTop);
	static HRESULT InitAll(CGameObj* pTop);
	static void UpdateAll(CGameObj* pTop);
	static void DrawAll(CGameObj* pTop);
	static void DrawUIAll(CGameObj* pTop);
	static void FinAll(CGameObj* pTop);

	// ********�����o�ϐ��Q�b�^�[�Z�b�^�[********
	// �������W�E��]�E�g�k
	XMFLOAT3 GetStartPos() { return m_vStartPos; }
	void SetStartPos(XMFLOAT3 vStartPos) { m_vStartPos = vStartPos; }
	XMFLOAT3 GetStartAngle() { return m_vStartAngle; }
	void SetStartAngle(XMFLOAT3 vStartAngle) { m_vStartAngle = vStartAngle; }
	XMFLOAT3 GetStartScale() { return m_vStartScale; }
	void SetStartScale(XMFLOAT3 vStartScale) { m_vStartScale = vStartScale; }
	// �g�����X�t�H�[��
	CTransform* GetTransform() { return &m_Transform; }
	void SetTransform(CTransform TF) { m_Transform = TF; }
	// �ڒn�t���O
	bool GetOnGround() { return m_bGround; }
	void SetOnGround(bool bOn) { m_bGround = bOn; }
	// �^�O
	void SetTag(EGameObjTag Tag) { m_Tag = Tag; }
	EGameObjTag GetTag() { return m_Tag; }
	// �A�����X�g�|�C���^
	void SetNext(CGameObj* pObj) { m_pNext = pObj; }
	CGameObj* GetNext() { return m_pNext; }
	void SetBack(CGameObj* pObj) { m_pBack = pObj; }
	CGameObj* GetBack() { return m_pBack; }
	// �X�e�[�g
	void SetState(CharaState state) { m_State = state; }
	CharaState GetState(void) { return m_State; }
	// �X�e�[�^�X
	void SetStatus(CharaStatus charastatus) { m_Status = charastatus; }
	CharaStatus GetStatus(void) { return m_Status; }
	void AddHP(int nHP);
	void SetMaxHP(int nMaxHP) { m_Status.nMaxHP = nMaxHP; };
	void SetHP(int nHP);
	// ���G����
	void SetInvTime(int nIntTime) { m_nInvTime = nIntTime; }
	int GetInvTime(void) { return m_nInvTime; }
	// �q�b�g�X�g�b�v����
	void SetHitStopTime(float fTime) { m_fHitStopTime = fTime; if (m_fHitStopTime < 0)m_fHitStopTime = 0; }
	float GetHitStopTime() { return m_fHitStopTime; }
	// ������
	virtual void SetVisible(bool bVisible = true) { m_bVisible = bVisible; }
	bool GetVisible() { return m_bVisible; }
	// �o�E���f�B�{�b�N�X�E�X�t�B�A�֌W
	void SetCenter(XMFLOAT3 vCenter) { m_vCenter = vCenter; }
	XMFLOAT3& GetCenter() { return m_vCenter; }
	void SetBBox(XMFLOAT3 vBBox);
	XMFLOAT3& GetBBox() { return m_vBBox; }
	void SetRadius(float fRadius);
	float GetRadius() { return m_fRadius; }
	// �����蔻��ؑ֗p
	void SetBSVisible(EBoundingShape eBS) { m_eBS = eBS; }
	CBox* GetBBOXInstance() { return m_pBBox; }
	// �����m�[�h�ԍ��i�[�p
	NODEINDEXWORK* GetIndexWork() { return &m_NodeIndexWork; }

	// ***********�����蔻�胁�\�b�h***********
	std::vector<CGameObj*>  CollisionOBB();            // OBB�����蔻��(�����̃m�[�h�ɓo�^����Ă���I�u�W�F�N�g�Ƃ�)
	virtual bool CollisionOBB(CGameObj* pOther);       // OBB�����蔻��(�m�[�h���d�Ȃ��Ă���I�u�W�F�N�g�Ƃ�)
	bool CollisionAABB(CGameObj* pOther);              // AABB�����蔻��
	bool CollisionBS(CGameObj* pOther);                // BS�����蔻��
	
	virtual DATARECORD *GetDataRecord() { 
		return m_gDataRecord; 
	}
};
