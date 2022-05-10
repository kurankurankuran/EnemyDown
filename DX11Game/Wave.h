// エフェクト [Effect.h]
#pragma once
#include "Mesh.h"
#include "main.h"
#include "Scene.h"

#include <list>

//*****************************************************************************
// 構造体定義
//*****************************************************************************
class CWaveParam {
public:
	XMFLOAT3 position;			// 位置座標 (Position)
	float	amplitude;			// 振幅 (Amplitude)
	float	time;				// 時間
};


class CWave : public CMesh
{
public:
	CWave(CScene*);
	virtual ~CWave();


private:
	CScene* m_pScene;
	ID3D11ShaderResourceView* m_pTexture;
	VERTEX_3D* m_pVertex;
	int* m_pIndex;
	CWaveParam*       m_pWaveParam;

public:

	HRESULT Init();
	void Fin();
	void Update();
	void Draw();

};
