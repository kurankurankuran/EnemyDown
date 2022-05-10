#include "WeaponManager.h"


CWeaponManager::CWeaponManager(CScene* pScene)
{
	
	m_pScene = pScene;
	m_nMagazine = 0;
	m_nReloadCnt = 0;
	m_bCreate = true;
	m_pParent = nullptr;
	m_bVisible = true;
	m_bOldVisible = true;
	m_pParentWorld = nullptr;
	
	m_nChangeNum = 0;
	m_nChangeCnt = 0;

	m_eType = EGameObjTag::GOT_PLAYER;

	m_pWeapon.clear();
	
}

CWeaponManager::~CWeaponManager()
{

}

void CWeaponManager::SetWorld(int i, XMFLOAT4X4& pWorld)
{
	if (m_pWeapon[i])
		m_pWeapon[i]->GetTransform()->SetWorld(pWorld);
}

void CWeaponManager::AllDelete()
{
	for (unsigned int i = 0; i < m_pWeapon.size(); ++i) {
		if (m_pWeapon[i]) delete m_pWeapon[i];
	}

	m_pWeapon.clear();
}


bool CWeaponManager::GetChangeFlg()
{
	if (m_nChangeCnt <= 0) {
		return true;
	}
	else
	{
		return false;
	}
}