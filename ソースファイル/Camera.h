#pragma once
#include "main.h"

#define	VIEW_ANGLE			(45.0f)										// ����p
#define	VIEW_ASPECT			((float)SCREEN_WIDTH/(float)SCREEN_HEIGHT)	// �A�X�y�N�g��
#define	VIEW_NEAR_Z			(1.0f)										// NearZ�l
#define	VIEW_FAR_Z			(50000.0f)									// FarZ�l




class CAssimpModel;
class CPlayer;
class CGameObj;
class CCamera
{
protected:
	static DirectX::XMFLOAT3 m_vStartPos;		// �������_
	static DirectX::XMFLOAT3 m_vPos;		    // ���_
	static DirectX::XMFLOAT3 m_vAngle;		    // ��]��
	static DirectX::XMFLOAT3 m_vStartTarget;    // ���������_
	static DirectX::XMFLOAT3 m_vTarget;	        // �����_
	static DirectX::XMFLOAT3 m_vUp;		            // ����x�N�g��
	static DirectX::XMFLOAT4X4 m_mtxWorld;	// ���[���h �}�g���b�N�X
	static DirectX::XMFLOAT4X4 m_mtxView;	// �r���[ �}�g���b�N�X
	static DirectX::XMFLOAT4X4 m_mtxProj;	// �v���W�F�N�V���� �}�g���b�N�X
	static float m_fNearZ;					// �O���N���b�v����
	static float m_fFarZ;					// ����N���b�v����
	static CAssimpModel* m_pSky;			// �X�J�C �h�[��
	static CGameObj* m_pTargetObj;				// ��ƂȂ�I�u�W�F�N�g
	static int m_nReeticleTextIndex;

	float m_fAspectRatio;			// �c����
	float m_fFovY;					// ����p(Degree)
	

	

	float m_fRadius;				// ���a

	//CPlayer* m_pPlayer;				// ��ƂȂ�I�u�W�F�N�g
	
	float m_fAngle;					// �J�����A���O��

	DirectX::XMFLOAT4 m_frus[6];	// ������
	DirectX::XMFLOAT4 m_frusw[6];	// ������(���[���h���)

	DirectX::XMFLOAT2 m_fFreeBulletMode;
public:
	CCamera();

	virtual void Init();
	virtual void Fin() {};
	virtual void Update();
	virtual void Draw() {};
	virtual void Clear();

	virtual void SetPos(DirectX::XMFLOAT3& vPos) { m_vPos = vPos; }
	virtual DirectX::XMFLOAT3& GetPos() { return m_vPos; }
	virtual void SetPos(float x, float y, float z) { m_vPos.x = x, m_vPos.y = y, m_vPos.z = z; }
	virtual void SetTarget(DirectX::XMFLOAT3& vTarget) { m_vTarget = vTarget; }
	virtual void SetTarget(float x, float y, float z) { m_vTarget.x = x, m_vTarget.y = y, m_vTarget.z = z; }
	virtual XMFLOAT3 GetTargetPos() { return m_vTarget; }
	virtual void SetUpVector(DirectX::XMFLOAT3& vUp) { m_vUp = vUp; }
	virtual void SetUpVector(float x, float y, float z) { m_vUp.x = x, m_vUp.y = y, m_vUp.z = z; }
	virtual XMFLOAT3 GetUpVector() { return m_vUp; }
	virtual void SetWorldMatrix(DirectX::XMFLOAT4X4& mtxWorld);
	virtual DirectX::XMFLOAT4X4& GetWorldMatrix() { return m_mtxWorld; }
	virtual DirectX::XMFLOAT4X4& CalcWorldMatrix();
	virtual void SetFovY(float fFovY);
	virtual void SetAspectRatio(float fAspect);
	virtual void SetRangeZ(float fNearZ, float fFarZ);
	virtual void UpdateMatrix();
	virtual DirectX::XMFLOAT4X4& GetViewMatrix() { return m_mtxView; }
	virtual DirectX::XMFLOAT4X4& GetProjMatrix() { return m_mtxProj; }
	virtual DirectX::XMFLOAT3& GetAngle() { return m_vAngle; }
	virtual void SetSkyDome(CAssimpModel* pSky) { m_pSky = pSky; }
	virtual CAssimpModel* GetSkyDome() { return m_pSky; }
	virtual void SetRadius(float fRadius) { m_fRadius = fRadius; }
	virtual void SetTargetObj(CGameObj* pObj);
	virtual int CollisionViewFrustum(XMFLOAT3 vCenter,float fRadius);//������(0:��\��,-1:����, 1:�\��)
	
	virtual void SetFreeBulletMode(XMFLOAT2 fFreeBulletMode) { m_fFreeBulletMode = fFreeBulletMode; } // �t���[�o���b�g���[�h���̔��a�ƃJ�����̍�����ݒ�
protected:
	virtual void CreateViewFrustum();
	virtual void UpdateViewFrustum();
};
