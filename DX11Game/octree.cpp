//=======================================================================================
//
//	  Octree�֘A���W���[��
//
//=======================================================================================
#include "octree.h"
#include "model.h"
#include <vector>
#include <algorithm>  // std::sort, std::unique

//---------------------------------------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------------------------------------
Octree::Octree()
{
	// ���[�g�m�[�h������
	m_RootNode.bIndex = false;
	ZeroMemory(&m_RootNode.box, sizeof(BOX));
	for (int i = 0; i < 8; ++i) {
		m_RootNode.child[i] = nullptr;
	}
	m_RootNode.index.nIndex = 0;
	m_RootNode.index.pIndex = nullptr;
	m_RootNode.ObjList.clear();
	
	// �f�o�b�O�p�����o�ϐ�
	m_bDrawObject = false;		                    // �`��t���O(�I�u�W�F�N�g�����݂���)
	m_bDrawRoot = false;                            // �`��t���O(���[�g�m�[�h)
	m_bDrawLandOctree = false;                      // �`��t���O�i�n�`�|���S�������݂���m�[�h�j
	m_bDrawAll = false;                             // �`��t���O(�S�Ă̍ŏ��m�[�h)
	m_SearchPoint = XMFLOAT3(0.0f, 0.0f, 0.0f);		// �T���|�C���g
	m_fRootNodeScale = 20000.0f;                    // �ŏ��m�[�h�̑傫��
	m_fMinNodeLimit = 200.0f;                       // ���[�g�m�[�h�̍ŏ����E
	float fScaleWork = m_fRootNodeScale;            // �ŏ��m�[�h�̑傫���v�Z�p���[�N
	
	// �ŏ��m�[�h�v�Z
	while (true)
	{
		fScaleWork = fScaleWork / 2;
		if (fScaleWork < m_fMinNodeLimit)
		{	
			fScaleWork *= 2;
			break;
		}
	}
	m_fMinNodeScale = fScaleWork;
}

//---------------------------------------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------------------------------------
Octree::~Octree()
{
}

//---------------------------------------------------------------------------------------
// Octree������
//---------------------------------------------------------------------------------------
void Octree::InitOctree(TAssimpVertex* pVertex, UINT* pIndex, UINT nIndex)
{
	// ��ԃ��x��������
	m_nSpaceLevel = 0;
	
	// ���[�g�m�[�h������
	m_RootNode.bIndex = false;
	ZeroMemory(&m_RootNode.box, sizeof(BOX));
	for (int i = 0; i < 8; ++i) {
		m_RootNode.child[i] = nullptr;
	}
	m_RootNode.index.nIndex = 0;
	m_RootNode.index.pIndex = nullptr;
	m_RootNode.ObjList.clear();
	
	// �ŏ��ɍő�͈͂�ݒ�
	m_RootNode.box.x_min = -m_fRootNodeScale/2.0f;			
	m_RootNode.box.x_max =  m_fRootNodeScale/2.0f;
	m_RootNode.box.y_min = -m_fRootNodeScale/2.0f;
	m_RootNode.box.y_max =  m_fRootNodeScale/2.0f;
	m_RootNode.box.z_min = -m_fRootNodeScale/2.0f;
	m_RootNode.box.z_max =  m_fRootNodeScale/2.0f;

	// ���[�g�m�[�h�͑S�Ă̒��_�̃C���f�b�N�X��������
	m_RootNode.index.pIndex = pIndex;
	m_RootNode.index.nIndex = nIndex;
	
	// �I�N�c���[���\�z
	MakeOctree(&m_RootNode, pVertex);					

	// �m�[�h�`��p���_�쐬
	MakeLineVertex();
}

//---------------------------------------------------------------------------------------
// �v���~�e�B�u������
//---------------------------------------------------------------------------------------
void Octree::MakeLineVertex()
{
	ID3D11Device* pDevice = GetDevice();

	m_meshBox.SetPimitiveType(PT_LINE);
	
	int nVertexNum = 8;
	VERTEX_3D* pVertexWk = new VERTEX_3D[nVertexNum];
	VERTEX_3D* pVtx = pVertexWk;
	pVtx[0].vtx = XMFLOAT3(0.0f, 1.0f, 0.0f);
	pVtx[1].vtx = XMFLOAT3(1.0f, 1.0f, 0.0f);
	pVtx[2].vtx = XMFLOAT3(1.0f, 0.0f, 0.0f);
	pVtx[3].vtx = XMFLOAT3(0.0f, 0.0f, 0.0f);
	pVtx[4].vtx = XMFLOAT3(0.0f, 1.0f, 1.0f);
	pVtx[5].vtx = XMFLOAT3(1.0f, 1.0f, 1.0f);
	pVtx[6].vtx = XMFLOAT3(1.0f, 0.0f, 1.0f);
	pVtx[7].vtx = XMFLOAT3(0.0f, 0.0f, 1.0f);
	for (int i = 0; i < 8; ++i, ++pVtx) {
		pVtx->diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx->nor = XMFLOAT3(0, 0, 0);
		pVtx->tex = XMFLOAT2(0, 0);
	}
	int	box_idx[] = {	// LINELIST �ŕ`�悷��Ƃ��̘A������
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7,
	};
	
	int nNumIndex = _countof(box_idx);
	int* pIndexWk = new int[nNumIndex];
	int* pIdx = pIndexWk;
	for (int i = 0; i < nNumIndex; ++i, ++pIdx) {
		*pIdx = box_idx[i];
	}

	m_meshBox.Init(pVertexWk, nVertexNum, pIndexWk, nNumIndex);

	delete[] pIndexWk;
	delete[] pVertexWk;

	m_meshSearchPoint.SetPimitiveType(PT_LINE);
	nVertexNum = 6;
	pVertexWk = new VERTEX_3D[nVertexNum];
	pVtx = pVertexWk;
	pVtx[0].vtx = XMFLOAT3(-3000.0f, 0.0f, 0.0f);
	pVtx[1].vtx = XMFLOAT3(3000.0f, 0.0f, 0.0f);
	pVtx[2].vtx = XMFLOAT3(0.0f, -3000.0f, 0.0f);
	pVtx[3].vtx = XMFLOAT3(0.0f, 3000.0f, 0.0f);
	pVtx[4].vtx = XMFLOAT3(0.0f, 0.0f, -3000.0f);
	pVtx[5].vtx = XMFLOAT3(0.0f, 0.0f, 3000.0f);
	for (int i = 0; i < nVertexNum; ++i, ++pVtx) {
		pVtx->diffuse = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		pVtx->nor = XMFLOAT3(0, 0, 0);
		pVtx->tex = XMFLOAT2(0, 0);
	}
	nNumIndex = 6;
	pIndexWk = new int[nNumIndex];
	pIdx = pIndexWk;
	for (int i = 0; i < nNumIndex; ++i, ++pIdx) {
		*pIdx = i;
	}
	
	m_meshSearchPoint.Init(pVertexWk, nVertexNum, pIndexWk, nNumIndex);
	delete[] pIndexWk;
	delete[] pVertexWk;
}

//---------------------------------------------------------------------------------------
// Octree�I��
//---------------------------------------------------------------------------------------
void Octree::DeleteOctree(NODE* nodept)
{
	for (int i = 0; i < 8; i++) {
		if (nodept->child[i]) {
			DeleteOctree(nodept->child[i]);		// �`���C���h������΍ċA�ō폜
			SAFE_DELETE_ARRAY(nodept->child[i]->index.pIndex);
			delete nodept->child[i];			// ����������폜
			nodept->child[i] = NULL;
		}
		else
		{
			SAFE_DELETE_ARRAY(nodept->index.pIndex);
		}
	}
	nodept->ObjList.clear();
	m_meshBox.Fin();
}

void Octree::ClearVector()
{
	// �ŏ��m�[�hvector���N���A
	m_NodesVector.clear();
}

//---------------------------------------------------------------------------------------
// Octree�\�z
//---------------------------------------------------------------------------------------
void Octree::MakeOctree(NODE* nodept,TAssimpVertex* pVertex)
{
	BOX testbox;
	int index;

	// �W�̈敪���s�[�g
	for (index = 0; index < 8; ++index) {
		// ���[�v�ϐ������Ɍ��ݗ̈�𕪊������Ώۗ̈�����߂�(���߂鏇�Ԃ̓��[�g�������ɏ����Ă���̂ŕς��Ȃ�����)
		switch (index)
		{
		case 0:// ���E���E��O
			testbox.x_min = nodept->box.x_min;
			testbox.x_max = (nodept->box.x_max + nodept->box.x_min) / 2;
			testbox.y_min = nodept->box.y_min;
			testbox.y_max = (nodept->box.y_max + nodept->box.y_min) / 2;
			testbox.z_min = nodept->box.z_min;
			testbox.z_max = (nodept->box.z_max + nodept->box.z_min) / 2;
			break;
		case 1:// �E�E���E��O
			testbox.x_min = (nodept->box.x_max + nodept->box.x_min) / 2;
			testbox.x_max = nodept->box.x_max;
			testbox.y_min = nodept->box.y_min;
			testbox.y_max = (nodept->box.y_max + nodept->box.y_min) / 2;
			testbox.z_min = nodept->box.z_min;
			testbox.z_max = (nodept->box.z_max + nodept->box.z_min) / 2;
			break;
		case 2:// ���E��E��O
			testbox.x_min = nodept->box.x_min;
			testbox.x_max = (nodept->box.x_max + nodept->box.x_min) / 2;
			testbox.y_min = (nodept->box.y_max + nodept->box.y_min) / 2;
			testbox.y_max = nodept->box.y_max;
			testbox.z_min = nodept->box.z_min;
			testbox.z_max = (nodept->box.z_max + nodept->box.z_min) / 2;
			break;
		case 3:// �E�E��E��O
			testbox.x_min = (nodept->box.x_max + nodept->box.x_min) / 2;
			testbox.x_max = nodept->box.x_max;
			testbox.y_min = (nodept->box.y_max + nodept->box.y_min) / 2;
			testbox.y_max = nodept->box.y_max;
			testbox.z_min = nodept->box.z_min;
			testbox.z_max = (nodept->box.z_max + nodept->box.z_min) / 2;
			break;
		case 4:// ���E���E��
			testbox.x_min = nodept->box.x_min;
			testbox.x_max = (nodept->box.x_max + nodept->box.x_min) / 2;
			testbox.y_min = nodept->box.y_min;
			testbox.y_max = (nodept->box.y_max + nodept->box.y_min) / 2;
			testbox.z_min = (nodept->box.z_max + nodept->box.z_min) / 2;
			testbox.z_max = nodept->box.z_max;
			break;
		case 5:// �E�E���E��
			testbox.x_min = (nodept->box.x_max + nodept->box.x_min) / 2;
			testbox.x_max = nodept->box.x_max;
			testbox.y_min = nodept->box.y_min;
			testbox.y_max = (nodept->box.y_max + nodept->box.y_min) / 2;
			testbox.z_min = (nodept->box.z_max + nodept->box.z_min) / 2;
			testbox.z_max = nodept->box.z_max;
			break;
		case 6:// ���E��E��
			testbox.x_min = nodept->box.x_min;
			testbox.x_max = (nodept->box.x_max + nodept->box.x_min) / 2;
			testbox.y_min = (nodept->box.y_max + nodept->box.y_min) / 2;
			testbox.y_max = nodept->box.y_max;
			testbox.z_min = (nodept->box.z_max + nodept->box.z_min) / 2;
			testbox.z_max = nodept->box.z_max;
			break;
		case 7:// �E�E��E��
			testbox.x_min = (nodept->box.x_max + nodept->box.x_min) / 2;
			testbox.x_max = nodept->box.x_max;
			testbox.y_min = (nodept->box.y_max + nodept->box.y_min) / 2;
			testbox.y_max = nodept->box.y_max;
			testbox.z_min = (nodept->box.z_max + nodept->box.z_min) / 2;
			testbox.z_max = nodept->box.z_max;
			break;
		
		
		default:
			break;
		}
		// �Ώۗ̈�̃T�C�Y���\���������Ȃ����炱��ȏ㕪�������֐����I��
		if (testbox.x_max - testbox.x_min < m_fMinNodeLimit)
		{
			// �f�o�b�O�p�ɒn�`�|���S�������݂��Ă���ŏ��m�[�h���X�g�ɒǉ�
			// �̈���ɒ��_�f�[�^�����邩�`�F�b�N
			// ���_�C���f�b�N�X���i�[���[�N
			INDEX* pIndex = new INDEX;
			pIndex->pIndex = nullptr;
			pIndex->nIndex = 0;
			if (CheckNodeBoxHit(&nodept->box, pVertex, pIndex, nodept->index))
			{
				// ���_�����݂���t���O�ݒ�
				nodept->bIndex = true;
				m_LandDrawNodesIndexVector.push_back(m_NodesVector.size());
			}
			// ���_�C���f�b�N�X���i�[���[�N���
			SAFE_DELETE_ARRAY(pIndex->pIndex);
			SAFE_DELETE(pIndex);
			
			// �ŏ��m�[�hvector�ɃZ�b�g
			m_NodesVector.push_back(nodept);
			


			break;
		}
	
		
		// ���_�C���f�b�N�X���i�[���[�N
		INDEX* pIndex = new INDEX;
		pIndex->pIndex = nullptr;
		pIndex->nIndex = 0;
		// �̈���ɒ��_�f�[�^�����邩�`�F�b�N
		if (CheckNodeBoxHit(&testbox, pVertex, pIndex, nodept->index)) 
		{
			// ��ԃ��x�����{�P
			m_nSpaceLevel++;

			// ���_�����݂���t���O�ݒ�
			nodept->bIndex = true;

			// �f�[�^���̈���ɑ��݂���Ȃ�V�K�m�[�h���쐬
			nodept->child[index] = new NODE;

			// ��x�S�Ẵ����o��������
			nodept->child[index]->bIndex = false;
			ZeroMemory(&nodept->child[index]->box, sizeof(BOX));
			for (int i = 0; i < 8; ++i) {
				nodept->child[index]->child[i] = nullptr;
			}
			nodept->child[index]->index.nIndex = 0;
			nodept->child[index]->index.pIndex = nullptr;
			nodept->child[index]->ObjList.clear();
		
			// �V�K�m�[�h�̗̈��ݒ�
			nodept->child[index]->box = testbox;
			// �V�K�m�[�h�̒��_�C���f�b�N�X����ݒ�
			nodept->child[index]->index = *pIndex;
		
			// ���_�C���f�b�N�X���i�[���[�N���
			//SAFE_DELETE_ARRAY(pIndex->pIndex);
			SAFE_DELETE(pIndex);
			// �ċA�Ăяo���ŐV�K�m�[�h�̕��������s
			MakeOctree(nodept->child[index], pVertex);
			// ��ԃ��x��-1
			m_nSpaceLevel--;
		}
		else
		{
			// ���_�f�[�^�������Ă��Ō�܂Ńm�[�h�쐬
			// ��ԃ��x�����{�P
			m_nSpaceLevel++;

			// �V�K�m�[�h���쐬
			nodept->child[index] = new NODE;
			
			// ��x�S�Ẵ����o��������
			nodept->child[index]->bIndex = false;
			ZeroMemory(&nodept->child[index]->box, sizeof(BOX));
			for (int i = 0; i < 8; ++i) {
				nodept->child[index]->child[i] = nullptr;
			}
			nodept->child[index]->index.nIndex = 0;
			nodept->child[index]->index.pIndex = nullptr;
			nodept->child[index]->ObjList.clear();
		
			// �V�K�m�[�h�̗̈��ݒ�
			nodept->child[index]->box = testbox;
			// ���_�C���f�b�N�X���i�[���[�N���
			SAFE_DELETE(pIndex);
			// �ċA�Ăяo���ŐV�K�m�[�h�̕��������s
			MakeOctree(nodept->child[index], pVertex);
			// ��ԃ��x��-1
			m_nSpaceLevel--;
		}
		
		// ���_�C���f�b�N�X���i�[���[�N���
		if (pIndex) {
			delete pIndex;
		}
		
	}


}

//---------------------------------------------------------------------------------------
// Octree���{�b�N�X�̃|���S���܂݃`�F�b�N
//---------------------------------------------------------------------------------------
bool Octree::CheckNodeBoxHit(BOX* boxpt, TAssimpVertex* pVertex, INDEX* pIndex, INDEX _Index)
{

	// �{�b�N�X���Ɉ�ł����_������΃m�[�h���쐬
	TAssimpVertex* pt;
	pt = pVertex;
	// �m�[�h���ɑ��݂��钸�_�̃C���f�b�N�X���ꎞ�i�[�p
	std::vector<UINT> Indexvector;
	
	/* 
	�C���f�b�N�X�����ɑ����_����A�e�m�[�h�̃{�b�N�X���ɒ��_�����݂���|���S����S�ē��肵�A
	�|���S���̒��_�̃C���f�b�N�X���e�m�[�h�Ɋi�[����
	*/
	for (UINT i = 0; i < _Index.nIndex; i += 3) 
	{
		// �|���S���̒��_���m�[�h�{�b�N�X���ɑ��݂��邩
		bool bIn = false;

		if (boxpt->x_min <= pt[_Index.pIndex[i + 0]].vPos.x && pt[_Index.pIndex[i + 0]].vPos.x < boxpt->x_max &&
			boxpt->y_min <= pt[_Index.pIndex[i + 0]].vPos.y && pt[_Index.pIndex[i + 0]].vPos.y < boxpt->y_max &&
			boxpt->z_min <= pt[_Index.pIndex[i + 0]].vPos.z && pt[_Index.pIndex[i + 0]].vPos.z < boxpt->z_max)
			bIn = true;
		if (!bIn) {
			if (boxpt->x_min <= pt[_Index.pIndex[i + 1]].vPos.x && pt[_Index.pIndex[i + 1]].vPos.x < boxpt->x_max &&
				boxpt->y_min <= pt[_Index.pIndex[i + 1]].vPos.y && pt[_Index.pIndex[i + 1]].vPos.y < boxpt->y_max &&
				boxpt->z_min <= pt[_Index.pIndex[i + 1]].vPos.z && pt[_Index.pIndex[i + 1]].vPos.z < boxpt->z_max)
				bIn = true;
		}
		if (!bIn) {
			if (boxpt->x_min <= pt[_Index.pIndex[i + 2]].vPos.x && pt[_Index.pIndex[i + 2]].vPos.x < boxpt->x_max &&
				boxpt->y_min <= pt[_Index.pIndex[i + 2]].vPos.y && pt[_Index.pIndex[i + 2]].vPos.y < boxpt->y_max &&
				boxpt->z_min <= pt[_Index.pIndex[i + 2]].vPos.z && pt[_Index.pIndex[i + 2]].vPos.z < boxpt->z_max)
				bIn = true;
		}

		/* 
		�m�[�h�{�b�N�X���Ɉ�ł����_�����݂��Ă����ꍇ�A���݂����S�Ă̒��_�C���f�b�N�X����
		�m�[�h���C���f�b�N�X���ɕۑ�����
		*/
		if (bIn)
		{
			Indexvector.push_back(_Index.pIndex[i + 0]);
			Indexvector.push_back(_Index.pIndex[i + 1]);
			Indexvector.push_back(_Index.pIndex[i + 2]);
		}
	}
	
	// vector���ɃC���f�b�N�X���i�[����Ă���Β��_�����݂��Ă���
	if (Indexvector.size() > 0) 
	{
		// �C���f�b�N�X���ۑ�
		pIndex->nIndex = Indexvector.size();
		// �m�[�h�����C���f�b�N�X�����������m��
		pIndex->pIndex = new UINT[Indexvector.size()];
		
		// �C���f�b�N�X�f�[�^�ۑ�
		for (UINT i = 0; i < Indexvector.size(); ++i)
		{
			pIndex->pIndex[i] = Indexvector[i];
		}

		return true;
	}
	else
	{
		return false;
	}
	
}

//---------------------------------------------------------------------------------------
// Octree�`��
//---------------------------------------------------------------------------------------
void Octree::DrawOctreeBox(NODE* nodept)
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	XMMATRIX mtxWorld, mtxScale, mtxTrans;
	
	// ���_�f�[�^��ݒ�
	mtxScale = XMMatrixScaling(
		nodept->box.x_max - nodept->box.x_min,
		nodept->box.y_max - nodept->box.y_min,
		nodept->box.z_max - nodept->box.z_min);
	mtxWorld = mtxScale;
	mtxTrans = XMMatrixTranslation(nodept->box.x_min, nodept->box.y_min, nodept->box.z_min);
	mtxWorld *= mtxTrans;
	XMStoreFloat4x4(m_meshBox.GetWorld(), mtxWorld);

	m_meshBox.SetDiffuse((0 < nodept->ObjList.size()) ? XMFLOAT4(1.0f, 0.0f, 1.0f, 0.5f) : XMFLOAT4(0.25f, 0.5f, 1.0f, 0.25f));
	m_meshBox.Draw();
	// �`���C���h�m�[�h�̃{�b�N�X�`��
	for (int i = 0; i < 8; i++) {
		if (nodept->child[i] && nodept->bIndex) {
			DrawOctreeBox(nodept->child[i]);
		}
	}
}

void Octree::DrawOctreeVector()
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// �`�悷��m�[�h���X�g��S�Đ������A�`��t���O���I�t�ɂȂ��Ă�����̂̓��X�g����Ȃ��AON�̂��͕̂`�悷��B
	for (auto itr = m_DrawNodesIndexVector.begin(); itr != m_DrawNodesIndexVector.end();)
	{
		NODE* pNode = m_NodesVector[*itr];
		if (pNode->ObjList.size() <= 0)
		{
			itr = m_DrawNodesIndexVector.erase(itr);
			continue;
		}
		else
		{
			XMMATRIX mtxWorld, mtxScale, mtxTrans;

			// ���_�f�[�^��ݒ�
			mtxScale = XMMatrixScaling(
				pNode->box.x_max - pNode->box.x_min,
				pNode->box.y_max - pNode->box.y_min,
				pNode->box.z_max - pNode->box.z_min);
			mtxWorld = mtxScale;
			mtxTrans = XMMatrixTranslation(pNode->box.x_min, pNode->box.y_min, pNode->box.z_min);
			mtxWorld *= mtxTrans;
			XMStoreFloat4x4(m_meshBox.GetWorld(), mtxWorld);

		
			bool bIn[2] = { false, false };
			XMFLOAT3 Diffuse = XMFLOAT3(0, 0, 0);
			for (auto NodeItr = pNode->ObjList.begin(); NodeItr != pNode->ObjList.end();)
			{
				CGameObj* pObj = *NodeItr;
				if (!bIn[0] && pObj->GetTag() == GOT_PLAYER)
				{
					// �v���C���[���o�^���ꂽ�m�[�h��F�t��
					Diffuse = XMFLOAT3(Diffuse.x, Diffuse.y, Diffuse.z + 1.0f);
					bIn[0] = true;
				}
				else if (!bIn[1] && pObj->GetTag() == GOT_ENEMY || pObj->GetTag() == GOT_BOSS)
				{
					// �G���o�^���ꂽ�m�[�h��F�t��
					Diffuse = XMFLOAT3(Diffuse.x + 1.0f, Diffuse.y, Diffuse.z);
					bIn[1] = true;
				}
				++NodeItr;
			}

			if (!bIn[0] && !bIn[1])
			{
				// �Y���̃I�u�W�F�N�g���o�^����Ă��Ȃ�
				++itr;
				continue;
			}

			// �F�ݒ�
			m_meshBox.SetDiffuse(XMFLOAT4(Diffuse.x, Diffuse.y, Diffuse.z, 0.5f));
			// �`��
			m_meshBox.Draw();
			++itr;
		}
	}
}

// �n�`���܂ލŏ��m�[�h�̂ݕ`��
void Octree::DrawOctreeLandVector()
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// �`�悷��m�[�h���X�g��S�Đ������A�`��t���O���I�t�ɂȂ��Ă�����̂̓��X�g����Ȃ��AON�̂��͕̂`�悷��B
	for (auto itr = m_LandDrawNodesIndexVector.begin(); itr != m_LandDrawNodesIndexVector.end();)
	{
		NODE* pNode = m_NodesVector[*itr];
		XMMATRIX mtxWorld, mtxScale, mtxTrans;

		// ���_�f�[�^��ݒ�
		mtxScale = XMMatrixScaling(
			pNode->box.x_max - pNode->box.x_min,
			pNode->box.y_max - pNode->box.y_min,
			pNode->box.z_max - pNode->box.z_min);
		mtxWorld = mtxScale;
		mtxTrans = XMMatrixTranslation(pNode->box.x_min, pNode->box.y_min, pNode->box.z_min);
		mtxWorld *= mtxTrans;
		XMStoreFloat4x4(m_meshBox.GetWorld(), mtxWorld);
		
		for (auto NodeItr = pNode->ObjList.begin(); NodeItr != pNode->ObjList.end();)
		{
			CGameObj* pObj = *NodeItr;
			if (pObj->GetTag() == GOT_PLAYER)
			{
				// �v���C���[���o�^���ꂽ�m�[�h��F�t��
				m_meshBox.SetDiffuse(XMFLOAT4(0.0f, 1.0f, 1.0f, 0.5f));
				break;
			}
			else
			{
				// �n�`���f�����o�^���ꂽ�m�[�h��F�t��
				m_meshBox.SetDiffuse(XMFLOAT4(0.0f, 1.0f, 0.0f, 0.5f));
			}
			++NodeItr;
		}
		
		// �`������s
		m_meshBox.Draw();
		++itr;
	}
}

// ���[�g�m�[�h�̕`��
void Octree::DrawOctreeRootNode()
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// ���[�g�m�[�h�̂ݕ`��
	m_RootNode;
	XMMATRIX mtxWorld, mtxScale, mtxTrans;

	// ���_�f�[�^��ݒ�
	mtxScale = XMMatrixScaling(
		m_RootNode.box.x_max - m_RootNode.box.x_min,
		m_RootNode.box.y_max - m_RootNode.box.y_min,
		m_RootNode.box.z_max - m_RootNode.box.z_min);
	mtxWorld = mtxScale;
	mtxTrans = XMMatrixTranslation(m_RootNode.box.x_min, m_RootNode.box.y_min, m_RootNode.box.z_min);
	mtxWorld *= mtxTrans;
	XMStoreFloat4x4(m_meshBox.GetWorld(), mtxWorld);

	m_meshBox.SetDiffuse(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f));

	// �`������s
	m_meshBox.Draw();
}

// �S�Ă̍ŏ��m�[�h�̕`��
void Octree::DrawOctreeAll()
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	for (auto itr = m_NodesVector.begin(); itr != m_NodesVector.end();)
	{
		XMMATRIX mtxWorld, mtxScale, mtxTrans;
		NODE* pNode = *itr;
		// ���_�f�[�^��ݒ�
		mtxScale = XMMatrixScaling(
			pNode->box.x_max - pNode->box.x_min,
			pNode->box.y_max - pNode->box.y_min,
			pNode->box.z_max - pNode->box.z_min);
		mtxWorld = mtxScale;
		mtxTrans = XMMatrixTranslation(pNode->box.x_min, pNode->box.y_min, pNode->box.z_min);
		mtxWorld *= mtxTrans;
		XMStoreFloat4x4(m_meshBox.GetWorld(), mtxWorld);

		m_meshBox.SetDiffuse(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f));

		// �`������s
		m_meshBox.Draw();

		++itr;
	}

	
}


//----- Octree �̕`��
void Octree::DrawOctree()
{
	if (m_bDrawObject) 
	{
		// �v���C���[�ƓG
		DrawOctreeVector();
	}
	if (m_bDrawLandOctree)
	{
		// �n�`���f��
		DrawOctreeLandVector();
	}
	if (m_bDrawRoot)
	{
		// ���[�g�m�[�h
		DrawOctreeRootNode();
	}
	if (m_bDrawAll)
	{
		// �S�Ă̍ŏ��m�[�h
		DrawOctreeAll();
	}
}

//---------------------------------------------------------------------------------------
// Octree�T��������
//---------------------------------------------------------------------------------------
void Octree::InitSearchTree(NODE* nodept)
{
	// �Z���N�g����Ă��Ȃ���ԂɃZ�b�g
	//nodept->OnSelect = false;		
	// �`���C���h�m�[�h�̏�Ԃ����Z�b�g
	//for (int i = 0; i < 8; i++) {
	//	// �`���C���h�m�[�h�̃Z���N�g��true�Ȃ�ċA�ő��s
	//	if (nodept->child[i] && nodept->child[i]->OnSelect) InitSearchTree(nodept->child[i]);
	//}
}

//---------------------------------------------------------------------------------------
// Octree�T��
//---------------------------------------------------------------------------------------
NODE* Octree::SearchTree(NODE* nodept, XMFLOAT3* point)
{
	//***********************************************************************************
	//  �����ɒT���|�C���g�i���� point�j���Ώۃm�[�h�̗̈�Ɋ܂܂�Ă����
	//  �m�[�h�� OnSelect �t���O���s�q�t�d�ɂȂ�悤�ȏ�����ǉ�����B
	//  �������`���C���h�i�q�j�������Ă���΁A�ċA�Ŏ��s���ē��l�̏�����
	//  ���[�܂ōs���B
	//***********************************************************************************
	NODE* pNode = nullptr;
	if (nodept->box.x_min <= point->x&&point->x < nodept->box.x_max &&
		nodept->box.y_min <= point->y&&point->y < nodept->box.y_max &&
		nodept->box.z_min <= point->z&&point->z < nodept->box.z_max)
	{
		// ���T���m�[�h�{�b�N�X���ɒT���|�C���g���܂܂��΁A�߂�l�X�V
		pNode = nodept;
		// �q�m�[�h���T��
		for (int i = 0; i < 8; ++i) {
			// �q�m�[�h�����݂���΍ċA�Ŏ��s
			if (nodept->child[i]) {
				pNode = SearchTree(nodept->child[i], point);
				if (pNode)
					return pNode;
				else
					continue;
			}
		}
		// �q�m�[�h������ or �q�m�[�h��S�ĒT���������A�T���|�C���g���܂ގq�m�[�h���Ȃ�����
		return pNode;
	}
	else
	{
		// �͈͊O�Ȃ�null��Ԃ�
		return nullptr;
	}
}

NODE* Octree::SearchVector(XMFLOAT3* point)
{
	// ��Ԕԍ������߂�
	XMFLOAT3 _point;
	_point.x = (point->x - (-m_fRootNodeScale / 2/*���[�g��Ԃ̍��[���W��X����*/))   / m_fMinNodeScale;
	_point.y = (point->y - (-m_fRootNodeScale / 2/*���[�g��Ԃ̉��[���W��Y����*/))   / m_fMinNodeScale;
	_point.z = (point->z - (-m_fRootNodeScale / 2/*���[�g��Ԃ̎�O�[���W��Z����*/)) / m_fMinNodeScale;

	// �}�C�i�X�̒l�ɂȂ�͂����Ȃ��̂Ń}�C�i�X�Ȃ�null��Ԃ�
	if (_point.x < 0 || _point.y < 0 || _point.z < 0)
	{
		return nullptr;
	}

	// �ŏ��m�[�h�̃C���f�b�N�X�ԍ������߂�
	int vectorIndex = Get3DMortonOrder((BYTE)_point.x, (BYTE)_point.y, (BYTE)_point.z);

	// ���[�g�m�[�h�O�̍��W�ɂ��Ă�null��Ԃ�
	if (vectorIndex < 0 || (int)m_NodesVector.size() <= vectorIndex)
	{
		return nullptr;
	}

	// vector���C���f�b�N�X�ԍ��ӏ��Ƀ|���S�������݂��邩
	if (m_NodesVector[vectorIndex]->bIndex)
	{
		return m_NodesVector[vectorIndex];
	}
	else
	{
		return nullptr;
	}
}

int Octree::GetVectorNumber(XMFLOAT3* point)
{
	// ��Ԕԍ������߂�
	XMFLOAT3 _point;
	_point.x = (point->x - (-m_fRootNodeScale / 2/*���[�g��Ԃ̍��[���W��X����*/))   / m_fMinNodeScale;
	_point.y = (point->y - (-m_fRootNodeScale / 2/*���[�g��Ԃ̉��[���W��Y����*/))   / m_fMinNodeScale;
	_point.z = (point->z - (-m_fRootNodeScale / 2/*���[�g��Ԃ̎�O�[���W��Z����*/)) / m_fMinNodeScale;

	// �}�C�i�X�̒l�ɂȂ�͂����Ȃ��̂Ń}�C�i�X�Ȃ�null��Ԃ�
	if (_point.x < 0 || _point.y < 0 || _point.z < 0)
	{
		return -1;
	}

	// �ŏ��m�[�h�̃C���f�b�N�X�ԍ������߂�
	int vectorIndex = Get3DMortonOrder((BYTE)_point.x, (BYTE)_point.y, (BYTE)_point.z);

	// ���[�g�m�[�h�O�̍��W�ɂ��Ă�null��Ԃ�
	if (vectorIndex < 0 || (int)m_NodesVector.size() <= vectorIndex)
	{
		return -1;
	}

	return vectorIndex;
}

bool Octree::EraseAndPushObjInNode(NODEINDEXWORK* pIndexWork, CGameObj* pObj)
{
	/* �Q�ƃm�[�h�̑I����@�ɂ���
	�����̒��S���W�𒆐S�Ƃ���OBB�����S�ɕ�ݍ��ށA�ŏ��m�[�h�̏W�܂�ō�闧���̂����߁A���̍ŏ��m�[�h�S�Ăɂ��ĎQ�Ƃ���
	�܂����g��OBB�̒��S����XYZ���ꂼ��̖ʂ܂ł̋��������߁A
	���ꂼ����ŏ��m�[�h�̑傫���Ŋ������l�{�P��
	�ŏI�I�ɎQ�Ƃ���ŏ��m�[�h�̏W�܂�ł���OBB�ɂ�����A���S�m�[�h�����������S����e�ʂ܂ł̃m�[�h���ł���B
	���̃m�[�h��  *          2             +     1
	�@�@�@�@�@    �@(���S��̔��Α���)      (���S��)
	���ŏ��m�[�h�̏W�܂�ł���OBB�̈�ӂ��\������ŏ��m�[�h�̐��ɂȂ�
	�����̐��l���g�p���Ăn�a�a�̒��S���܂ލŏ��m�[�h�𒆐S�Ƃ���ŏ��m�[�h�̏W����OBB�����߁A���̍\���ŏ��m�[�h��S�ď����m�[�h�Ƃ���
	�i���ʓI�ɏ�������\��������m�[�h�ƂȂ��Ă���j
	*/

	// �O�񏊑����Ă����m�[�h��ۑ�
	pIndexWork->OldIndex.clear();
	pIndexWork->OldIndex = pIndexWork->CurIndex;
	pIndexWork->CurIndex.clear();

	// OBB��XYZ���ꂼ��̖ʂ܂ł̋��������߂�
	XMFLOAT3 vOBBScl = pObj->GetBBox();
	XMFLOAT3 vObjScale = pObj->GetTransform()->GetScale();
	float fObjOBBScalX;
	float fObjOBBScalY;
	float fObjOBBScalZ;
	
	fObjOBBScalX = vOBBScl.x * vObjScale.x;
	fObjOBBScalY = vOBBScl.y * vObjScale.y;
	fObjOBBScalZ = vOBBScl.z * vObjScale.z;

	// �ŏ��m�[�h�W�܂�OBB�̒��S�m�[�h�������A���S����e�ʂ܂ł̃m�[�h��
	int nNodeNumX = (int)(fObjOBBScalX / m_fMinNodeScale + 1);
	int nNodeNumY = (int)(fObjOBBScalY / m_fMinNodeScale + 1);
	int nNodeNumZ = (int)(fObjOBBScalZ / m_fMinNodeScale + 1);

	// �e�������x�N�g��
	XMFLOAT4X4 mW = pObj->GetTransform()->GetWorld();
	XMVECTOR vecX = XMVector3Normalize(XMVectorSet(mW._11, mW._12, mW._13, 1.0f));
	XMVECTOR vecY = XMVector3Normalize(XMVectorSet(mW._21, mW._22, mW._23, 1.0f));
	XMVECTOR vecZ = XMVector3Normalize(XMVectorSet(mW._31, mW._32, mW._33, 1.0f));
	XMVECTOR vCenter = XMLoadFloat3(&pObj->GetCenter());
	XMMATRIX mWorld = XMMatrixTranslationFromVector(vCenter);
	mWorld *= XMLoadFloat4x4(&mW);
	XMStoreFloat4x4(&mW, mWorld);

	// �ŏ��m�[�h�W�܂藧���̂̍�����O�̍��W
	XMVECTOR vecBaceVertex = XMVectorSet(mW._41, mW._42, mW._43, 1.0f);
	vecBaceVertex +=
		- vecX * m_fMinNodeScale * (float)nNodeNumX
		- vecY * m_fMinNodeScale * (float)nNodeNumY
		- vecZ * m_fMinNodeScale * (float)nNodeNumZ;

	XMFLOAT3 vCheckPoint;
	// �ŏ��̃`�F�b�N�|�C���g�͍�����O�̒��_
	XMVECTOR vecCheckPoint = vecBaceVertex;

	// �ŏ��m�[�h�̏W�܂藧���̂̈�ӂ̃m�[�h��
	int nLoopNumX = nNodeNumX * 2 + 1;
	int nLoopNumY = nNodeNumY * 2 + 1;
	int nLoopNumZ = nNodeNumZ * 2 + 1;

	for (int k = 0; k < nLoopNumZ; ++k)
	{
		vecCheckPoint += vecZ * m_fMinNodeScale * (float)k;
		for (int j = 0; j < nLoopNumY; ++j)
		{
			vecCheckPoint += vecY * m_fMinNodeScale * (float)j;
			for (int i = 0; i < nLoopNumX; ++i)
			{
				vecCheckPoint += vecX * m_fMinNodeScale * (float)i;

				// ���߂��`�F�b�N�|�C���g�̍��W���珊���m�[�h�̃C���f�b�N�X�ԍ������߁A�ۑ�����
				XMStoreFloat3(&vCheckPoint, vecCheckPoint);
				int nIndexNum = GetVectorNumber(&vCheckPoint);

				// �L���łȂ��C���f�b�N�X�l��-1�ŗ�O����
				if (nIndexNum < 0)
				{
					vecCheckPoint -= vecX * m_fMinNodeScale * (float)i;
					continue;
				}
				pIndexWork->CurIndex.push_back(nIndexNum);

				// �f�o�b�O�p�ɕ`��t���O��On�ɂ���(�`��t���O��true == �o�^����Ă���I�u�W�F�N�g������̂ŕ`��vector�ɒǉ����Ȃ�)
				if (m_NodesVector[nIndexNum]->ObjList.size() <= 0)
				{
					// TODO : �d���`�F�b�N�i�����ԃv���C����u����Əd���ɂ��v�f���ی��Ȃ�������j
					m_DrawNodesIndexVector.push_back(nIndexNum);
				}
				vecCheckPoint -= vecX * m_fMinNodeScale * (float)i;
			}
			vecCheckPoint -= vecY * m_fMinNodeScale * (float)j;
		}
		vecCheckPoint -= vecZ * m_fMinNodeScale * (float)k;
	}



	// �`��m�[�h���X�g����d�����Ȃ�
	/*std::sort(m_DrawNodesIndexVector.begin(), m_DrawNodesIndexVector.end());
	m_DrawNodesIndexVector.erase(std::unique(m_DrawNodesIndexVector.begin(), m_DrawNodesIndexVector.end()), m_DrawNodesIndexVector.end());*/

	// OldIndex��CurIndex���r���AOldIndex�̒��ɂ���ACurIndex�ɖ����l���C���f�b�N�X�l�Ƃ���ŏ��m�[�h����ObjList���玩�g�ւ̃|�C���^���Ȃ��B
	bool bWork = false; // ���������true��
	for (unsigned int j = 0; j < pIndexWork->OldIndex.size(); ++j)
	{
		bWork = false;
		for (unsigned int i = 0; i < pIndexWork->CurIndex.size(); ++i)
		{
			if (pIndexWork->OldIndex[j] != pIndexWork->CurIndex[i])
			{
				continue;
			}
			else
			{
				// �����l�𔭌�
				bWork = true;
				break;
			}

		}
		if (!bWork)
		{
			// �ŏ��m�[�h���玩�g�ւ̃|�C���^���Ȃ�
			EraseObjInNode(pIndexWork->OldIndex[j], pObj);
			
		}
	}


	// CurIndex�ɂ���AOldIndex�ɖ����l���C���f�b�N�X�l�Ƃ���ŏ��m�[�h�Ɏ��g�ւ̃|�C���^��ǉ�����B
	for (unsigned int j = 0; j < pIndexWork->CurIndex.size(); ++j)
	{
		bWork = false;
		for (unsigned int i = 0; i < pIndexWork->OldIndex.size(); ++i)
		{
			if (pIndexWork->CurIndex[j] != pIndexWork->OldIndex[i])
			{
				continue;
			}
			else
			{
				// �����l�𔭌������̂Œǉ����Ȃ�
				bWork = true;
				break;
			}

		}
		if (!bWork)
		{
			// �ŏ��m�[�h�Ɏ��g�ւ̃|�C���^��ǉ�
			PushObjInNode(pIndexWork->CurIndex[j], pObj);
		}
	}

	return true;
}

NODE* Octree::GetNodeInVector(int nIndex)
{ 
	// �͈͊O
	if (nIndex < 0 || (int)m_NodesVector.size() <= nIndex)return nullptr;

	return m_NodesVector[nIndex]; 
}

// �ŏ��m�[�h�̃I�u�W�F�N�g�������X�g�ɒǉ�����
bool Octree::PushObjInNode(int nIndex, CGameObj* pObj)               
{
	// �����I�u�W�F�N�g�������m�[�h�ɕ�����o�^����邱�Ƃ�h���i�d���`�F�b�N�j
	bool bCheck = true;
	for (auto itr = m_NodesVector[nIndex]->ObjList.begin(); itr != m_NodesVector[nIndex]->ObjList.end();)
	{
		if (*itr == pObj)
		{
			// ���ɓo�^����Ă���
			bCheck = false;
			break;
		}

		++itr;
	}
	if (bCheck) {
		m_NodesVector[nIndex]->ObjList.push_back(pObj);
	}
	return bCheck;
}

// �ŏ��m�[�h�̃I�u�W�F�N�g�������X�g����Ȃ�
bool Octree::EraseObjInNode(int nIndex, CGameObj* pObj)              
{
	// �͈͊O
	if (nIndex < 0 || (int)m_NodesVector.size() < nIndex)return false;

	for (auto itr = m_NodesVector[nIndex]->ObjList.begin(); itr != m_NodesVector[nIndex]->ObjList.end();)
	{
		if (*itr != pObj)
		{
			++itr;
		}
		else
		{
			// �|�C���^����
			itr = m_NodesVector[nIndex]->ObjList.erase(itr);
			return true;
		}
	}

	// ���X�g���ɊY���|�C���^�����i���̏����œ����m�[�h���ɒT���|�C���g����������ꍇ�ɋN����j
	return false;
}

void Octree::SetCameraOctTree(CCamera* pCamera)
{
	m_meshBox.SetCamera(pCamera);
	m_meshSearchPoint.SetCamera(pCamera);
}

void Octree::SetLightOctTree(CLight* pLight)
{
	m_meshBox.SetLight(pLight);
	m_meshSearchPoint.SetLight(pLight);
}


DWORD Octree::BitSeparateFor3D(BYTE n)
{
	DWORD s = n;
	s = (s | s << 8) & 0x0000f00f;
	s = (s | s << 4) & 0x000c30c3;
	s = (s | s << 2) & 0x00249249;
	return s;
}

DWORD Octree::Get3DMortonOrder(BYTE x, BYTE y, BYTE z)  
{
	return BitSeparateFor3D(x) | BitSeparateFor3D(y) << 1 | BitSeparateFor3D(z) << 2;
}
//=======================================================================================
//	End of File
//=======================================================================================
