#include "Weapon.h"
#include "Enemy.h"
#include "Scene.h"


CWeapon::CWeapon(CScene* pScene) : CModel(pScene)
{
	m_nDamage = 0;
	m_pTarget = nullptr;
	//m_EfkHandle = nullptr;
	m_nEffectCnt = 0;
	m_ColObjVector.clear();
}

CWeapon::~CWeapon()
{

}

bool CWeapon::SerchTarget()
{
	float vecTargetLen;
	float fZAngle;
	CEnemy* pEnemy;
	float fDist;

	pEnemy = (CEnemy*)m_pScene->Find(GOT_ENEMY);

	// ���[���h�ϊ��s��擾
	XMFLOAT4X4 mW = m_Transform.GetWorld();
	XMFLOAT3 vZ = XMFLOAT3(mW._31, mW._32, mW._33);
	// ���W�擾�A�N���A
	m_Transform.SetPos(XMFLOAT3(mW._41, mW._42, mW._43));

	if (m_pTarget == nullptr) {
		while (pEnemy) {
			if (pEnemy->GetVisible()) {
				XMFLOAT3& vPos = pEnemy->GetTransform()->GetPos();
				fDist = FLT_MAX;
				XMFLOAT3 vTarget;
				vTarget.x = vPos.x - m_Transform.GetPos().x;
				vTarget.y = vPos.y - m_Transform.GetPos().y;
				vTarget.z = vPos.z - m_Transform.GetPos().z;
				XMVECTOR vecTarget = XMLoadFloat3(&vTarget);
				// �^�[�Q�b�g�ւ̃x�N�g���̒����擾
				XMStoreFloat(&vecTargetLen, XMVector3Length(vecTarget));
				// ���K��
				vecTarget = XMVector3Normalize(vecTarget);
				// �ǐՔ͈͓��i�~�T�C���O�� 45�x�̉~���ȓ��j�ɂ���ꍇ�̂ݒǐ�
				fZAngle = XMVectorGetX(XMVector3AngleBetweenNormals(XMLoadFloat3(&vZ), vecTarget));
				if (fZAngle <= 3.14159265359f / 4.0f && vecTargetLen <= 2500.0f) {
					if (!m_pTarget) {
						fDist = vecTargetLen;
						m_pTarget = pEnemy;
					}
					else {
						if (fDist > vecTargetLen) {
							fDist = vecTargetLen;
							m_pTarget = pEnemy;
						}
					}
				}
			}
			pEnemy = (CEnemy*)m_pScene->Find(GOT_ENEMY, pEnemy);
		}
	}

	if (m_pTarget)
	{
		return true;
	}
	else
	{
		return false;
	}
}