//=============================================================================
//
// ���f�� �N���X��` [Model.h]
// Author : KAZUKI KURA
//
//=============================================================================
#pragma once
#include "GameObj.h"
#include "AssimpModel.h"

#define PLAYER_BS_RAD (200.0f)
#define PLAYER_CENTER_X (0.0f)
#define PLAYER_CENTER_Y (0.0f)
#define PLAYER_CENTER_Z (0.0f)

#define ENEMY_BS_RAD (50.0f)
#define ENEMY_CENTER_X (0.0f)
#define ENEMY_CENTER_Y (0.0f)
#define ENEMY_CENTER_Z (0.0f)

#define MISSILE_BS_RAD (50.0f)
#define MISSILE_CENTER_X (0.0f)
#define MISSILE_CENTER_Y (0.0f)
#define MISSILE_CENTER_Z (0.0f)

enum EModelType {
	MT_PALADIN = 0, // �R�m
	MT_MUTANT,	    // �~���[�^���g
	MT_WIZARD,	    // ���@�g��
	MT_MARIA,	    // �}���A
	MT_SPHERE,		// �G����
	MT_SKY,		    // �X�J�C�h�[��
	MT_LAND,		// �n��
	MT_CUBE,        // �L���[�u
	MT_ROCK,		// ��
	MT_CLIFF,       // �R

	MAX_MODEL
};

//// �X�e�[�g��
//enum CharaState {
//	Wait = 0,
//	Walk,
//	Dash,
//	Jump,
//	DashJump,
//	Attack,
//	LongRangeAttack,
//	Reload,
//	ReloadWalk,
//	HitBack,
//	Dying
//};

class CLand;
class CModel : public CGameObj
{
private:
	static CAssimpModel* m_pModels[MAX_MODEL];
	CAssimpModel* m_pModel;
	EModelType m_ModelType;
protected:
	//// �X�e�[�g
	//CharaState m_State;
	// �n�`
	CLand* m_pLand;
	// ���S�G�t�F�N�g�Đ����ԃ^�C�}�[
	int m_nDethEffectTimer; 
	// ���݂̃A�j���[�V�����ԍ�
	int m_nAnimStack;
	// ���̃A�j���[�V�����ԍ�
	int m_nNextAnimStack;
	// �A�j���[�V��������
	double m_dAnimTime;
public:
	// ********�R���X�g���N�^�E�f�X�g���N�^*******
	CModel(CScene* pScene);
	virtual ~CModel();

	// ********���f���f�[�^�ǂݍ��݁E�������*********
	static HRESULT LoadAll();
	static void ReleaseAll();

	// ********�S�又��*******
	virtual HRESULT Awake();
	virtual HRESULT Init();
	virtual void Fin();
	virtual void Update();
	virtual void Draw();
	virtual void DrawAlpha();

	// *********�Q�b�^�[�Z�b�^�[********
	// ���f���f�[�^
	void SetModel(EModelType model);
	EModelType GetModelType(void) { return m_ModelType; }
	// Asiimp�f�[�^
	static CAssimpModel* GetAssimp(EModelType model);
	//// �X�e�[�g
	//void SetState(CharaState state) { m_State = state; }
	//CharaState GetState(void) { return m_State; }
	// ��_���[�W�E���S���o�J�n(GameObjTag����G�t�F�N�g�������I��)
	void StartHitMotion();
	void StartDeath();       
	// �J�����E���C�g
	void SetCamera(CCamera* camera) { m_pModel->SetCamera(camera); }
	void SetLight(CLight* light) { m_pModel->SetLight(light); }
	// �A�j���֌W
	void SetAnimIndex(int nAnimIndex) { m_pModel->SetAnimIndex(nAnimIndex); }
	void SetAnimTime(double dTime) { m_pModel->SetAnimTime(dTime); }
	double GetAnimDuration(int nAnimIndex = -1/* -1:���݂̃A�j�� */) { return m_pModel->GetAnimDuration(nAnimIndex); }
	int GetAnimStack(void) { return m_nAnimStack; }
	int GetNextAnimStack(void) { return m_nNextAnimStack; }
	double GetAnimTime(void) { return m_dAnimTime; }
	XMFLOAT4X4 GetBoneGlobalMatrix(std::string BoneName);
	
	// *********�����蔻��**********
	// �Փ˂��������OBB�̊O���ɉ����o��
	std::vector<CGameObj*> CollisionOBBPlane(float* fAngle = nullptr, XMFLOAT3* newPos = nullptr);
	// OBB�����蔻��(�m�[�h���d�Ȃ��Ă���I�u�W�F�N�g�Ƃ�)
	bool CollisionOBB(CGameObj* pOther);
	// �����蔻��܂Ƃ�
	void Collision();    
};