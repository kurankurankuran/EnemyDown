#include"Transform.h"

CTransform::CTransform()
{
	m_vPos = XMFLOAT3(0, 0, 0);
	m_vAngle = XMFLOAT3(0, 0, 0);
	m_vScale = XMFLOAT3(1, 1, 1);
	m_vVel = XMFLOAT3(0, 0, 0);
	m_fVelMax = 10.0f;
	m_vCurAccel = XMFLOAT3(0, 0, 0);
	m_fFallSpeedMax = 100.0f;
	m_fRollSpeed = 0.5f;
	m_fGravityScale = 1;
	XMStoreFloat4x4(&m_mWorld, DirectX::XMMatrixIdentity());
}

CTransform::~CTransform()
{

}

// ベクトル方向にモデルの向きを向けさせる
void CTransform::RotationModel(XMFLOAT3 vMove)
{
	// ワールド変換行列取得
	XMFLOAT4X4 mW = GetWorld();
	if (vMove.x == 0 && vMove.y == 0 && vMove.z == 0)
	{
		return;
	}

	XMVECTOR vecZ = XMVector3Normalize(XMVectorSet(mW._31, mW._32, mW._33, 1.0f));
	XMVECTOR vecMove = XMVector3Normalize(XMVectorSet(vMove.x, vMove.y, vMove.z, 1.0f));

	// 角度計算
	float fAngle = XMVectorGetX(XMVector3AngleBetweenNormals(vecMove, vecZ));

	if (fAngle == 0)
	{
		return;
	}

	// 回転軸計算
	XMVECTOR vecAxis = XMVector3Cross(vecZ, vecMove);
	if (XMVectorGetX(vecAxis) == 0 &&
		XMVectorGetY(vecAxis) == 0 &&
		XMVectorGetZ(vecAxis) == 0)
	{
		return;
	}

	// 回転行列作成、反映
	XMMATRIX mR = XMMatrixRotationAxis(vecAxis, fAngle);
	XMMATRIX mtxWorld = XMLoadFloat4x4(&mW);
	mtxWorld = XMMatrixMultiply(mtxWorld, mR);
	XMStoreFloat4x4(&mW, mtxWorld);

	// 変換結果をワールド行列に反映
	SetWorld(mW);
	return;
}

// ワールド行列トランスフォームを反映させる
void CTransform::UpdateWorld()
{
	XMMATRIX mtxWorld, mtxScl, mtxRot, mtxTranslate;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_vAngle.x),
		XMConvertToRadians(m_vAngle.y), XMConvertToRadians(m_vAngle.z));
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックス設定
	XMStoreFloat4x4(&m_mWorld, mtxWorld);
}

void CTransform::AddPos(XMFLOAT3 vVector)                                       // 位置にvectorを加算
{
	m_vPos.x += vVector.x;
	m_vPos.y += vVector.y;
	m_vPos.z += vVector.z;
	m_mWorld._41 = m_vPos.x;
	m_mWorld._42 = m_vPos.y;
	m_mWorld._43 = m_vPos.z;
}


XMMATRIX CTransform::ExtractOffset(const XMMATRIX& mWorld) {
	return XMMatrixTranslation(mWorld.r[3].m128_f32[0], mWorld.r[3].m128_f32[1], mWorld.r[3].m128_f32[2]);
}
XMMATRIX CTransform::ExtractScaling(const XMMATRIX& mWorld) {
	return XMMatrixScaling(
		XMVector3Length(XMVECTOR{ mWorld.r[0].m128_f32[0],mWorld.r[0].m128_f32[1],mWorld.r[0].m128_f32[2] }).m128_f32[0],
		XMVector3Length(XMVECTOR{ mWorld.r[1].m128_f32[0],mWorld.r[1].m128_f32[1],mWorld.r[1].m128_f32[2] }).m128_f32[0],
		XMVector3Length(XMVECTOR{ mWorld.r[2].m128_f32[0],mWorld.r[2].m128_f32[1],mWorld.r[2].m128_f32[2] }).m128_f32[0]
	);
}
// ワールド行列から回転成分のみを抽出する
XMMATRIX CTransform::ExtractRotation(const XMMATRIX&  mWorld) {
	XMMATRIX mOffset = ExtractOffset(mWorld);
	XMMATRIX mScaling = ExtractScaling(mWorld);

	XMVECTOR det;
	// 左からScaling、右からOffsetの逆行列をそれぞれかける。
	return XMMatrixInverse(&det, mScaling) * mWorld * XMMatrixInverse(&det, mOffset);
}