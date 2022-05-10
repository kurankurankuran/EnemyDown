#pragma once
#include "main.h"

class CTransform
{
public:
	CTransform();                                                        // コンストラクタ
	~CTransform();                                                       // デストラクタ
	void RotationModel(XMFLOAT3 vMove);                                  // 向き調整
	void UpdateWorld();                                                  // ワールド行列にトランスフォームを反映
	void AddPos(XMFLOAT3 vVector);                                       // 位置にvectorを加算
	XMFLOAT3 GetForward() { return XMFLOAT3(m_mWorld._31, m_mWorld._32, m_mWorld._33); }
																		 
	// セッター・ゲッター
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
	XMFLOAT3 m_vStartPos;       // 初期座標
	XMFLOAT3 m_vPos;		    // 座標
	XMFLOAT3 m_vAngle;		    // 角度
	XMFLOAT3 m_vScale;		    // 拡縮
	XMFLOAT3 m_vVel;		    // 速度
	float    m_fVelMax;         // 最高速度
	XMFLOAT3 m_vCurAccel;		// 現在加速度ベクトル
	float    m_fMoveAccel;	    // 移動加速度スケール
	float    m_fFallSpeedMax;   // 最大落下速度
	float    m_fRollSpeed;      // 振り向き速度
	float    m_fGravityScale;   // 重力スケール
	XMFLOAT4X4 m_mWorld;	    // ワールド行列
};
