//=============================================================================
//
// ���f�� �N���X���� [Model.cpp]
// Author : KAZUKI KURA
//
//=============================================================================
#include "Model.h"
#include "Scene.h"
#include "Input.h"
#include "DebugProc.h"
#include "EffekseerManager.h"


// 3D���f�� �t�@�C����
namespace {
	LPCSTR g_pszModelPath[MAX_MODEL] = {
		"data/model/Player/Paladin.fbx",// �v���C���[   
		"data/model/Mutant/Mutant.fbx",            // �~���[�^���g���f��
		"data/model/Wizard/Wizard.fbx",            // ���@�g�����f��
		"data/model/Maria/Maria.fbx",              // �}���A���f��
		"data/model/sphere.fbx",                   // �G���́i��~�j���f��
		"data/model/sky.fbx",                      // �X�J�C �h�[��
		"data/model/Terrian/Terrian.fbx",        // �n�ʃ��f��
		"data/model/cube.fbx",                     // �G�ꂽ�玀�ʃL���[�u
		"data/model/WallRock/WallRock.fbx",        // �⃂�f��
		"data/model/Cliff/Sedimentary.fbx", // �R���f��
	};
}

// �ÓI�����o
CAssimpModel* CModel::m_pModels[MAX_MODEL] = { nullptr };

// �R���X�g���N�^
CModel::CModel(CScene* pScene) : CGameObj(pScene)
{
	m_pModel = nullptr;
	m_nDethEffectTimer = 0;
	m_ModelType = MAX_MODEL;
	m_bGround = false;
	m_pLand = nullptr;
}

// �f�X�g���N�^
CModel::~CModel()
{
	/*if (m_pModel) {
		m_pModel->Release();
		delete m_pModel;
		m_pModel = nullptr;
	}*/
}

// �S���f���Ǎ�
HRESULT CModel::LoadAll()
{
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDC = GetDeviceContext();

	// Assimp�p�V�F�[�_������
	if (!CAssimpModel::InitShader(pDevice))
		return E_FAIL;

	// �S���f���Ǎ�
	HRESULT hr = S_OK;
	for (int i = 0; i < MAX_MODEL; ++i) {
		SAFE_DELETE(m_pModels[i]);
		
		
		m_pModels[i] = new CAssimpModel();
		if (!m_pModels[i]->Load(pDevice, pDC, g_pszModelPath[i])) {
			hr = E_FAIL;
			break;
		}
	}
	return hr;
}

// �S���f�����
void CModel::ReleaseAll()
{
	// �S���f�����
	for (int i = 0; i < MAX_MODEL; ++i) {
		if (m_pModels[i]) {
			m_pModels[i]->Release();
			delete m_pModels[i];
			m_pModels[i] = nullptr;
		}
	}

	// Assimp�p�V�F�[�_�I������
	CAssimpModel::UninitShader();
}

// �ŏ��̏�����
HRESULT CModel::Awake()
{
	return CGameObj::Awake();
}

// ������
HRESULT CModel::Init()
{
	// �n�`���f���|�C���^
	m_pLand = (CLand*)m_pScene->Find(GOT_LAND);

	HRESULT hr = CGameObj::Init();
	if (FAILED(hr))
		return hr;

	return hr;
}

// �I������
void CModel::Fin()
{
	CGameObj::Fin();
}

// �X�V
void CModel::Update()
{
	CGameObj::Update();
}

// �`��
void CModel::Draw()
{
	if (!m_bVisible) return;
	if (m_pModel) {
	
		ID3D11DeviceContext* pDC = GetDeviceContext();
		m_pModel->Draw(pDC, m_Transform.GetWorld(), eOpacityOnly);
	}
}

// �����������`��
void CModel::DrawAlpha()
{
	if (m_bVisible) {
		if (m_pModel) {
			ID3D11DeviceContext* pDC = GetDeviceContext();
			m_pModel->Draw(pDC, m_Transform.GetWorld(), eTransparentOnly);
		}
		CGameObj::DrawAlpha();
	}

}

// ���f���ݒ�
void CModel::SetModel(EModelType model)
{
	if (model < 0 || model >= MAX_MODEL) {
		m_pModel = nullptr;
		m_ModelType = MAX_MODEL;
	}
	else {
		m_pModel = m_pModels[model];
		m_ModelType = model;
	}
	
	// �J�����ƌ�����ݒ�
	if (m_pModel && m_pScene) {
		m_pModel->SetCamera(m_pScene->GetCameraManager()->GetCamera());
		m_pModel->SetLight(m_pScene->GetLight());
	}
	// ���E���擾
	if (m_pModel) {
		SetCenter(m_pModel->GetCenter());
		SetBBox(m_pModel->GetBBox()); 
		SetRadius(m_pModel->GetRadius());
	}
	else {
		SetCenter(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetBBox(XMFLOAT3(0.5f, 0.5f, 0.5f));
		SetRadius(0.5f);
	}
}



// Assimp���f���擾
CAssimpModel* CModel::GetAssimp(EModelType model)
{
	if (model < 0 || model >= MAX_MODEL)
		return nullptr;
	return m_pModels[model];
}

// �{�[���̌��݂̃��[���h�}�g���N�X���擾
XMFLOAT4X4 CModel::GetBoneGlobalMatrix(std::string BoneName)
{
	return m_pModel->GetBoneGlobalMatrix(BoneName.c_str());
}

void CModel::StartHitMotion()
{
	SetState(CharaState::HitBack);
	m_dAnimTime = 0;
}

void CModel::StartDeath()// ���S���o�J�n(GameObjId����G�t�F�N�g�������I��)
{
	// ���o���s��
	m_bVisible = false;
	int nEfkHandle = GetTag();

	switch (nEfkHandle)
	{
	case		GOT_ENEMY:         // �G���S���̔���
		m_nDethEffectTimer = 100;
		nEfkHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(DEATH_EXPLOSION_ENEMY_EFK), false, TIMER_DEATH_EXPLOSION_ENEMY_EFK);
		// ���S��
		CSound::Play(SE_EXPLOSION);
		break;
	case		GOT_BOSS:         // �{�X���S���̔���
		m_nDethEffectTimer = 100;
		nEfkHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(DEATH_EXPLOSION_ENEMY_EFK), false, TIMER_DEATH_EXPLOSION_ENEMY_EFK);

		break;
	case		GOT_PLAYER:         // �v���C���[���S���̔���
		m_nDethEffectTimer = 120;
		nEfkHandle = CEffekseerManager::Play(CEffekseerManager::GetPath(DEATH_EXPLOSION_PLAYER_EFK), false, TIMER_DEATH_EXPLOSION_PLAYER_EFK);

		break;
	default:
		// Tag�ɑΉ�����Efk����
		m_nDethEffectTimer = 0;
		return;
	}

	CEffekseerManager::SetMatrix(nEfkHandle, false, m_Transform.GetWorld());
}



// �Փ˂��������OBB�̊O���ɉ����o��
std::vector<CGameObj*> CModel::CollisionOBBPlane(float* fAngle,XMFLOAT3* newPos)            // �Փ˂��������OBB�̊O���ɉ����o��
{
	std::vector<CGameObj*> vectorColObj = CGameObj::CollisionOBB();// �Փ˂����I�u�W�F�N�g

	if (0 < vectorColObj.size())
	{
		XMFLOAT3 vForce = XMFLOAT3(0, 0, 0); // �����o���x�N�g��
		float fNormalAngle = XM_PI;
		for (unsigned int i = 0; i < vectorColObj.size(); ++i)
		{
			// �����OBB
			CBox* pColBox = vectorColObj[i]->GetBBOXInstance();
			// OBB�擾�ł�����
			if (!pColBox)continue;
			// �g���K�[���肩
			if (pColBox->GetTrigger())continue;

			// �����OBB���_���
			VERTEX_3D* pColVertex = pColBox->GetVertex();
			// �����OBB�C���f�b�N�X���
			UINT* pColIndex = pColBox->GetIndex();
			// �����OBB�C���f�b�N�X��
			UINT nColIndexNum = pColBox->GetIndexNum();
			// ����̃��[���h�s��
			XMMATRIX mColWorld = XMLoadFloat4x4(&vectorColObj[i]->GetTransform()->GetWorld());
			// ����̃��b�V���̍��W�ϊ��p���[���h�s��
			XMMATRIX meshWorld = XMMatrixTranslationFromVector(XMLoadFloat3(&vectorColObj[i]->GetCenter()));
			meshWorld *= mColWorld;


			// �����̒��S���瑊��̒��S�ւ̃x�N�g�������߂�
			// ����̒��S
			XMFLOAT3 vColObjCenter; 
			XMStoreFloat3(&vColObjCenter, XMVector3TransformCoord(XMLoadFloat3(&vectorColObj[i]->GetCenter()), mColWorld));
			// �����̒��S
			XMFLOAT3 vCenter; 
			XMStoreFloat3(&vCenter, XMVector3TransformCoord(XMLoadFloat3(&m_vCenter), XMLoadFloat4x4(&m_Transform.GetWorld())));
			// ���S�ԃx�N�g��
			//XMFLOAT3 vCenterToCenter = XMFLOAT3(vCenter.x - vColObjCenter.x, vCenter.y - vColObjCenter.y, vCenter.z - vColObjCenter.z);
			XMFLOAT3 vCenterToCenter = XMFLOAT3(vColObjCenter.x - vCenter.x, vColObjCenter.y - vCenter.y, vColObjCenter.z - vCenter.z);

			// ********���S�ԃx�N�g���ɂ��Ă̓����蔻��********
			for (unsigned int i = 0; i < nColIndexNum; i += 3) {
				// �����OBB�̃��b�V���̒��_���W
				XMFLOAT3 V0;
				XMFLOAT3 V1;
				XMFLOAT3 V2;
				XMStoreFloat3(&V0, XMVector3TransformCoord(XMLoadFloat3(&pColVertex[pColIndex[i + 0]].vtx), meshWorld));
				XMStoreFloat3(&V1, XMVector3TransformCoord(XMLoadFloat3(&pColVertex[pColIndex[i + 1]].vtx), meshWorld));
				XMStoreFloat3(&V2, XMVector3TransformCoord(XMLoadFloat3(&pColVertex[pColIndex[i + 2]].vtx), meshWorld));

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
					XMLoadFloat3(&vCenterToCenter)));
				if (base == 0.0f)
					continue;	// �ʂƃ��C�����s

				// ���q�����߂�
				XMFLOAT3 P0V0(
					V0.x - vCenter.x,
					V0.y - vCenter.y,
					V0.z - vCenter.z);
				float dot;
				XMStoreFloat(&dot, XMVector3Dot(N,
					XMLoadFloat3(&P0V0)));
				// t�����_�����߂�
				float t = dot / base;

				if (t <= 0.0f)
					continue;	// ��_���n�_�̌��
				if (t > 1.0f)
					continue;   // ��_���I�_������


				XMFLOAT3 X(
					vCenter.x + t * vCenterToCenter.x,
					vCenter.y + t * vCenterToCenter.y,
					vCenter.z + t * vCenterToCenter.z);

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

				X;// �Փ˓_
				N;// �Փ˃|���S���̖@���i�P�ʉ��ς݁j
				// Y��������ɂ������Ă���@����Ԃ�
				if (fAngle)
				{
					float _fNormalAngle;
					XMStoreFloat(&_fNormalAngle, XMVector3AngleBetweenNormals(XMVectorSet(0, 1, 0, 1), N));
					if (_fNormalAngle < fNormalAngle)
					{
						fNormalAngle = _fNormalAngle;
						*fAngle = fNormalAngle;
					}
				}
				/*  �����o���x�N�g���̌v�Z
					�@�Փ˂������b�V���̂����ꂩ�̒��_���玩���̒��S�ւ̃x�N�g����
					���b�V���̖@���̓��ς��v�Z���A�����o���x�N�g���̌v�Z���@�����肷��
					�A�����o���x�N�g�����v�Z��A���̏Փ˃I�u�W�F�N�g�ɐi��
				*/
				XMFLOAT3 _vForce = XMFLOAT3(0, 0, 0);
				float r;// �@���ɓ��e����OBB�̉e�̔����̒���
				float s;// �Փ˃��b�V���̒��_���玩���̒��S�܂ł̃x�N�g���̉e��@���ɗ��Ƃ����ۂ̒���
				float fLen[3];
				XMFLOAT4X4 mWorld = m_Transform.GetWorld();
				XMStoreFloat(&fLen[0], XMVector3Dot(XMVectorSet(mWorld._11, mWorld._12, mWorld._13, 1.0f)*m_vBBox.x, N));
				XMStoreFloat(&fLen[1], XMVector3Dot(XMVectorSet(mWorld._21, mWorld._22, mWorld._23, 1.0f)*m_vBBox.y, N));
				XMStoreFloat(&fLen[2], XMVector3Dot(XMVectorSet(mWorld._31, mWorld._32, mWorld._33, 1.0f)*m_vBBox.z, N));
				r = fabsf(fLen[0]) + fabsf(fLen[1]) + fabsf(fLen[2]);
				// �Փ˃��b�V���̒��_���玩���̒��S�����̃x�N�g��
				XMVECTOR vecVertexToCenter = XMVectorSet(vCenter.x - V0.x, vCenter.y - V0.y, vCenter.z - V0.z, 1.0f);
				XMStoreFloat(&s, XMVector3Dot(vecVertexToCenter, N));
				s = fabsf(s);

				float fDot;
				XMStoreFloat(&fDot, XMVector3Dot(vecVertexToCenter, N));
				if (0 <= fDot)
				{
					XMStoreFloat3(&_vForce, (r - s) * N);
				}
				else
				{
					XMStoreFloat3(&_vForce, (r + s) * N);
				}

				vForce.x += _vForce.x;
				vForce.y += _vForce.y;
				vForce.z += _vForce.z;
			}
		}

		if (newPos)
		{
			newPos->x = m_Transform.GetPos().x + vForce.x;
			newPos->y = m_Transform.GetPos().y + vForce.y;
			newPos->z = m_Transform.GetPos().z + vForce.z;
		}
	}



	return vectorColObj;
}

bool CModel::CollisionOBB(CGameObj* pOther)
{


	if (CGameObj::CollisionOBB(pOther))// �v���C���[�̓o�^�m�[�h�ɓo�^����Ă���I�u�W�F�N�g�Ƃ̂ݓ����蔻����s���V���ȏՓˊ֐���GameObj�Œ�`����K�v���邩��
	{
		// ����̒��_���
		// ����̒��_��
		TAssimpVertex* pVertex = nullptr;
		UINT*          pIndex = nullptr;
		UINT           nVertexNum = 0;
		UINT           nIndexNum = 0;
		// ���_�z��A�C���f�b�N�X�z����擾���Ă���
		CModel* pModel = (CModel*)pOther;
		CAssimpModel* pAssimp = GetAssimp(pModel->GetModelType());
		pAssimp->GetVertexCount(&nVertexNum, &nIndexNum);
		pVertex = new TAssimpVertex[nVertexNum];
		pIndex = new UINT[nIndexNum];
		pAssimp->GetVertex(pVertex, pIndex);

		// �����̏㉺���E�O��ɂn�a�a�ʂ܂ŐL�т���������A����̃��[���h�s��𑊎�̃��f���̒��_�ɂ����A�����Ƒ���̒��_����Փ˃|���S���̖@���𓱂�
		// �����̂n�a�a�̒��S�_
		XMFLOAT4X4 mWorld = m_Transform.GetWorld();
		XMMATRIX mtxWorld = XMLoadFloat4x4(&mWorld);
		XMFLOAT3 vCenter;
		XMVECTOR vecCenter; // �v�Z�p
		vecCenter = XMVector3TransformCoord(XMVectorSet(m_vCenter.x, m_vCenter.y, m_vCenter.z, 1.0f), mtxWorld);
		XMStoreFloat3(&vCenter, vecCenter); // OBB���S�_�ۑ�
		// �����̃��[�J�����W���擾
		XMFLOAT3 vX = XMFLOAT3(mWorld._11, mWorld._12, mWorld._13);
		XMFLOAT3 vY = XMFLOAT3(mWorld._21, mWorld._22, mWorld._23);
		XMFLOAT3 vZ = XMFLOAT3(mWorld._31, mWorld._32, mWorld._33);
		// ���S����ʂ̈ʒu�܂ł̃x�N�g��
		XMVECTOR vecX = XMVector3Normalize(XMVectorSet(vX.x, vX.y, vX.z, 1.0f)) * m_vBBox.x;
		XMVECTOR vecY = XMVector3Normalize(XMVectorSet(vY.x, vY.y, vY.z, 1.0f)) * m_vBBox.y;
		XMVECTOR vecZ = XMVector3Normalize(XMVectorSet(vZ.x, vZ.y, vZ.z, 1.0f)) * m_vBBox.z;
		
		// �����쐬
		XMFLOAT3 vP0, vP1;
		XMStoreFloat3(&vP0, vecCenter);
		vP1 = XMFLOAT3(
		    m_Transform.GetPos().x + m_Transform.GetVel().x, 
			m_Transform.GetPos().y + m_Transform.GetVel().y, 
			m_Transform.GetPos().z + m_Transform.GetVel().z);
		
		// ���C�i�����j
		XMFLOAT3 vW(vP1.x - vP0.x, vP1.y - vP0.y, vP1.z - vP0.z);

		// ����̑S�Ă̎O�p�`�ɂ���
		for (UINT i = 0; i < nIndexNum; i += 3) {
			// �O�p�`�̒��_���W
			XMFLOAT3& V0 = pVertex[pIndex[i + 0]].vPos;
			XMFLOAT3& V1 = pVertex[pIndex[i + 1]].vPos;
			XMFLOAT3& V2 = pVertex[pIndex[i + 2]].vPos;
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

			X;// �Փ˓_
			N;// �Փ˃|���S���̖@��

			// ���˕����ɐi�s�x�N�g����ύX
			XMVECTOR vecW = XMVectorSet(vW.x, vW.y, vW.z, 1.0f);
			vecW = vecW - 2 * XMVector3Dot(vecW, N)*N;
			XMStoreFloat3(&vW, vecW);
			m_Transform.GetVel().x += vW.x;
			m_Transform.GetVel().y += vW.y;
			m_Transform.GetVel().z += vW.z;
			
			// ���_�z��A�C���f�b�N�X�z��̉��
			SAFE_DELETE_ARRAY(pIndex);
			SAFE_DELETE_ARRAY(pVertex);

			return true;// �������Ă���
		}
		// ���_�z��A�C���f�b�N�X�z��̉��
		SAFE_DELETE_ARRAY(pIndex);
		SAFE_DELETE_ARRAY(pVertex);
	}

	

	return false;
}


//**************************************************************************************************************
// �����蔻��܂Ƃߏ���
//**************************************************************************************************************
void CModel::Collision()
{
	// �G�ꂽ�玀�ʃI�u�W�F�N�g�Ƃ̓����蔻��
	vector<CGameObj*> vectorColObj = CGameObj::CollisionOBB();
	if (0 < vectorColObj.size())
	{
		for (auto itr = vectorColObj.begin(); itr != vectorColObj.end();)
		{
			CGameObj* pColObj = *itr;
			// ���S����I�u�W�F�N�g�ɐG���Ǝ��S
			if (pColObj->GetTag() == GOT_DEATHOBJ)
			{
				m_Status.nHP = 0;

				return;
			}

			++itr;
		}
	}



	//**********�����蔻��**********
	// �n�`���f���ڒn�t���O��OBB�ڒn�t���O
	bool bTerrian, bBBox;
	bTerrian = bBBox = false;
	
	// �n�`���n����
	XMFLOAT3 X, N;
	if (m_pLand->Collision(
		this,
		XMFLOAT3(
			m_Transform.GetPos().x,
			m_Transform.GetPos().y + m_vBBox.y,
			m_Transform.GetPos().z),// P0 �����̎n�_
		XMFLOAT3(
			m_Transform.GetPos().x,
			m_Transform.GetPos().y - 1.0f,
			m_Transform.GetPos().z),  // P1 �����̏I�_(�����n�ʂɂ߂荞�ނ܂Ő�����L�΂��Ă���)
		&X, &N))
	{
		// �ڒn���Ă���ꍇ�̏���
		// Y�������̑��x0
		m_Transform.SetVel(XMFLOAT3(m_Transform.GetVel().x, 0, m_Transform.GetVel().z));
		// �����x0
		m_Transform.SetCurAccel(XMFLOAT3(0, 0, 0));
		// �ʒu����
		m_Transform.SetPos(X);
		// �n�`���f���ɐڒn
		bTerrian = true;

		m_Transform.GetWorld()._41 = m_Transform.GetPos().x;
		m_Transform.GetWorld()._42 = m_Transform.GetPos().y;
		m_Transform.GetWorld()._43 = m_Transform.GetPos().z;
	}

	// OBB�Ƃ̓����蔻��
	float fLandAngle;
	if (0 < CollisionOBBPlane(&fLandAngle, &X).size())
	{
		if (fLandAngle < XMConvertToRadians(45.0f))
		{
			// Y�������̑��x0
			m_Transform.SetVel(XMFLOAT3(m_Transform.GetVel().x, 0, m_Transform.GetVel().z));
			// �����x0
			m_Transform.SetCurAccel(XMFLOAT3(0, 0, 0));
			// OBB���f���ɐڒn
			bBBox = true;
		}
		// �ʒu����
		m_Transform.SetPos(X);
		m_Transform.GetWorld()._41 = m_Transform.GetPos().x;
		m_Transform.GetWorld()._42 = m_Transform.GetPos().y;
		m_Transform.GetWorld()._43 = m_Transform.GetPos().z;
	}

	// �n�`���f����OBB�̂����ꂩ�ɐڒn���Ă���ΐڒn�t���OON
	if (bTerrian || bBBox)
	{
		m_bGround = true;
	}
	else
	{
		m_bGround = false;


	}
}
