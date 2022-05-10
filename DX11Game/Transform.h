#pragma once
#include "main.h"

class CTransform
{
public:
	CTransform();                                                        // �R���X�g���N�^
	~CTransform();                                                       // �f�X�g���N�^
	void RotationModel(XMFLOAT3 vMove);                                  // ��������
	void UpdateWorld();                                                  // ���[���h�s��Ƀg�����X�t�H�[���𔽉f
	void AddPos(XMFLOAT3 vVector);                                       // �ʒu��vector�����Z
	XMFLOAT3 GetForward() { return XMFLOAT3(m_mWorld._31, m_mWorld._32, m_mWorld._33); }
																		 
	// �Z�b�^�[�E�Q�b�^�[
	void SetStartPos(XMFLOAT3 vStartPos) { m_vStartPos = vStartPos; }
	XMFLOAT3& GetStartPos() { return m_vStartPos; }
	void SetPos(XMFLOAT3 vPos) { m_vPos = vPos; }
	XMFLOAT3& GetPos() { return m_vPos; }
	void SetAngle(XMFLOAT3 vAngle) { m_vAngle = vAngle; }
	XMFLOAT3& GetAngle() { return m_vAngle; }
	virtual void SetScale(XMFLOAT3 vScale) { m_vScale = vScale; }
	XMFLOAT3& GetScale() { return m_vScale; }
	void SetVel(XMFLOAT3 vVel){m_vVel = vVel; }
	XMFLOAT3& GetVel() { return m_vVel; }
	void SetVelMax(float fVelMax) { m_fVelMax = fVelMax; }
	float& GetVelMax() { return m_fVelMax; }
	void SetCurAccel(XMFLOAT3 vAccel) { m_vCurAccel = vAccel; }
	XMFLOAT3& GetCurAccel() { return m_vCurAccel; }
	void SetMoveAccel(float fMoveAccel) { m_fMoveAccel = fMoveAccel; }
	float& GetMoveAccel() { return m_fMoveAccel; }
	void SetFallSpeedMax(float fFallSpeedMax) { m_fFallSpeedMax = fFallSpeedMax; }
	float& GetFallSpeedMax() { return m_fFallSpeedMax; }
	void SetRollSpeed(float fRollSpeed) { m_fRollSpeed = fRollSpeed; }
	float& GetRollSpeed() { return m_fRollSpeed; }
	void SetGravityScale(float fGravityScale) { m_fGravityScale = fGravityScale; }
	float& GetGravityScale() { return m_fGravityScale; }
	void SetWorld(XMFLOAT4X4& mWorld) { m_mWorld = mWorld; }
	XMFLOAT4X4& GetWorld() { return m_mWorld; }


	XMMATRIX ExtractOffset(const XMMATRIX& mWorld);
	XMMATRIX ExtractScaling(const XMMATRIX& mWorld);
	XMMATRIX ExtractRotation(const XMMATRIX&  mWorld);
private:
	XMFLOAT3 m_vStartPos;       // �������W
	XMFLOAT3 m_vPos;		    // ���W
	XMFLOAT3 m_vAngle;		    // �p�x
	XMFLOAT3 m_vScale;		    // �g�k
	XMFLOAT3 m_vVel;		    // ���x
	float    m_fVelMax;         // �ō����x
	XMFLOAT3 m_vCurAccel;		// ���݉����x�x�N�g��
	float    m_fMoveAccel;	    // �ړ������x�X�P�[��
	float    m_fFallSpeedMax;   // �ő嗎�����x
	float    m_fRollSpeed;      // �U��������x
	float    m_fGravityScale;   // �d�̓X�P�[��
	XMFLOAT4X4 m_mWorld;	    // ���[���h�s��
};
