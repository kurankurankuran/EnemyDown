// ������ [Box.h]
#pragma once
#include "Mesh.h"
#include<memory>

class CBox : public CMesh
{
private:
	static ID3D11VertexShader* m_pVertexShader;	// ���_�V�F�[�_
	static ID3D11InputLayout* m_pInputLayout;	// ���_�t�H�[�}�b�g
	static ID3D11PixelShader* m_pPixelShader;	// �s�N�Z���V�F�[�_


	VERTEX_3D* m_pVertex;
	UINT*      m_pIndex;
	UINT       m_nVertexNum;
	UINT       m_nIndexNum;
	XMFLOAT3   m_VertexPosArray[8];// OBB�̊p�W���_�̍��W�i�����蔻��Ŏg�p�j
	float      m_fCenterToVertexDistance; // OBB�̒��S�_���璸�_�܂ł̋���

	static ID3D11Buffer* m_pConstantBuffer;// �~�j�}�b�v�`��V�F�[�_�ɓn���p
	std::shared_ptr<CCamera> m_pMiniMapCamera;             // �~�j�}�b�v�p�J�����|�C���^
	XMFLOAT4 m_vColor;
	bool     m_bTrigger;                   // �C�x���g�N���p�����蔻��Ƃ��Ă̂ݎg���itrue�ł��蔲����j
public:
	CBox();
	virtual ~CBox();

	static HRESULT InitShader();
	static void FinShader();

	HRESULT Init(XMFLOAT3 vBBox);
	void SetColor(XMFLOAT3 color);
	void DepthDraw();// �~�j�}�b�v�p�e�N�X�`���ւ̏�������

	VERTEX_3D* GetVertex() { return m_pVertex; }
	UINT*      GetIndex() { return m_pIndex; }
	UINT       GetVertexNum() { return m_nVertexNum; }
	UINT       GetIndexNum() { return m_nIndexNum; }

	XMFLOAT3*  GetVertexPosArray() { return m_VertexPosArray; }
	float GetCenterToVertexDistance() { return m_fCenterToVertexDistance; }

	void SetMiniMapCamera(std::shared_ptr<CCamera> pCamera) { m_pMiniMapCamera = pCamera; }
	void SetTrigger(bool bTrigger) { m_bTrigger = bTrigger; }
	bool GetTrigger() { return m_bTrigger; }
};
