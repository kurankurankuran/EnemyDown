//=============================================================================
//
// �I�u�W�F�N�g �N���X���� [GameObj.cpp]
// Author : KAZUKI KURA
//
//=============================================================================
#include"GameObj.h"
#include"Scene.h"
#include <vector>

// �ۑ�����ϐ��̈ʒu���w��
DATARECORD CGameObj::m_gDataRecord[] =
{
   DATA_LOCAL(CGameObj, m_vStartPos),
   DATA_LOCAL(CGameObj, m_vStartAngle),
   DATA_LOCAL(CGameObj, m_vStartScale),
   DATA_LOCAL(CGameObj, m_vCenter),
   DATA_LOCAL(CGameObj, m_vBBox),
   DATA_END
};


// �O���[�o���ϐ�
namespace {
	const CMeshMaterial g_material = {
		XMFLOAT4(0.0f, 1.0f, 0.0f, 0.3f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
		1.0f
	};
	const XMFLOAT4 g_vRed(1.0f, 0.0f, 0.0f, 0.3f);
	const XMFLOAT4 g_vGreen(0.0f, 1.0f, 0.0f, 0.3f);
	const float DEL_HITSTOPTIME = 0.1f;
	

	
}



// �R���X�g���N�^
CGameObj::CGameObj(CScene* pScene) : m_pScene(pScene)
{
	// �^�O
	m_Tag = GOT_GAMEOBJ;

	// �A�����X�g�ɓo�^
	m_pBack = NULL;
	CGameObj* pTop = m_pScene->GetObj();
	m_pNext = pTop;
	if (pTop) {
		pTop->m_pBack = this;
	}
	else{
		// ����null���ƃ��X�g�̖���
		m_pScene->SetLastObj(this);
	}
	m_pScene->SetObj(this);



	// �X�e�[�^�X������
	m_Status.nATK = 0;
	m_Status.nDEF = 0;
	m_Status.nMaxHP = 0;
	m_Status.nHP = 0;
	m_Status.nINT = 0;
	m_Status.nRES = 0;

	// �o�E���f�B�{�b�N�X�E�X�t�B�A
	m_vBBox = XMFLOAT3(0.5f, 0.5f, 0.5f);
	m_fRadius = 0.5f;
	m_pBBox = nullptr;
	m_pBSphere = nullptr;
	m_eBS = BOUND_NONE;

	// ��
	m_bVisible = true;
	// �q�b�g�X�g�b�v����
	m_fHitStopTime = 0.0f;
}

// �f�X�g���N�^
CGameObj::~CGameObj(void)
{
	// �A�����X�g����폜
	if (m_pNext) {
		m_pNext->m_pBack = m_pBack;
	}
	else
	{
		// �O�����Ȃ��Ƃ������Ƃ͌��݂̎����̌��̃I�u�W�F�N�g������
		m_pScene->SetLastObj(m_pBack);
	}
	if (m_pBack) {
		m_pBack->m_pNext = m_pNext;
	} else {
		m_pScene->SetObj(m_pNext);
	}

	// �m�[�h���玩�����폜
	for (unsigned int i = 0; i < m_NodeIndexWork.CurIndex.size(); ++i)
	{
		CScene::GetOctree()->EraseObjInNode(m_NodeIndexWork.CurIndex[i], this);
	}
}

// �������O�ɍs����������
HRESULT CGameObj::Awake()    
{
	return S_OK;
}

// ������
HRESULT CGameObj::Init()
{
	// �n�ʐڒn�t���O
	m_bGround = false;
	// ���
	SetState(CharaState::Wait);
	// �g�����X�t�H�[��������
	m_Transform.SetPos(m_vStartPos);
	m_Transform.SetAngle(m_vStartAngle);
	m_Transform.SetScale(m_vStartScale);
	m_Transform.UpdateWorld();
	m_Transform.SetVel(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_Transform.SetCurAccel(XMFLOAT3(0.0f, 0.0f, 0.0f));
	

	m_fHitStopTime = 0;
	m_nInvTime = 0;

	CGameObj::SetRadius(m_fRadius);
	CGameObj::SetBBox(m_vBBox);

	// �m�[�h�ɓo�^
	Octree* pOctree = CScene::GetOctree();
	pOctree->EraseAndPushObjInNode(&m_NodeIndexWork, this);

	return S_OK;
}

// ���
void CGameObj::Fin()
{
	SAFE_DELETE(m_pBBox);
	SAFE_DELETE(m_pBSphere);
}

// �X�Vq
void CGameObj::Update()
{
	// XZ���ʈړ�
	switch (m_State)
	{
	case Walk:
		// ���x�ɉ����x�����Z
		m_Transform.SetVel(
			XMFLOAT3(
				m_Transform.GetVel().x + m_Transform.GetCurAccel().x,
				0.0f,
				m_Transform.GetVel().z + m_Transform.GetCurAccel().z));

		// �ō����x����
		XMVECTOR vecVel = XMLoadFloat3(&m_Transform.GetVel());
		if (m_Transform.GetVelMax() < XMVectorGetX(XMVector3Length(vecVel)))
		{
			XMStoreFloat3(&m_Transform.GetVel(), XMVector3Normalize(vecVel) * m_Transform.GetVelMax());
		}
		break;
	default:
		m_Transform.SetVel(XMFLOAT3(0, 0, 0));
		break;
	}
	// Y���ړ�
	m_Transform.SetVel(
		XMFLOAT3(
			m_Transform.GetVel().x,
			m_Transform.GetVel().y + m_Transform.GetCurAccel().y,
			m_Transform.GetVel().z));
	if (m_Transform.GetVel().y < -m_Transform.GetFallSpeedMax())
	{
		m_Transform.SetVel(XMFLOAT3(
			m_Transform.GetVel().x,
			-m_Transform.GetFallSpeedMax(),
			m_Transform.GetVel().z
		));
	}
	
	// ���W�ɑ��x�����Z
	m_Transform.SetPos(
		XMFLOAT3(
			m_Transform.GetPos().x + m_Transform.GetVel().x,
			m_Transform.GetPos().y + m_Transform.GetVel().y,
			m_Transform.GetPos().z + m_Transform.GetVel().z
		));

	// ���[���h�s��ɔ��f
	//m_Transform.UpdateWorld();
	m_Transform.GetWorld()._41 = m_Transform.GetPos().x;
	m_Transform.GetWorld()._42 = m_Transform.GetPos().y;
	m_Transform.GetWorld()._43 = m_Transform.GetPos().z;
	m_Transform.SetWorld(m_Transform.GetWorld());
}

// �`��
void CGameObj::Draw()
{
}

// �������`��
void CGameObj::DrawAlpha()
{
	m_pScene->GetLight()->SetEnable(false);
	if (m_eBS != BOUND_NONE) {
		XMVECTOR vCenter = XMLoadFloat3(&m_vCenter);
		int nCurCullNum = GetCurCullMode();

		if (m_eBS == BOUND_OBB) {
			XMMATRIX mWorld = XMMatrixTranslationFromVector(vCenter);
			mWorld *= XMLoadFloat4x4(&m_Transform.GetWorld());
		
			XMFLOAT4X4 mW;
			XMStoreFloat4x4(&mW, mWorld);
			if (m_pBBox) {
				SetCullMode(CULLMODE_NONE);//�J�����O����
				m_pBBox->SetWorld(&mW);
				m_pBBox->Draw();
				SetCullMode(nCurCullNum);//���̃J�����O�ɖ߂�
			}
		}
		else
		{
			XMMATRIX mWorld = XMLoadFloat4x4(&m_Transform.GetWorld());
			vCenter = XMVector3TransformCoord(vCenter, mWorld);
			mWorld = XMMatrixTranslationFromVector(vCenter);
			XMFLOAT4X4 mW;
			XMStoreFloat4x4(&mW, mWorld);
			//if (m_pBBox && m_bBBox) {
			if (m_pBBox && m_eBS == BOUND_AABB) {
				SetCullMode(CULLMODE_NONE);//�J�����O����
				m_pBBox->SetWorld(&mW);
				m_pBBox->Draw();
				SetCullMode(nCurCullNum);//���̃J�����O�ɖ߂�
			}
			if (m_pBSphere && m_eBS == BOUND_SPHERE) {
				m_pBSphere->SetWorld(&mW);
				m_pBSphere->Draw();
			
			}

		}
	}
	m_pScene->GetLight()->SetEnable(true);

}

void CGameObj::DrawUI()
{
}

// �ŏ��̑S������
HRESULT CGameObj::AwakeAll(CGameObj* pTop)
{
	HRESULT hr = S_OK;
	for (CGameObj* pObj = pTop; pObj; pObj = pObj->m_pNext) {
		hr = pObj->Awake();
		if (FAILED(hr)) {
			return hr;
		}
	}
	return hr;
}

// �S������
HRESULT CGameObj::InitAll(CGameObj* pTop)
{
	HRESULT hr = S_OK;
	for (CGameObj* pObj = pTop; pObj; pObj = pObj->m_pNext) {
		hr = pObj->Init();
		if (FAILED(hr)) {
			return hr;
		}
	}
	return hr;
}

// �S�X�V
void CGameObj::UpdateAll(CGameObj* pTop)
{
	CGameObj* pNext;
	for (CGameObj* pObj = pTop; pObj; pObj = pNext) {
		pNext = pObj->m_pNext;
		// �q�b�g�X�g�b�v����
		if (pObj->GetHitStopTime() > 0)
		{
			pObj->SetHitStopTime(pObj->GetHitStopTime() - DEL_HITSTOPTIME);
			continue;
		}

		pObj->Update();
	}
}

// �S�`��
void CGameObj::DrawAll(CGameObj* pTop)
{
	// �s���������`��
	for (CGameObj* pObj = pTop; pObj; pObj = pObj->m_pNext) {
		//if (pObj->m_bVisible) {
			pObj->Draw();
		//}
	}

	// �����������`��
	SetBlendState(BS_ALPHABLEND);
	SetZWrite(false);
	for (CGameObj* pObj = pTop; pObj; pObj = pObj->m_pNext) {
		//if (pObj->m_bVisible) {
			pObj->DrawAlpha();
		//}
	}
	SetZWrite(true);
	SetBlendState(BS_NONE);
	
}

void CGameObj::DrawUIAll(CGameObj* pTop)
{
	for (CGameObj* pObj = pTop; pObj; pObj = pObj->m_pNext) {
		pObj->DrawUI();
	}
}

// �S�I������
void CGameObj::FinAll(CGameObj* pTop)
{
	CGameObj* pNext;
	for (CGameObj* pObj = pTop; pObj; pObj = pNext) {
		pNext = pObj->m_pNext;
		pObj->Fin();
		delete pObj;
	}
}


void CGameObj::AddHP(int nHP)
{
	m_Status.nHP += nHP;
	if (m_Status.nMaxHP < m_Status.nHP)
	{
		m_Status.nHP = m_Status.nMaxHP;
	}
}
void CGameObj::SetHP(int nHP)
{
	m_Status.nHP = nHP;
	if (m_Status.nMaxHP < m_Status.nHP)
	{
		m_Status.nHP = m_Status.nMaxHP;
	}
}

std::vector<CGameObj*> CGameObj::CollisionOBB()                 // OBB�����蔻��(�����̃m�[�h�ɓo�^����Ă���I�u�W�F�N�g�Ƃ�)
{
	std::vector<CGameObj*> ColObjVector;
	if (!m_bVisible)
		return ColObjVector;

	Octree* pOctree = CScene::GetOctree();
	
	// �����̏����m�[�h����o�^����Ă���I�u�W�F�N�g�𒊏o
	std::vector<CGameObj*> NodeInObjVector;
	for (unsigned int i = 0; i < m_NodeIndexWork.CurIndex.size(); ++i)
	{
		for (
			auto itr = pOctree->GetNodeInVector(m_NodeIndexWork.CurIndex[i])->ObjList.begin(); 
			itr != pOctree->GetNodeInVector(m_NodeIndexWork.CurIndex[i])->ObjList.end();
			)
		{
			if (*itr != this) {
				NodeInObjVector.push_back(*itr);
			}
			++itr;
		}
		
	}

	// �x�N�^�[����d�����Ȃ�
	std::sort(NodeInObjVector.begin(), NodeInObjVector.end());
	NodeInObjVector.erase(std::unique(NodeInObjVector.begin(), NodeInObjVector.end()), NodeInObjVector.end());

	XMFLOAT4X4 mW1 = m_Transform.GetWorld();     // ���t���[�����W�̗\�z���[���h�s��
	for (unsigned int i = 0; i < NodeInObjVector.size(); ++i)
	{
		
		XMFLOAT4X4 mW2 = NodeInObjVector[i]->GetTransform()->GetWorld();
		// ���S���W�擾
		XMFLOAT3 vP1, vP2;
	
		XMStoreFloat3(&vP1, XMVector3TransformCoord(XMLoadFloat3(&m_vCenter), XMLoadFloat4x4(&mW1)));
		XMStoreFloat3(&vP2, XMVector3TransformCoord(XMLoadFloat3(&NodeInObjVector[i]->GetCenter()), XMLoadFloat4x4(&mW2)));
		// ���S���W�Ԃ̃x�N�g�������߂�
		XMVECTOR vD;
		vD = XMVectorSet(vP2.x - vP1.x, vP2.y - vP1.y, vP2.z - vP1.z, 0.0f);
		// ���f�����W�������߂�
		XMVECTOR vN[6];
		vN[0] = XMVectorSet(mW1._11, mW1._12, mW1._13, 0.0f);
		vN[1] = XMVectorSet(mW1._21, mW1._22, mW1._23, 0.0f);
		vN[2] = XMVectorSet(mW1._31, mW1._32, mW1._33, 0.0f);
		vN[3] = XMVectorSet(mW2._11, mW2._12, mW2._13, 0.0f);
		vN[4] = XMVectorSet(mW2._21, mW2._22, mW2._23, 0.0f);
		vN[5] = XMVectorSet(mW2._31, mW2._32, mW2._33, 0.0f);
		// OBB�̑傫��(����)���|�����x�N�g�������߂�
		XMFLOAT3& vBB1 = m_vBBox;
		XMFLOAT3& vBB2 = NodeInObjVector[i]->GetBBox();
		XMVECTOR vL[6];
		vL[0] = XMVectorSet(mW1._11 * vBB1.x, mW1._12 * vBB1.x, mW1._13 * vBB1.x, 0.0f);
		vL[1] = XMVectorSet(mW1._21 * vBB1.y, mW1._22 * vBB1.y, mW1._23 * vBB1.y, 0.0f);
		vL[2] = XMVectorSet(mW1._31 * vBB1.z, mW1._32 * vBB1.z, mW1._33 * vBB1.z, 0.0f);
		vL[3] = XMVectorSet(mW2._11 * vBB2.x, mW2._12 * vBB2.x, mW2._13 * vBB2.x, 0.0f);
		vL[4] = XMVectorSet(mW2._21 * vBB2.y, mW2._22 * vBB2.y, mW2._23 * vBB2.y, 0.0f);
		vL[5] = XMVectorSet(mW2._31 * vBB2.z, mW2._32 * vBB2.z, mW2._33 * vBB2.z, 0.0f);
		// ���������ʂ̖@���x�N�g��(���f�����W��)
		float fL, f, fD;
		bool bCol = true;
		for (int i = 0; i < 6; ++i) {
			XMVECTOR& vS = vN[i];//������(���I�u�W�F�N�g�̍��W��)
			// OBB�̉e(�̔���)�̍��v
			fL = 0.0f;
			for (int j = 0; j < 6; ++j) {

				// �������ɒ��S�_����e�ʂɐ������e�ʂ܂ł̋����̑傫���x�N�g���̉e�𗎂Ƃ��A���̒��������v����
				XMStoreFloat(&f, XMVector3Dot(vS, vL[j]));
				// ��Βl�����ĉ��Z
				fL += fabsf(f);
			}
			// ���S���W�Ԃ̋����Ɣ�r
			XMStoreFloat(&fD, XMVector3Dot(vS, vD)); // �������ɒ��S�ԃx�N�g���̉e�𗎂Ƃ������̑傫���i�j
			fD = fabsf(fD);
			if (fL < fD)
			{
				bCol = false;/*return false*/;// �������Ă��Ȃ�
				break;
			}
		}
		if (!bCol)
		{
			continue;
		}
		// �RD�̏ꍇ�͑o���̕����x�N�g���ɐ����ȕ������ɂ��Ă����߂� 
		// �����������ꂼ��̕ӂɐ����ȃx�N�g��
		XMVECTOR vS;
		for (int i = 0; i < 3; ++i) {
			for (int j = 3; j < 6; ++j) {
				// �������������߂�
				vS = XMVector3Normalize(XMVector3Cross(vN[i], vN[j]));
				// OBB�̉e(�̔���)�̍��v
				fL = 0.0f;
				for (int k = 0; k < 6; ++k) {
					XMStoreFloat(&f, XMVector3Dot(vS, vL[k]));
					fL += fabsf(f);
				}
				// ���S���W�Ԃ̋����Ɣ�r
				XMStoreFloat(&fD, XMVector3Dot(vS, vD));
				fD = fabsf(fD);
				if (fL < fD)
				{
					bCol = false;/*return false*/;// �������Ă��Ȃ�
					break;
				}
			}
			if (!bCol)
			{
				break;
			}
		}
		if (bCol) {
			ColObjVector.push_back(NodeInObjVector[i]);// �������Ă���

			// �����Ŋ֐��|�C���^���p���ăN���X���ƂɏՓˎ��̏����iUnity�ł���OnCollision�Ȃǁj��ǉ����Ă�������

		}
	}

	return ColObjVector; // �������Ă���I�u�W�F�N�g�|�C���^�x�N�^�[��Ԃ�
}

// OBB���m�̏Փ˔���
bool CGameObj::CollisionOBB(CGameObj* pOther)
{
	if(pOther == this) return false;
	if (!m_bVisible || !pOther->m_bVisible)
		return false;

	// ���[���h �}�g���b�N�X�擾
	XMFLOAT4X4 mW1 = m_Transform.GetWorld();
	mW1._41 = m_Transform.GetPos().x;
	mW1._42 = m_Transform.GetPos().y;
	mW1._43 = m_Transform.GetPos().z;
	XMFLOAT4X4 mW2 = pOther->GetTransform()->GetWorld();
	// ���S���W�擾
	XMFLOAT3 vP1, vP2;
	XMStoreFloat3(&vP1, XMVector3TransformCoord(XMLoadFloat3(&m_vCenter), XMLoadFloat4x4(&mW1)));
	XMStoreFloat3(&vP2, XMVector3TransformCoord(XMLoadFloat3(&pOther->GetCenter()),XMLoadFloat4x4(&mW2)));
	// ���S���W�Ԃ̃x�N�g�������߂�
	XMVECTOR vD;
	vD = XMVectorSet(vP2.x - vP1.x, vP2.y - vP1.y, vP2.z - vP1.z, 0.0f);
	// ���f�����W�������߂�
	XMVECTOR vN[6];
	vN[0] = XMVectorSet(mW1._11, mW1._12, mW1._13, 0.0f);
	vN[1] = XMVectorSet(mW1._21, mW1._22, mW1._23, 0.0f);
	vN[2] = XMVectorSet(mW1._31, mW1._32, mW1._33, 0.0f);
	vN[3] = XMVectorSet(mW2._11, mW2._12, mW2._13, 0.0f);
	vN[4] = XMVectorSet(mW2._21, mW2._22, mW2._23, 0.0f);
	vN[5] = XMVectorSet(mW2._31, mW2._32, mW2._33, 0.0f);
	// OBB�̑傫��(����)���|�����x�N�g�������߂�
	XMFLOAT3& vBB1 = m_vBBox;
	XMFLOAT3& vBB2 = pOther->GetBBox();
	XMVECTOR vL[6];
	vL[0] = XMVectorSet(mW1._11 * vBB1.x, mW1._12 * vBB1.x, mW1._13 * vBB1.x, 0.0f);
	vL[1] = XMVectorSet(mW1._21 * vBB1.y, mW1._22 * vBB1.y, mW1._23 * vBB1.y, 0.0f);
	vL[2] = XMVectorSet(mW1._31 * vBB1.z, mW1._32 * vBB1.z, mW1._33 * vBB1.z, 0.0f);
	vL[3] = XMVectorSet(mW2._11 * vBB2.x, mW2._12 * vBB2.x, mW2._13 * vBB2.x, 0.0f);
	vL[4] = XMVectorSet(mW2._21 * vBB2.y, mW2._22 * vBB2.y, mW2._23 * vBB2.y, 0.0f);
	vL[5] = XMVectorSet(mW2._31 * vBB2.z, mW2._32 * vBB2.z, mW2._33 * vBB2.z, 0.0f);
	// ���������ʂ̖@���x�N�g��(���f�����W��)
	float fL, f, fD;
	for (int i = 0; i < 6; ++i) {
		XMVECTOR& vS = vN[i];//������(���I�u�W�F�N�g�̍��W��)
		// OBB�̉e(�̔���)�̍��v
		fL = 0.0f;
		for (int j = 0; j < 6; ++j) {

			// �������ɒ��S�_����e�ʂɐ������e�ʂ܂ł̋����̑傫���x�N�g���̉e�𗎂Ƃ��A���̒��������v����
			XMStoreFloat(&f, XMVector3Dot(vS, vL[j]));
			// ��Βl�����ĉ��Z
			fL += fabsf(f);
		}
		// ���S���W�Ԃ̋����Ɣ�r
		XMStoreFloat(&fD, XMVector3Dot(vS, vD)); // �������ɒ��S�ԃx�N�g���̉e�𗎂Ƃ������̑傫���i�j
		fD = fabsf(fD);
		if (fL < fD) return false;// �������Ă��Ȃ�
	}
    // �RD�̏ꍇ�͑o���̕����x�N�g���ɐ����ȕ������ɂ��Ă����߂� 
	// �����������ꂼ��̕ӂɐ����ȃx�N�g��
	XMVECTOR vS;
	for (int i = 0; i < 3; ++i) {
		for (int j = 3; j < 6; ++j) {
			// �������������߂�
			vS = XMVector3Normalize(XMVector3Cross(vN[i], vN[j]));
			// OBB�̉e(�̔���)�̍��v
			fL = 0.0f;
			for (int k = 0; k < 6; ++k) {
				XMStoreFloat(&f, XMVector3Dot(vS, vL[k]));
				fL += fabsf(f);
			}
			// ���S���W�Ԃ̋����Ɣ�r
			XMStoreFloat(&fD, XMVector3Dot(vS, vD));
			fD = fabsf(fD);
			if (fL < fD) return false;// �������Ă��Ȃ�
		}
	}
	return true;// �������Ă���
}


// AABB���m�̏Փ˔���
bool CGameObj::CollisionAABB(CGameObj* pOther)
{
	if (pOther == this) return false;
	if (!m_bVisible || !pOther->m_bVisible)
		return false;
	// ���S���W�����߂�
	XMFLOAT3 vA, vB;
	XMStoreFloat3(&vA,
		XMVector3TransformCoord(
			XMLoadFloat3(&m_vCenter),
			XMLoadFloat4x4(&m_Transform.GetWorld())));
	XMStoreFloat3(&vB,
		XMVector3TransformCoord(
			XMLoadFloat3(&pOther->m_vCenter),
			XMLoadFloat4x4(&pOther->GetTransform()->GetWorld())));
	// �Փ˔���
	XMFLOAT3& vAr = m_vBBox;
	XMFLOAT3& vBr = pOther->m_vBBox;
	return vA.x - vAr.x <= vB.x + vBr.x &&
		vB.x - vBr.x <= vA.x + vAr.x &&
		vA.y - vAr.y <= vB.y + vBr.y &&
		vB.y - vBr.y <= vA.y + vAr.y &&
		vA.z - vAr.z <= vB.z + vBr.z &&
		vB.z - vBr.z <= vA.z + vAr.z;
}

// ���E�����m�̏Փ˔���
bool CGameObj::CollisionBS(CGameObj* pOther)
{
	if (pOther == this) return false;
	if (!m_bVisible || !pOther->m_bVisible)
		return false;
	// ���S���W�����߂�
	XMFLOAT3 vA, vB;
	XMStoreFloat3(&vA,
		XMVector3TransformCoord(
			XMLoadFloat3(&m_vCenter),
			XMLoadFloat4x4(&m_Transform.GetWorld())));
	XMStoreFloat3(&vB,
		XMVector3TransformCoord(
			XMLoadFloat3(&pOther->m_vCenter),
			XMLoadFloat4x4(&pOther->GetTransform()->GetWorld())));
	// �Փ˔���
	float dx = vA.x - vB.x;
	float dy = vA.y - vB.y;
	float dz = vA.z - vB.z;
	float dr = m_fRadius + pOther->m_fRadius;
	return dx * dx + dy * dy + dz * dz <= dr * dr;
}

// ���E�{�b�N�X�ݒ�
void CGameObj::SetBBox(XMFLOAT3 vBBox)
{
	HRESULT hr = S_OK;
	if (vBBox.x == 0 || vBBox.y == 0 || vBBox.z == 0)return;
	if (m_pBBox) {
		m_pBBox->Fin();
		SAFE_DELETE(m_pBBox);
	}
	m_vBBox = vBBox;

	m_pBBox = new CBox();
	hr = m_pBBox->Init(vBBox);

	if (FAILED(hr)) {
		m_pBBox->Fin();
		SAFE_DELETE(m_pBBox);
	}
	if (m_pBBox) {
		m_pBBox->SetCamera(m_pScene->GetCameraManager()->GetCamera());
		std::shared_ptr<CCamera> pMiniCamera = m_pScene->GetCameraManager()->GetMiniMapCamera();
		if (pMiniCamera)
		{
			m_pBBox->SetMiniMapCamera(pMiniCamera);
		}
		m_pBBox->SetLight(m_pScene->GetLight());
		m_pBBox->SetMaterial(&g_material);
	}
}

// ���E���ݒ�
void CGameObj::SetRadius(float fRadius)
{
	HRESULT hr = S_OK;
	if (m_pBSphere) {
		m_pBSphere->Fin();
		SAFE_DELETE(m_pBSphere);
	}

	m_fRadius = fRadius;

	m_pBSphere = new CSphere();
	hr = m_pBSphere->Init(16, 8, fRadius);

	if (FAILED(hr)) {
		m_pBSphere->Fin();
		SAFE_DELETE(m_pBSphere);
	}
	if (m_pBSphere) {
		m_pBSphere->SetCamera(m_pScene->GetCameraManager()->GetCamera());
		m_pBSphere->SetLight(m_pScene->GetLight());
		m_pBSphere->SetMaterial(&g_material);
	}
}
