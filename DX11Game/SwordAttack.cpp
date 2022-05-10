// ���U�� [SwordAttack.cpp]
#include "SwordAttack.h"
#include "Scene.h"
#include "Enemy.h"
#include "Explosion.h"
//#include "Game.h"
#include "Player.h"
#include "Sound.h"
#include "EffectManager.h"
#include "EffekseerManager.h"
#include "Text.h"
#include "DebugProc.h"
#include "TextureData.h"

// �萔
namespace {
	// �v���C���[
	//const int TIME_PLAYER = 15; // ��������
	const int DAMAGE_TO_ENEMY = 9999; // �_���[�W
	const int INV_TIME_PLAYER = 30;// �q�b�g��������ɗ^���閳�G����
	
	// �G�l�~�[
	//const int TIME_ENEMY = 150;  // ��������
	const int ENEMY_DAMAGE_TO_PLAYER = 20;  // �_���[�W
	const int INV_TIME_ENEMY = 10;// �q�b�g��������ɗ^���閳�G����

	// �{�X
	//const int TIME_BOSS = 120;  // ��������
	const int BOSS_DAMAGE_TO_PLAYER = 20;  // �_���[�W
	const int INV_TIME_BOSS = 10;// �q�b�g��������ɗ^���閳�G����

	// �e�L�X�g�\������
	const int DAMAGE_CNT = 100;
	const XMFLOAT2 DAMAGE_SCALE = XMFLOAT2(8.0f, 16.0f);
	const XMFLOAT4 DAMAGE_COLOR = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	const int SPLIT = 40;
}



// �R���X�g���N�^
CSwordAttack::CSwordAttack(CScene* pScene) : CWeapon(pScene)
{
	SetTag(GOT_GAMEOBJ);
}

// �f�X�g���N�^
CSwordAttack::~CSwordAttack()
{
}

// ������
HRESULT CSwordAttack::Init()
{
	// �����������̂��߃��X�g�̍Ō���Ɉړ�
	m_pScene->ToEnd(this);

	SetModel(MT_SPHERE);
	posArray = std::vector<PosBuffer>(SPLIT, PosBuffer());
	m_Mesh.SetCamera(m_pScene->GetCameraManager()->GetCamera());
	m_Mesh.SetLight(m_pScene->GetLight());
	m_Mesh.SetPimitiveType(PT_UNDEFINED);
	m_fLongScale = 0.0f;
	HRESULT hr = CModel::Init();
	m_pBBox->SetTrigger(true);
	return hr;
}

void CSwordAttack::Fin()
{
	CModel::Fin();
}

// �X�V
void CSwordAttack::Update()
{
	// �e���q�b�g�X�g�b�v���Ȃ玩�����X�g�b�v
	if (0 < m_pParent->GetHitStopTime())
	{
		return;
	}

	// �e���q�b�g�o�b�N��ԂɂȂ����炷���Ɏ�������������
	CModel* pParentModel = (CModel*)m_pParent;
	if (CharaState::HitBack == pParentModel->GetState())
	{
		Fin();
		delete this;
		return;
	}

	// �������ԍX�V
	--m_Status.nHP;
	if (m_Status.nHP <= 0) {
		if (m_Status.nHP == 0)
		{
			Fin();
		}
		// ���̋O�Ղ���납������Ă�������
		if (0 < posArray.size())
		{
			posArray.pop_back();
			// ���̋O�Ճ��b�V���쐬
			MakeTraces();
			return;
		}
		delete this;
		return;
	}
	if (!m_bVisible) {
		Fin();
		delete this;
		return;
	}

	
	// ���[���h�}�g���N�X�ɔ��f�����Ă���(�e���A�j�������[�J��)
	XMMATRIX mtxWorld, mtxScl, mtxRot, mtxTranslate;
	// ���[���h�}�g���N�X�̏�����
	mtxWorld = XMMatrixIdentity();
	mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&m_pParent->GetTransform()->GetWorld()));
	
	// ��������̃��f���Ɉʒu�����킹��
	pParentModel->SetAnimIndex(pParentModel->GetAnimStack());
	pParentModel->SetAnimTime(pParentModel->GetAnimTime());
	XMFLOAT4X4 AnimGlobalMat = pParentModel->GetBoneGlobalMatrix(m_strBoneName);
	mtxWorld = XMLoadFloat4x4(&AnimGlobalMat) * mtxWorld;
	
	// ���̓����蔻��̃��[�J���F�ʒu��]�傫����ݒ�
	XMMATRIX mtxLocalWorld;
	mtxLocalWorld = XMMatrixIdentity();
	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(
		m_Transform.GetScale().x, 
		m_Transform.GetScale().y, 
		m_Transform.GetScale().z);
	mtxLocalWorld = XMMatrixMultiply(mtxLocalWorld, mtxScl);
	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_Transform.GetAngle().x),
		XMConvertToRadians(m_Transform.GetAngle().y), 
		XMConvertToRadians(m_Transform.GetAngle().z));
	mtxLocalWorld = XMMatrixMultiply(mtxLocalWorld, mtxRot);
	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(
		m_Transform.GetPos().x, 
		m_Transform.GetPos().y, 
		m_Transform.GetPos().z);
	mtxLocalWorld = XMMatrixMultiply(mtxLocalWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�ݒ�
	mtxWorld = mtxLocalWorld * mtxWorld;
	XMStoreFloat4x4(&m_Transform.GetWorld(), mtxWorld);
	
	// �����蔻����s����m_vPos�̒l�����[���h���W�ɂ�������̂Ƃ��Ĉ������A���Ɋւ��Ă̓��[�J���̒l�Ƃ��Ĉ����Ă���̂ňꎞ�I��m_vPos�̒l��ύX����
	XMFLOAT3 vPos = m_Transform.GetPos();
	m_Transform.SetPos(
		XMFLOAT3(
			m_Transform.GetWorld()._41,
			m_Transform.GetWorld()._42,
			m_Transform.GetWorld()._43
		));
	// �e�̓o�^�m�[�h���ɂ���I�u�W�F�N�g�Ƃ̓����蔻��
	std::vector<CGameObj*> ColObjVector = CGameObj::CollisionOBB();

	if (ColObjVector.size() > 0)
	{
		switch (m_Type) {
			// �������v���C���[���甭�˂��ꂽ�e�Ȃ�
		case GOT_PLAYER:
			for (unsigned int i = 0; i < ColObjVector.size(); ++i)
			{
				// ���肪�s���łȂ�
				if (!ColObjVector[i]->GetVisible())continue;
				// ���肪����łȂ�
				if (ColObjVector[i]->GetStatus().nHP <= 0)continue;
				// ���肪���G���Ԓ��łȂ�
				if (ColObjVector[i]->GetInvTime() > 0)continue;

				// �Փˑ����ID���Ƃ̏���
				switch (ColObjVector[i]->GetTag())
				{
					// �G���{�X
				case GOT_ENEMY:
				case GOT_BOSS:
				{
					// ���C�t���炷����
					int nDamage = ColObjVector[i]->GetStatus().nDEF - m_nDamage;
					if (nDamage < 0) {
						ColObjVector[i]->AddHP(nDamage);
						// �q�b�g�X�g�b�v
						ColObjVector[i]->SetHitStopTime(1.0f);
						// �_���[�W���[�V��������
						CModel* pModel = (CModel*)ColObjVector[i];
						pModel->StartHitMotion();
						// ���G���Ԕ���
						ColObjVector[i]->SetInvTime(INV_TIME_PLAYER);
						m_pParent->SetHitStopTime(0.5f);
					}
					else
					{
						nDamage = 0;
					}

					// �_���[�W�e�L�X�g
					std::string DamegTest = std::to_string(-nDamage);
					CText::OutPutText(DamegTest, m_Transform.GetPos(), DAMAGE_SCALE, DAMAGE_COLOR, DAMAGE_CNT);
					
					// �G�t�F�N�g
					int Handle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::HIT_EXPLOSION_PLAYER_EFK), false, TIMER_HIT_EXPLOSION_PLAYER_EFK);
					CEffekseerManager::SetMatrix(Handle, false, m_Transform.GetWorld());

					// �T�E���h
					CSound::Play(SE_SWORD_HIT);
					
					break;
				}
				// �v���C���[�Ȃ�X���[
				case GOT_PLAYER:

					break;
				// ID���ݒ�I�u�W�F�N�g�Ȃ�X���[
				default:
				{
					break;
				
				}
				}

			}

			break;
			// �������G���{�X���甭�˂��ꂽ�e�Ȃ�
		case GOT_BOSS:
			for (unsigned int i = 0; i < ColObjVector.size(); ++i)
			{
				// ���肪���̏ꍇ�̂ݓ����蔻����s��
				if (!ColObjVector[i]->GetVisible())continue;
				// ���肪�q�b�g�X�g�b�v���łȂ�
				//if (ColObjVector[i]->GetHitStopTime() > 0)continue;
				// ���肪����łȂ�
				if (ColObjVector[i]->GetStatus().nHP <= 0)continue;
				// ���肪���G���Ԓ��łȂ�
				if (ColObjVector[i]->GetInvTime() > 0)continue;

				// �Փˑ��肪�v���C���[�Ȃ�ȉ��̏���
				if (ColObjVector[i]->GetTag() == GOT_PLAYER) {

					// ���C�t���炷����
					int nDamage = ColObjVector[i]->GetStatus().nDEF - m_nDamage;
					if (nDamage < 0) {
						ColObjVector[i]->AddHP(nDamage);
						// �_���[�W���[�V��������
						CModel* pModel = (CModel*)ColObjVector[i];
						pModel->StartHitMotion();

						// ���G����
						ColObjVector[i]->SetInvTime(INV_TIME_BOSS);
					}
					else
					{
						nDamage = 0;
					}

					// �_���[�W�e�L�X�g
					std::string DamegTest = std::to_string(-nDamage);
					CText::OutPutText(DamegTest, m_Transform.GetPos(), DAMAGE_SCALE, DAMAGE_COLOR, DAMAGE_CNT);

					// �G�t�F�N�g
					int Handle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::HIT_EXPLOSION_ENEMY_EFK), false, TIMER_HIT_EXPLOSION_ENEMY_EFK);
					CEffekseerManager::SetMatrix(Handle, false, m_Transform.GetWorld());
					
					// �T�E���h
					CSound::Play(SE_PLAYER_DAMAGE);

					break;
				}
				// ID���ݒ�I�u�W�F�N�g�Ȃ�X���[
				else if (ColObjVector[i]->GetTag() != GOT_ENEMY && ColObjVector[i]->GetTag() != GOT_BOSS) {

					break;
				}
			}
			break;
		case GOT_ENEMY:
			for (unsigned int i = 0; i < ColObjVector.size(); ++i)
			{
				// ���肪���̏ꍇ�̂ݓ����蔻����s��
				if (!ColObjVector[i]->GetVisible())continue;
				// ���肪�q�b�g�X�g�b�v���łȂ�
				//if (ColObjVector[i]->GetHitStopTime() > 0)continue;
				// ���肪����łȂ�
				if (ColObjVector[i]->GetStatus().nHP <= 0)continue;
				// ���肪���G���Ԓ��łȂ�
				if (ColObjVector[i]->GetInvTime() > 0)continue;

				// �Փˑ��肪�v���C���[�Ȃ�ȉ��̏���
				if (ColObjVector[i]->GetTag() == GOT_PLAYER) {

					// ���C�t���炷����
					int nDamage = ColObjVector[i]->GetStatus().nDEF - m_nDamage;
					if (nDamage < 0) {
						ColObjVector[i]->AddHP(nDamage);

						// �_���[�W���[�V��������
						CModel* pModel = (CModel*)ColObjVector[i];
						pModel->StartHitMotion();

						// ���G����
						ColObjVector[i]->SetInvTime(INV_TIME_ENEMY);
					}
					else
					{
						nDamage = 0;
					}

					// �_���[�W�e�L�X�g
					std::string DamegTest = std::to_string(-nDamage);
					CText::OutPutText(DamegTest, m_Transform.GetPos(), DAMAGE_SCALE, DAMAGE_COLOR, DAMAGE_CNT);

					// �G�t�F�N�g
					int Handle = CEffekseerManager::Play(CEffekseerManager::GetPath(EfkType::HIT_EXPLOSION_ENEMY_EFK), false, TIMER_HIT_EXPLOSION_ENEMY_EFK);
					CEffekseerManager::SetMatrix(Handle, false, m_Transform.GetWorld());

					// �T�E���h
					CSound::Play(SE_PLAYER_DAMAGE);

					break;
				}
				// ID���ݒ�I�u�W�F�N�g�Ȃ�X���[
				else if (ColObjVector[i]->GetTag() != GOT_ENEMY && ColObjVector[i]->GetTag() != GOT_BOSS) {
					
					break;
				}
			}
			break;
		}

	
	}

	// �l�����ɖ߂�
	m_Transform.SetPos(vPos);

	// �m�[�h�ɓo�^
	Octree* pOctree = CScene::GetOctree();
	pOctree->EraseAndPushObjInNode(&m_NodeIndexWork, this);

	// ���̋O�Ճ��b�V���쐬
	MakeTraces();
}



void CSwordAttack::Draw()
{
	int nCurCullMode = GetCurCullMode();
	SetCullMode(CULLMODE_NONE);
	m_pScene->GetLight()->SetEnable(false);

	// �����������`��
	SetBlendState(BS_ALPHABLEND);
	//SetZWrite(false);
	if (index.size() >= 4)
	{
		m_Mesh.SetTexture(CTextureData::GetTextureResource(m_TexName));
		m_Mesh.Draw();
	}
	//SetZWrite(true);
	SetBlendState(BS_NONE);

	m_pScene->GetLight()->SetEnable(true);
	SetCullMode(nCurCullMode);
}

// ����
int CSwordAttack::Fire(XMFLOAT4X4* pWorld, EGameObjTag Type, CModel* pModel)
{
	/*CScene* pScene = CScene::GetCurrent();
	if (!pScene)
		return -1;
	CSwordAttack* pMissile = new CSwordAttack(pScene);
	pMissile->SetWorld(*pWorld);
	pMissile->Init();
	pMissile->m_pTarget = pModel;
	pMissile->m_Type = Type;

	pMissile->SetBSVisible((EBoundingShape)(pScene)->GetBound());*/



	return 1;
}

void CSwordAttack::CreateCurveVertex(std::vector<PosBuffer>& usedPosArray)
{
	if (usedPosArray.size() < 3 || SPLIT < 1) { return; }
	std::vector<PosBuffer> newPosArray;
	newPosArray.reserve(usedPosArray.size() + (usedPosArray.size() - 1) * SPLIT);
	const float amount = 1.0f / (SPLIT + 1);

	PosBuffer newPos;
	newPosArray.push_back(usedPosArray.front());
	for (size_t i = 0; i < usedPosArray.size() - 1; ++i)
	{
		float ratio = amount;
		//  CatMul�Ɏg��4�̓_�����ip0, p3���Ȃ����̏����������j
		XMVECTOR p0Head = i == 0 ? (XMLoadFloat3(&usedPosArray[1].headPos) + XMLoadFloat3(&usedPosArray[2].headPos)) * 0.5f : XMLoadFloat3(&usedPosArray[i - 1].headPos);
		XMVECTOR p1Head = XMLoadFloat3(&usedPosArray[i].headPos);
		XMVECTOR p2Head = XMLoadFloat3(&usedPosArray[i + 1].headPos);
		XMVECTOR p3Head = i == usedPosArray.size() - 2 ? (p0Head + p2Head) * 0.5f : XMLoadFloat3(&usedPosArray[i + 2].headPos);

		XMVECTOR p0Tail = i == 0 ? (XMLoadFloat3(&usedPosArray[1].tailPos) + XMLoadFloat3(&usedPosArray[2].tailPos)) * 0.5f : XMLoadFloat3(&usedPosArray[i - 1].tailPos);
		XMVECTOR p1Tail = XMLoadFloat3(&usedPosArray[i].tailPos);
		XMVECTOR p2Tail = XMLoadFloat3(&usedPosArray[i + 1].tailPos);
		XMVECTOR p3Tail = i == usedPosArray.size() - 2 ? (p0Tail + p2Tail) * 0.5f : XMLoadFloat3(&usedPosArray[i + 2].tailPos);

		for (size_t j = 0; j < static_cast<size_t>(SPLIT - 1); ++j)
		{
			newPos = PosBuffer();

			newPos.isUsed = true;
			XMStoreFloat3(&newPos.headPos, XMVectorCatmullRom(p0Head, p1Head, p2Head, p3Head, ratio));
			XMStoreFloat3(&newPos.tailPos, XMVectorCatmullRom(p0Tail, p1Tail, p2Tail, p3Tail, ratio));

			newPosArray.push_back(newPos);
			ratio += amount;
		}
		newPosArray.push_back(usedPosArray[i + 1]);
	}
	usedPosArray = newPosArray;
}

void CSwordAttack::MakeTraces()
{
	XMFLOAT4X4 mW = m_Transform.GetWorld();
	XMFLOAT3 vZ = XMFLOAT3(
		mW._31, 
		mW._32, 
		mW._33);
	XMStoreFloat3(&vZ, XMVector3Normalize(XMLoadFloat3(&vZ)));

	XMFLOAT3 vHead;
	vHead.x = mW._41 - vZ.x * m_fLongScale * m_vBBox.y;
	vHead.y = mW._42 - vZ.y * m_fLongScale * m_vBBox.y;
	vHead.z = mW._43 - vZ.z * m_fLongScale * m_vBBox.y;
	XMFLOAT3 vTail;
	vTail.x = mW._41 + vZ.x * m_fLongScale * m_vBBox.y;
	vTail.y = mW._42 + vZ.y * m_fLongScale * m_vBBox.y;
	vTail.z = mW._43 + vZ.z * m_fLongScale * m_vBBox.y;
	tempPos.headPos = vHead;
	tempPos.tailPos = vTail;
	tempPos.isUsed = true;
	//�f�[�^���X�V
	for (int i = posArray.size() - 1; i > 0; --i)
	{
		posArray[i] = posArray[i - 1];
	}
	if (posArray.empty())return;
	posArray.front() = tempPos;
	tempPos = PosBuffer();

	//�Ȑ������
	std::vector<PosBuffer> usedPosArray;
	for (unsigned int i = 0; i < posArray.size(); ++i)
	{
		if (posArray[i].isUsed) {
			usedPosArray.push_back(posArray[i]);
		}
	}
	if (usedPosArray.empty())return;
	CreateCurveVertex(usedPosArray);

	//���_�f�[�^���X�V����
	float amount = 1.0f / (usedPosArray.size() - 1);
	float v = 0;
	vertex.clear();
	index.clear();
	vertex.resize(usedPosArray.size() * 2);
	/*usedPosArray[0].isUsed = true;*/
	for (size_t i = 0, j = 0; i < vertex.size() && j < usedPosArray.size(); i += 2, ++j)
	{
		//if (!usedPosArray[j].isUsed)continue;
		vertex[i].vtx = usedPosArray[j].headPos;
		vertex[i].tex = XMFLOAT2(1.0f, v);
		vertex[i].nor = XMFLOAT3(mW._21, mW._22, mW._23);
		vertex[i].diffuse = XMFLOAT4(1, 1, 1, 0.7f);
		index.push_back(i);
		vertex[i + 1].vtx = usedPosArray[j].tailPos;
		vertex[i + 1].tex = XMFLOAT2(0.0f, v);
		vertex[i + 1].nor = XMFLOAT3(mW._21, mW._22, mW._23);
		vertex[i + 1].diffuse = XMFLOAT4(1, 1,	1, 0.1f);
		index.push_back(i + 1);

		v += amount;
	}
	if (4 <= index.size()) 
	{
		HRESULT hr;
		hr = m_Mesh.Init(&vertex[0], vertex.size(), &index[0], index.size());
	}
}

void CSwordAttack::SetTraceScale(XMFLOAT3 vScale)
{
	//m_Transform.SetScale(vScale);
	m_fLongScale = m_Transform.GetScale().x;
	if (m_fLongScale < m_Transform.GetScale().y)
	{
		m_fLongScale = m_Transform.GetScale().y;
	}
	if (m_fLongScale < m_Transform.GetScale().z)
	{
		m_fLongScale = m_Transform.GetScale().z;
	}
}