#include "CreateObj.h"
#include "Scene.h"
#include "Player.h"
#include "EnemyManager.h"
#include "Enemy_Mutant.h"
#include "Enemy_Wizard.h"
#include "Enemy_Maria.h"
#include "Land.h"
#include "DeathCube.h"

CCreateObj::CCreateObj()
{
	
}

CCreateObj::~CCreateObj()
{

}

CGameObj* CCreateObj::CreateObj(EObjID eObjID)
{
	// �I�u�W�F�N�g�N���X�̐���
	switch (eObjID)
	{
	case OI_PLAYER:
		return new CPlayer(CScene::GetCurrent());
	case OI_ENEMY_MANAGER:
		return new CEnemyManager(CScene::GetCurrent());
	case OI_ENEMY_MUTANT:
		return new CEnemy_Mutant(CScene::GetCurrent());
	case OI_ENEMY_WIZARD:
		return new CEnemy_Wizard(CScene::GetCurrent());
	case OI_ENEMY_MARIA:
		return new CEnemy_Maria(CScene::GetCurrent());
	case OI_LAND:
		return new CLand(CScene::GetCurrent());
	case OI_DEATHOBJ:
		return new CDeathCube(CScene::GetCurrent());
	default:
		break;
	}

	CModel* pModel = new CModel(CScene::GetCurrent());
	// CModel�N���X�̐���
	switch (eObjID)
	{
	case OI_PLADIN:
		pModel->SetModel(EModelType::MT_PALADIN);
		return pModel;
	case OI_MUTANT:
		pModel->SetModel(EModelType::MT_MUTANT);
		return pModel;
	case OI_WIZARD:
		pModel->SetModel(EModelType::MT_WIZARD);
		return pModel;
	case OI_MARIA:
		pModel->SetModel(EModelType::MT_MARIA);
		return pModel;
	case OI_CLIFF:
		pModel->SetModel(EModelType::MT_CLIFF);
		// �X�P�[���ɖ��Ή�
		pModel->SetBBox(XMFLOAT3(150.f, 1000.f, 2500.0f));
		return pModel;
	case OI_ROCK:
		pModel->SetModel(EModelType::MT_ROCK);
		pModel->SetBBox(XMFLOAT3(150, 150, 150));
		return pModel;
	default:
		return nullptr;
	}
}

//CGameObj* CCreateObj::CreateObj(EModelType eModelType)
//{
//	CModel* pModel;
//	pModel = new CModel(CScene::GetCurrent());
//	switch (eModelType)
//	{
//	case MT_PALADIN:// �R�m
//		pModel->SetModel(EModelType::MT_PALADIN);
//		break;
//	case MT_MUTANT:// �~���[�^���g
//		pModel->SetModel(EModelType::MT_MUTANT);
//		break;
//	case MT_WIZARD:// ���@�g��
//		pModel->SetModel(EModelType::MT_WIZARD);
//		break;
//	case MT_MARIA:// �}���A
//		pModel->SetModel(EModelType::MT_MARIA);
//		break;
//	case MT_SPHERE:// ����
//		pModel->SetModel(EModelType::MT_SPHERE);
//		break;
//	case MT_SKY:// �X�J�C�h�[��
//		pModel->SetModel(EModelType::MT_SKY);
//		break;
//	case MT_LAND:// �n�`
//		pModel->SetModel(EModelType::MT_LAND);
//		break;
//	case MT_CUBE:// �L���[�u
//		pModel->SetModel(EModelType::MT_CUBE);
//		break;
//	case MT_ROCK:// ��
//		pModel->SetModel(EModelType::MT_ROCK);
//		break;
//	case MT_CLIFF:// �R
//		pModel->SetModel(EModelType::MT_CLIFF);
//		break;
//	default:// �ݒ�Ȃ�
//		pModel->SetModel(EModelType::MAX_MODEL);
//		break;
//	}
//
//	return pModel;
//}