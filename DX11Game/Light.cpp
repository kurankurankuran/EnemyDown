#include "Light.h"

using namespace DirectX;

// マクロ定義
#define LIGHT0_DIRECTION	XMFLOAT3(-0.5f,-1.0f, -0.5f)
#define LIGHT0_DIFFUSE		XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f)
#define LIGHT0_AMBIENT		XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f)
#define LIGHT0_SPECULAR		XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f)

// コンストラクタ
CLight::CLight()
{
	Init();
}

// 初期化
void CLight::Init()
{
	XMFLOAT3 vDir = LIGHT0_DIRECTION;
	XMStoreFloat3(&m_direction, XMVector3Normalize(XMLoadFloat3(&vDir)));
	m_diffuse = LIGHT0_DIFFUSE;
	m_ambient = LIGHT0_AMBIENT;
	m_specular = LIGHT0_SPECULAR;
	m_bEnable = true;
}


void CLight::Update()
{
	// 平行光源の向き設定（自動:Y軸回転）
	DirectX::XMVECTOR vecDirection;
	vecDirection = DirectX::XMLoadFloat3(&m_direction);
	DirectX::XMMATRIX mtxRot;
	mtxRot = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(LIGHT_DIR_ROTVALUE));
	vecDirection = DirectX::XMVector3Transform(vecDirection, mtxRot);
	DirectX::XMStoreFloat3(&m_direction, DirectX::XMVector3Normalize(vecDirection));


}


XMFLOAT3& CLight::GetDir()
{
	if (m_bEnable) return m_direction;
	static XMFLOAT3 off(0.0f, 0.0f, 0.0f);
	return off;
}
