// �G�t�F�N�g [Effect.h]
#pragma once
#include "Mesh.h"
#include "main.h"
#include "Scene.h"

#include <list>

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
class CWaveParam {
public:
	XMFLOAT3 position;			// �ʒu���W (Position)
	float	amplitude;			// �U�� (Amplitude)
	float	time;				// ����
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
