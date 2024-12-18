// �n�� [Land.cpp]
#include "Land.h"
#include "DebugProc.h"
#include "octree.h"
#include "Scene.h"
#include "Player.h"
#include "Input.h"
#include <list>


namespace{
	const float LIMIT_LANDANGLE = 45.0f; // ����Ζʊp�x�̌��E
	const float DYNAMIC_FRICTION = 0.85f;
}


// �R���X�g���N�^
CLand::CLand(CScene* pScene) : CModel(pScene)
{
	SetTag(GOT_LAND);
	SetObjID(EObjID::OI_LAND);
	m_pVertex = nullptr;
	m_nVertex = 0;
	m_pIndex = nullptr;
	m_nIndex = 0;

}

// �f�X�g���N�^
CLand::~CLand()
{
}

HRESULT CLand::Awake() // �ŏ��̏�����
{
	SetModel(MT_LAND);

	// ���_�z��A�C���f�b�N�X�z����擾���Ă���
	CAssimpModel* pModel = GetAssimp(MT_LAND);
	pModel->GetVertexCount(&m_nVertex, &m_nIndex);
	m_pVertex = new TAssimpVertex[m_nVertex];
	m_pIndex = new UINT[m_nIndex];
	pModel->GetVertex(m_pVertex, m_pIndex);


	// (�����ł����̂��H)
	// �I�N�g�c���[������
	// �c���[���������E�\�z
	Octree* pOctree = CScene::GetOctree();
	pOctree->InitOctree(m_pVertex, m_pIndex, m_nIndex);
	// �Ώۃt���O���N���A
	pOctree->InitSearchTree(pOctree->GetRootNode());
	// ���݂̒T���|�C���g�Ńc���[��T���A�Ώۃt���O��ݒ�
	pOctree->SearchTree(pOctree->GetRootNode(), pOctree->GetSearchPoint());

	// ���b�V���ɃJ���������C�g�Z�b�g
	pOctree->SetCameraOctTree(m_pScene->GetCameraManager()->GetCamera());
	pOctree->SetLightOctTree(m_pScene->GetLight());

	return S_OK;
}

// ������
HRESULT CLand::Init()
{
	return S_OK;
}

// �I������
void CLand::Fin()
{
	// �I�N�g�c���[�폜
	Octree* pOctree = CScene::GetOctree();
	pOctree->DeleteOctree(pOctree->GetRootNode());
	pOctree->ClearVector();

	CModel::Fin();

	// ���_�z��A�C���f�b�N�X�z��̉��
	SAFE_DELETE_ARRAY(m_pIndex);
	SAFE_DELETE_ARRAY(m_pVertex);
}

void CLand::Update()  // �X�V����
{

	Octree* pOctree = CScene::GetOctree();
	if (CInput::GetKeyTrigger(VK_O))
	{
		pOctree->SetDrawObjectFlg(!pOctree->GetDrawObjectFlg());
		
	}
	if (CInput::GetKeyTrigger(VK_P))
	{
		pOctree->SetDrawLandOctreeFlg(!pOctree->GetDrawLandOctreeFlg());
	
	}
	if (CInput::GetKeyTrigger(VK_I))
	{
		pOctree->SetDrawRootNodeFlg(!pOctree->GetDrawRootNodeFlg());

	}
	if (CInput::GetKeyTrigger(VK_U))
	{
		pOctree->SetDrawAllNodeFlg(!pOctree->GetDrawAllNodeFlg());

	}
	
	
	//CModel::Update();
}

void CLand::Draw()
{

	CModel::Draw();	
	
	
	// Octree�`��
	m_pScene->GetLight()->SetEnable(false);
	Octree* pOctree = CScene::GetOctree();
	pOctree->DrawOctree();
	m_pScene->GetLight()->SetEnable(true);
}

bool CLand::Collision(XMFLOAT3 vPos, XMFLOAT3 vP0, XMFLOAT3 vP1, XMFLOAT3* pX, XMFLOAT3* pN)
{
	/*
	�n�`�̃I�N�c���[�̒�����v���C���[�̍��W����������m�[�h + ���̎��� = �v27�m�[�h��T�����A
	���̃m�[�h�Ɋ܂܂��|���S���ɂ��ă��C�ɂ�铖���蔻����s��
	*/
	NODE* pNodeArray[27];
	XMFLOAT3 _vPos = vPos;
	// ��������m�[�h + ���̓m�[�h��z��Ɋi�[���Ă���(�|���S�����܂܂�Ă��Ȃ��Ȃ�null���i�[�����)
	Octree* pOctree = CScene::GetOctree();
	int nArrayNum = 0;
	float fMinNodeScaleX = pOctree->GetMinNodeScale();
	float fMinNodeScaleY = fMinNodeScaleX;
	float fMinNodeScaleZ = fMinNodeScaleX;
	for (int k = 0, nCodeZ = -1; k < 3; ++k, ++nCodeZ)
	{
		_vPos.z = vPos.z + fMinNodeScaleZ * nCodeZ;
		for (int j = 0, nCodeY = -1; j < 3; ++j, ++nCodeY)
		{
			_vPos.y = vPos.y + fMinNodeScaleY * nCodeY;
			for (int i = 0, nCodeX = -1; i < 3; ++i, ++nCodeX)
			{
				_vPos.x = vPos.x + fMinNodeScaleX * nCodeX;

				// �z��Ɋi�[
				pNodeArray[nArrayNum] = pOctree->SearchVector(&_vPos);
				++nArrayNum;
			}
		}
	}

	// ���C�i�����j
	XMFLOAT3 vW(vP1.x - vP0.x, vP1.y - vP0.y, vP1.z - vP0.z);
	for (int j = 0; j < 27; ++j)
	{
		if (pNodeArray[j]) {
			// �S�Ă̎O�p�`�ɂ���
			for (UINT i = 0; i < pNodeArray[j]->index.nIndex; i += 3) {
				// �O�p�`�̒��_���W
				XMFLOAT3& V0 = m_pVertex[pNodeArray[j]->index.pIndex[i + 0]].vPos;
				XMFLOAT3& V1 = m_pVertex[pNodeArray[j]->index.pIndex[i + 1]].vPos;
				XMFLOAT3& V2 = m_pVertex[pNodeArray[j]->index.pIndex[i + 2]].vPos;
				// �ӂɑΉ�����x�N�g��
				XMFLOAT3 V0V1(V1.x - V0.x, V1.y - V0.y, V1.z - V0.z);
				XMFLOAT3 V1V2(V2.x - V1.x, V2.y - V1.y, V2.z - V1.z);
				XMFLOAT3 V2V0(V0.x - V2.x, V0.y - V2.y, V0.z - V2.z);

				// �O�p�`(=����)�̖@���x�N�g�������߂�
				XMVECTOR N = XMVector3Normalize(
					XMVector3Cross(XMLoadFloat3(&V0V1),
						XMLoadFloat3(&V1V2)));

				// t�̎�(t=(N�E(V0-P0))/(N�EW))�̕�������߂�
				float base;
				XMStoreFloat(&base, XMVector3Dot(N,
					XMLoadFloat3(&vW)));
				if (base == 0.0f)
					continue;	// �ʂƃ��C�����s

				// ���q�����߂�
				XMFLOAT3 P0V0(V0.x - vP0.x, V0.y - vP0.y, V0.z - vP0.z);
				float dot;
				XMStoreFloat(&dot, XMVector3Dot(N,
					XMLoadFloat3(&P0V0)));
				// t�����_�����߂�
				float t = dot / base;

				if (t < 0.0f)
					continue;	// ��_���n�_�̌��
				if (t > 1.0f)
					continue;   // ��_���I�_������


				XMFLOAT3 X(vP0.x + t * vW.x,
					vP0.y + t * vW.y, vP0.z + t * vW.z);
				// ���O����
				XMFLOAT3 V0X(X.x - V0.x, X.y - V0.y, X.z - V0.z);
				XMStoreFloat(&dot, XMVector3Dot(N,
					XMVector3Cross(XMLoadFloat3(&V0V1),
						XMLoadFloat3(&V0X))));
				if (dot < 0.0f)
					continue;
				XMFLOAT3 V1X(X.x - V1.x, X.y - V1.y, X.z - V1.z);
				XMStoreFloat(&dot, XMVector3Dot(N,
					XMVector3Cross(XMLoadFloat3(&V1V2),
						XMLoadFloat3(&V1X))));
				if (dot < 0.0f)
					continue;
				XMFLOAT3 V2X(X.x - V2.x, X.y - V2.y, X.z - V2.z);
				XMStoreFloat(&dot, XMVector3Dot(N,
					XMVector3Cross(XMLoadFloat3(&V2V0),
						XMLoadFloat3(&V2X))));
				if (dot < 0.0f)
					continue;


				// ������
				if (pX) *pX = X;
				if (pN) XMStoreFloat3(pN, N);

				return true;// �������Ă���
			}
		}
	}

	return false;	// �������Ă��Ȃ�
}


bool CLand::Collision(CGameObj* pObj, XMFLOAT3 vPX, XMFLOAT3 vPZ, XMFLOAT3* pX, XMFLOAT3* pN)
{
	bool bLand = false;
	XMFLOAT3 vSlopeVec = XMFLOAT3(0,0,0); // �Ζʂɂ̂����㖀�C�͂��v�Z������̉^���x�N�g��
	/*
	�n�`�̃I�N�c���[�̒�����v���C���[�̍��W����������m�[�h + ���̎��� = �v27�m�[�h��T�����A
	���̃m�[�h�Ɋ܂܂��|���S���ɂ��ă��C�ɂ�铖���蔻����s��
	*/
	std::vector<int> NodeIndexVector = pObj->GetIndexWork()->CurIndex;
	// ��������m�[�h + ���̓m�[�h��z��Ɋi�[���Ă���(�|���S�����܂܂�Ă��Ȃ��Ȃ�null���i�[�����)
	Octree* pOctree = CScene::GetOctree();
	
	XMFLOAT3 vP0 = XMFLOAT3(
		pObj->GetTransform()->GetPos().x + pObj->GetCenter().x,
		pObj->GetTransform()->GetPos().y + pObj->GetCenter().y,
		pObj->GetTransform()->GetPos().z + pObj->GetCenter().z
	);
	XMFLOAT3 vP1 = XMFLOAT3(
		pObj->GetTransform()->GetPos().x,
		pObj->GetTransform()->GetPos().y,
		pObj->GetTransform()->GetPos().z
	);
	
	// ���C�i�����j
	XMFLOAT3 vW(
		vP1.x - vP0.x,
		vP1.y - vP0.y,
		vP1.z - vP0.z);

	// ������OBB�̒��S�_����i�s�����Ƀx�N�g�����΂��A�Փ˂����|���S����̓_�܂ł̋���
	// (���ꂪ�ŏ��̒l�̃|���S���̉����o���x�N�g����Ԃ�)
	float fCenterToFacePointDistance;
	XMStoreFloat(&fCenterToFacePointDistance, XMVector3Length(XMLoadFloat3(&vW))); // �ő�l�ŏ�����
	
	XMVECTOR vecCenter = XMLoadFloat3(&vP0);	// �����̂n�a�a�̒��S�_

	for (auto itr = NodeIndexVector.begin(); itr != NodeIndexVector.end();)
	{
		NODE* pNode = pOctree->GetNodeInVector(*itr);
		if (pNode) {
			
			// *******************�n�`�n���f���Ƃ̓����蔻��************************
			// �S�Ă̎O�p�`�ɂ���
			for (UINT i = 0; i < pNode->index.nIndex; i += 3) {
				// �O�p�`�̒��_���W
				XMFLOAT3& V0 = m_pVertex[pNode->index.pIndex[i + 0]].vPos;
				XMFLOAT3& V1 = m_pVertex[pNode->index.pIndex[i + 1]].vPos;
				XMFLOAT3& V2 = m_pVertex[pNode->index.pIndex[i + 2]].vPos;
				// �ӂɑΉ�����x�N�g��
				XMFLOAT3 V0V1(V1.x - V0.x, V1.y - V0.y, V1.z - V0.z);
				XMFLOAT3 V1V2(V2.x - V1.x, V2.y - V1.y, V2.z - V1.z);
				XMFLOAT3 V2V0(V0.x - V2.x, V0.y - V2.y, V0.z - V2.z);

				// �O�p�`(=����)�̖@���x�N�g�������߂�
				XMVECTOR N = XMVector3Normalize(
					XMVector3Cross(XMLoadFloat3(&V0V1),
						XMLoadFloat3(&V1V2)));


				// t�̎�(t=(N�E(V0-P0))/(N�EW))�̕�������߂�
				float base;
				XMStoreFloat(&base, XMVector3Dot(N,
					XMLoadFloat3(&vW)));
				if (base == 0.0f)
					continue;	// �ʂƃ��C�����s

				// ���q�����߂�
				XMFLOAT3 P0V0(V0.x - vP0.x, V0.y - vP0.y, V0.z - vP0.z);
				float dot;
				XMStoreFloat(&dot, XMVector3Dot(N,
					XMLoadFloat3(&P0V0)));
				// t�����_�����߂�
				long double t = (long double)(dot / base);

				if (t < 0.0f)
					continue;	// ��_���n�_�̌��
				if (t > 1.0f)
					continue;   // ��_���I�_������


				XMFLOAT3 X(
					(float)(vP0.x + t * vW.x),
					(float)(vP0.y + t * vW.y),
					(float)(vP0.z + t * vW.z));

				// ���O����
				XMFLOAT3 V0X(X.x - V0.x, X.y - V0.y, X.z - V0.z);
				XMStoreFloat(&dot, XMVector3Dot(N,
					XMVector3Cross(XMLoadFloat3(&V0V1),
						XMLoadFloat3(&V0X))));
				if (dot < 0.0f)
					continue;
				XMFLOAT3 V1X(X.x - V1.x, X.y - V1.y, X.z - V1.z);
				XMStoreFloat(&dot, XMVector3Dot(N,
					XMVector3Cross(XMLoadFloat3(&V1V2),
						XMLoadFloat3(&V1X))));
				if (dot < 0.0f)
					continue;
				XMFLOAT3 V2X(X.x - V2.x, X.y - V2.y, X.z - V2.z);
				XMStoreFloat(&dot, XMVector3Dot(N,
					XMVector3Cross(XMLoadFloat3(&V2V0),
						XMLoadFloat3(&V2X))));
				if (dot < 0.0f)
					continue;



				// ������OBB���S�_����̋������ł��߂����b�V���𓱂�
				float fDistance;
				XMStoreFloat(&fDistance, XMVector3Length(XMVectorSet(X.x, X.y, X.z, 1.0f) - vecCenter));
				if (fCenterToFacePointDistance < fDistance)
				{
					continue;
				}
				else
				{
					fCenterToFacePointDistance = fDistance;

					// �Ζʊp�x���傫���ƎΖʕ����ɂ��藎����
					float fLandAngle;
					XMStoreFloat(&fLandAngle, XMVector3AngleBetweenNormals(-XMVector3Normalize(XMLoadFloat3(&vW)), N));
					if (XMConvertToRadians(LIMIT_LANDANGLE) < fLandAngle)
					{
						// �Ζʊp�x
						float fSlopeAngle;
						XMStoreFloat(&fSlopeAngle, XMVector3AngleBetweenNormals(XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f), -N));
						// �Ζʂ̉������P�ʃx�N�g��
						XMVECTOR vecSlopeX = XMVector3Cross(XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f), N);
						// �Ζʉ������P�ʃx�N�g��
						XMVECTOR vecSlopeUnder = XMVector3Normalize(XMVector3Cross(vecSlopeX, -N));

						// �^����
						float fUnderLength;
						fUnderLength = GRAVITY_SCALE * sinf(fSlopeAngle);
						// ���C��
						float fFriction;
						fFriction = DYNAMIC_FRICTION * GRAVITY_SCALE * cosf(fSlopeAngle);
						// �^���ʌ���
						XMStoreFloat3(&vSlopeVec, vecSlopeUnder * fUnderLength - vecSlopeUnder * fFriction);



						// ������
						if (pX) *pX = X;
						if (pN) XMStoreFloat3(pN, N);
						bLand = true;
					}
					else
					{
						

						// ������
						if (pX) *pX = X;
						if (pN) XMStoreFloat3(pN, N);
						bLand = true;
					}
				}
			}
		}
		++itr;
	}

	if (bLand)
	{
		// �Ζʉ^���x�N�g�������Z
		// �^���ʂ������x�ɉ��Z
		pObj->GetTransform()->SetCurAccel(
			XMFLOAT3(
				vSlopeVec.x + pObj->GetTransform()->GetCurAccel().x,
				vSlopeVec.y + pObj->GetTransform()->GetCurAccel().y,
				vSlopeVec.z + pObj->GetTransform()->GetCurAccel().z
			));
	}

	return bLand;	// �������Ă��Ȃ�
}