// �}�V���K���Ǘ��N���X [MGManager.h]
#pragma once
#include "WeaponManager.h"
#include "Machinegun.h"
#include <vector>

#include "Player.h"
#include "Land.h"

using namespace std;
namespace {
	const int MAX_MACHINEGUN = 240;
}

//struct TParts;

class CMGManager :public CWeaponManager
{
private:
	bool                      m_bDelFireRate;           // ���˃��[�g�����t���O
	float                     m_fFireRate;
	//ID3D11ShaderResourceView* m_pTex;
	CLand*                    m_pLand;
	int                       m_nEffekseerHandle;       // ���ˌ����G�t�F�N�V�A�̃n���h��
	XMFLOAT3                  m_vGeneratePos;           // �����ʒu
	int                       m_nChargeCnt;             // MP�����񕜃J�E���g
public:
	CMGManager(CScene* pScene);
	~CMGManager();

	virtual HRESULT Init(XMFLOAT4X4*);
	virtual void Fin();
	virtual void Update();

	virtual int Fire(int nDamage, int nTotalTime = 0, int nDelay = 0);
	//virtual int FireEnemy();
	virtual int Create( EGameObjTag Type, CModel* pEnemy = nullptr);
	


};

//TPartsInfo* GetMachinegunInitInfo();
