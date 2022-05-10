// 剣攻撃 [Sword.h]
#pragma once
#include "Weapon.h"
#include "Mesh.h"





class CEnemy;
class CSwordAttack : public CWeapon
{
private:
	CGameObj* m_pParent;
	std::string m_strBoneName;

	// 軌跡描画用テクスチャ
	LPCWSTR m_TexName;
	// 軌跡描画用メッシュ
	CMesh m_Mesh;
	//剣の位置を保存する構造体
	struct PosBuffer
	{
		XMFLOAT3 headPos = { 0,0,0 }; //剣の先端の位置
		XMFLOAT3 tailPos = { 0,0,0 }; //剣の末端の位置
		bool isUsed = false;
	};

	std::vector<PosBuffer> posArray;             //剣の位置を保存するバッファ
	std::vector<VERTEX_3D> vertex;               //頂点バッファ
	std::vector<int> index;               //インデックスバッファ
	PosBuffer tempPos;                           //現在フレームでの剣の位置

	float m_fLongScale;                   // 攻撃の軌跡の参考にする長さ
public:
	CSwordAttack(CScene* pScene);
	~CSwordAttack();

	virtual HRESULT Init();
	virtual void Fin();
	virtual void Update();
	virtual void Draw();

	int Fire(XMFLOAT4X4* pWorld, EGameObjTag Type, CModel* pEnemy = nullptr);

	//int Fire();

	void SetParentObj(CGameObj* pGameObj) { m_pParent = pGameObj; }
	void SetBoneName(std::string strBoneName) { m_strBoneName = strBoneName; }
	void SetTexture(LPCWSTR pName) { m_TexName = pName; }
	void CreateCurveVertex(std::vector<PosBuffer>& usedPosArray);
	
	void SetTraceScale(XMFLOAT3 vScale);

private:
	void MakeTraces();
};
