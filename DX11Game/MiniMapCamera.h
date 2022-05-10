#pragma once
#include "main.h"
#include "Camera.h"


class CAssimpModel;
class CPlayer;
class CGameObj;
class CMiniMapCamera : public CCamera
{
private:
	DirectX::XMFLOAT3 m_vPos;		    // ���_
	DirectX::XMFLOAT3 m_vAngle;		    // ��]��
	DirectX::XMFLOAT3 m_vStartTarget;    // ���������_
	DirectX::XMFLOAT3 m_vTarget;	        // �����_
	DirectX::XMFLOAT3 m_vUp;		            // ����x�N�g��
	DirectX::XMFLOAT4X4 m_mtxWorld;	// ���[���h �}�g���b�N�X
	DirectX::XMFLOAT4X4 m_mtxView;	// �r���[ �}�g���b�N�X
	DirectX::XMFLOAT4X4 m_mtxProj;	// �v���W�F�N�V���� �}�g���b�N�X
	float m_fNearZ;					// �O���N���b�v����
	float m_fFarZ;					// ����N���b�v����
	CAssimpModel* m_pSky;			// �X�J�C �h�[��
	CGameObj* m_pTargetObj;				// ��ƂȂ�I�u�W�F�N�g
public:
	CMiniMapCamera();

	void Init();
	void Update();
	void Draw();
	void Clear();

	void SetPos(DirectX::XMFLOAT3& vPos) { m_vPos = vPos; }
	DirectX::XMFLOAT3& GetPos() { return m_vPos; }
	void SetPos(float x, float y, float z) { m_vPos.x = x, m_vPos.y = y, m_vPos.z = z; }
	void SetTarget(DirectX::XMFLOAT3& vTarget) { m_vTarget = vTarget; }
	void SetTarget(float x, float y, float z) { m_vTarget.x = x, m_vTarget.y = y, m_vTarget.z = z; }
	XMFLOAT3 GetTargetPos() { return m_vTarget; }
	void SetUpVector(DirectX::XMFLOAT3& vUp) { m_vUp = vUp; }
	void SetUpVector(float x, float y, float z) { m_vUp.x = x, m_vUp.y = y, m_vUp.z = z; }
	XMFLOAT3 GetUpVector() { return m_vUp; }
	void SetWorldMatrix(DirectX::XMFLOAT4X4& mtxWorld);
	DirectX::XMFLOAT4X4& GetWorldMatrix() { return m_mtxWorld; }
	DirectX::XMFLOAT4X4& CalcWorldMatrix();
	void SetFovY(float fFovY);
	void SetAspectRatio(float fAspect);
	void SetRangeZ(float fNearZ, float fFarZ);
	void UpdateMatrix();
	DirectX::XMFLOAT4X4& GetViewMatrix() { return m_mtxView; }
	DirectX::XMFLOAT4X4& GetProjMatrix() { return m_mtxProj; }
	DirectX::XMFLOAT3& GetAngle() { return m_vAngle; }
	void SetSkyDome(CAssimpModel* pSky) { m_pSky = pSky; }
	CAssimpModel* GetSkyDome() { return m_pSky; }
	void SetRadius(float fRadius) { m_fRadius = fRadius; }
	//void SetPlayer(CPlayer* pPlayer);
	void SetTargetObj(CGameObj* pObj);
	int CollisionViewFrustum(XMFLOAT3 vCenter, float fRadius);//������(0:��\��,-1:����, 1:�\��)

	//void SetTargetObj(CGameObj* pObj) { m_pTargetObj = pObj; }
	//CGameObj* GetTargetObj() { return m_pTargetObj; }
	void SetFreeBulletMode(XMFLOAT2 fFreeBulletMode) { m_fFreeBulletMode = fFreeBulletMode; } // �t���[�o���b�g���[�h���̔��a�ƃJ�����̍�����ݒ�

private:
	void CreateViewFrustum();
	void UpdateViewFrustum();
};
