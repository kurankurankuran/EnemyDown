// エフェクト [Effect.h]
#pragma once
#include "Mesh.h"
#include "main.h"
#include "Scene.h"

#include <list>

const int MAX_EFFECT = 100;


enum EFFECT_TYPE {
	SMOKE_EFFECT = 0,

	MAX_SMOKE
};


struct EffectPar {

	EFFECT_TYPE m_Type;
	XMFLOAT3 m_vPos;
	XMFLOAT2 m_vScale;
	float    m_fAlpha;


};

class CEffect : public CMesh
{
public:
	CEffect();
	virtual ~CEffect();


private:
	CScene* m_pScene;
	ID3D11ShaderResourceView* m_pTex;

	std::list<EffectPar> m_EffectPar;

	/*XMFLOAT3 m_vPos;
	XMFLOAT3 m_vScale;
	float    m_fAlpha;*/
public:
	
	HRESULT Init(CScene*);
	void Fin();
	void Update();
	void Draw();

	void SetEffect(EFFECT_TYPE nType,XMFLOAT3 pos,XMFLOAT2 size);
};
