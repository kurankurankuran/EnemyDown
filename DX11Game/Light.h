#pragma once
#include "main.h"

#define LIGHT_DIR_ROTVALUE (0.1f) // ïΩçsÉâÉCÉgÇÃYé≤âÒì]ó 

class CLight
{
private:
	DirectX::XMFLOAT4 m_diffuse;
	DirectX::XMFLOAT4 m_ambient;
	DirectX::XMFLOAT4 m_specular;
	DirectX::XMFLOAT3 m_direction;

	bool m_bEnable;
	
public:
	CLight();

	void Init();
	void Update();
	
	DirectX::XMFLOAT4& GetDiffuse() { return m_diffuse; }
	DirectX::XMFLOAT4& GetAmbient() { return m_ambient; }
	DirectX::XMFLOAT4& GetSpecular() { return m_specular; }
	DirectX::XMFLOAT3& GetDir();/* { return m_direction; }*/
	void SetDir(DirectX::XMFLOAT3 direction) { m_direction = direction; }

	void SetEnable(bool bEnable = true) { m_bEnable = bEnable; }
};
