//=======================================================================================
//
//	  Octree�֘A���W���[��
//
//=======================================================================================
#pragma once
#include "main.h"
#include "AssimpModel.h"
#include "mesh.h"
#include "Camera.h"
#include "Light.h"
#include <vector>
#include <list>

//-------- �\����
struct BOX {					// �̈�
	float	x_min, x_max;			// �w�����@�ŏ��l�A�ő�l
	float	y_min, y_max;			// �x�����@�ŏ��l�A�ő�l
	float	z_min, z_max;			// �y�����@�ŏ��l�A�ő�l
};

struct INDEX {
	UINT* pIndex;               // �|���S���̒��_�����i�[���邽�߂̃������̈�m�ۗp 
	UINT  nIndex;               // �m�[�h���̐�������C���f�b�N�X��
};


struct NODE {					        // �m�[�h
	NODE	*child[8];			        // �q�m�[�h�i�W���j
	BOX		box;				        // �̈�
								        
	INDEX   index;                      // �̈���̃|���S���̒��_�C���f�b�N�X���
	bool    bIndex;                     // �̈���Ƀ|���S�������݂��邩
	//bool	OnSelect;				    // �Z���N�g���t���O

	std::list<CGameObj*>   ObjList;     // �m�[�h�ɓo�^����Ă���I�u�W�F�N�g�̃��X�g
};

struct NODEINDEXWORK {
	std::vector<int> OldIndex;
	std::vector<int> CurIndex;
};

class Octree
{
private:
	NODE		m_RootNode;			     // ���[�g�m�[�h		     
	float       m_fRootNodeScale;        // ���[�g�m�[�h�̑傫��
	float       m_fMinNodeLimit;         // ���[�g�m�[�h�̍ŏ����E
	float       m_fMinNodeScale;         // �ŏ��m�[�h�̑傫��
	int         m_nSpaceLevel;           // ��ԃ��x��(���[�g��0)
	std::vector<NODE*> m_NodesVector;    // �ŏ��m�[�h�̃|�C���^vector

	

	// �f�o�b�O�p�����o�ϐ�		   	     
	std::vector<int> m_DrawNodesIndexVector;        // �`�悷��ŏ��m�[�h�̃C���f�b�N�Xvector�i�I�u�W�F�N�g���o�^����Ă���m�[�h�̂݁j
	std::vector<int> m_LandDrawNodesIndexVector;    // �`�悷��ŏ��m�[�h�̃C���f�b�N�Xvector�i�n�`�|���S�����܂ރm�[�h�̂݁j
	bool		m_bDrawObject;		     // Octree �`��t���O(�I�u�W�F�N�g���o�^����Ă���m�[�h)
	bool		m_bDrawLandOctree;		 // Octree �`��t���O�i�n�`�|���S�������݂���m�[�h�j
	bool		m_bDrawRoot;		 // Octree �`��t���O�i���[�g�m�[�h�j
	bool		m_bDrawAll;		     // Octree �`��t���O�i�S�Ă̍ŏ��m�[�h�j
	XMFLOAT3	m_SearchPoint;		     // �T���|�C���g
	CMesh		m_meshBox;               // Octree �`��p���b�V��
	CMesh		m_meshSearchPoint;       // �T���|�C���g�������郁�b�V��
private:
	void MakeLineVertex();                           // Octree&�T���|�C���g�`��p���_�쐬
	DWORD BitSeparateFor3D(BYTE n);                  // 3�o�C�g���ƂɊԊu���J����֐�
	DWORD Get3DMortonOrder(BYTE x, BYTE y, BYTE z);  // 8���؃��[�g�������Z�o�֐�
public:
	Octree();
	~Octree();


	//-------- �֐��v���g�^�C�v
	void InitOctree(TAssimpVertex* pVertex, UINT* pIndex, UINT nIndex);
	void DeleteOctree(NODE* nodept);
	void ClearVector();                                  // �ŏ��m�[�hvector���N���A
	void MakeOctree(NODE* nodept, TAssimpVertex* pVertex);
	bool CheckNodeBoxHit(BOX *boxpt, TAssimpVertex* pVertex, INDEX* pIndex, INDEX _Index);
	void DrawOctreeBox(NODE* nodept);
	void DrawOctreeVector();
	void DrawOctreeLandVector();
	void DrawOctreeRootNode();
	void DrawOctreeAll();
	void DrawOctree();
	void InitSearchTree(NODE* nodept);
	NODE* SearchTree(NODE* nodept, XMFLOAT3* point);               // �����ؓ������[�g���珇�ɂ��ǂ菊���m�[�h�����o��
	NODE* SearchVector(XMFLOAT3* point);                           // ���`�����ؓ����珊���m�[�h�����o��
	int   GetVectorNumber(XMFLOAT3* point);                        // ���W����������ŏ��m�[�h��vector�ɂ�����C���f�b�N�X�ԍ����擾����
	bool  EraseAndPushObjInNode(NODEINDEXWORK* , CGameObj*);       // �ŏ��m�[�h�̃I�u�W�F�N�g�������X�g�ɒǉ��ƁA�O�񏊑����Ă����m�[�h����Ȃ�
	bool  PushObjInNode(int, CGameObj*);                           // �ŏ��m�[�h�̃I�u�W�F�N�g�������X�g�ɒǉ�����
	bool  EraseObjInNode(int, CGameObj*);                          // �ŏ��m�[�h�̃I�u�W�F�N�g�������X�g����Ȃ�
	NODE* GetRootNode() { return &m_RootNode; }
	NODE* GetNodeInVector(int nIndex);
	float GetMinNodeScale() { return m_fMinNodeScale; }
	void SetSearchPoint(XMFLOAT3 vPoint) { m_SearchPoint = vPoint; }
	XMFLOAT3* GetSearchPoint() { return &m_SearchPoint; }
	void SetDrawObjectFlg(bool bflg) { m_bDrawObject = bflg; }
	bool GetDrawObjectFlg() { return m_bDrawObject; }
	void SetDrawLandOctreeFlg(bool bflg) { m_bDrawLandOctree = bflg; }
	bool GetDrawLandOctreeFlg() { return m_bDrawLandOctree; }
	void SetDrawRootNodeFlg(bool bflg) { m_bDrawRoot = bflg; }
	bool GetDrawRootNodeFlg() { return m_bDrawRoot; }
	void SetDrawAllNodeFlg(bool bflg) { m_bDrawAll = bflg; }
	bool GetDrawAllNodeFlg() { return m_bDrawAll; }

	void SetCameraOctTree(CCamera* pCamera);
	void SetLightOctTree(CLight* pLight);

	
};