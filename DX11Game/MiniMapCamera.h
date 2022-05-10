#pragma once
#include "main.h"
#include "Camera.h"


class CAssimpModel;
class CPlayer;
class CGameObj;
class CMiniMapCamera : public CCamera
{
private:
	DirectX::XMFLOAT3 m_vPos;		    // 視点
	DirectX::XMFLOAT3 m_vAngle;		    // 回転量
	DirectX::XMFLOAT3 m_vStartTarget;    // 初期注視点
	DirectX::XMFLOAT3 m_vTarget;	        // 注視点
	DirectX::XMFLOAT3 m_vUp;		            // 上方ベクトル
	DirectX::XMFLOAT4X4 m_mtxWorld;	// ワールド マトリックス
	DirectX::XMFLOAT4X4 m_mtxView;	// ビュー マトリックス
	DirectX::XMFLOAT4X4 m_mtxProj;	// プロジェクション マトリックス
	float m_fNearZ;					// 前方クリップ距離
	float m_fFarZ;					// 後方クリップ距離
	CAssimpModel* m_pSky;			// スカイ ドーム
	CGameObj* m_pTargetObj;				// 基準となるオブジェクト
public:
	CMiniMapCamera();

	void Init();
	void Update();
	void Draw();
	void Clear();

	void SetPos(DirectX::XMFLOAT3& vPos) { m_vPos = vPos; }
	DirectX::XMFLOAT3& GetPos() { return m_vPos; }
	void SetPos(float x, float y, float z) { m_vPos.x = x, m_vPos.y = y, m_vPos.z = z; }
	void SetTarget(DirectX::XMFLOAT3& vTarget) { m_vTarget = vTarget; }
	void SetTarget(float x, float y, float z) { m_vTarget.x = x, m_vTarget.y = y, m_vTarget.z = z; }
	XMFLOAT3 GetTargetPos() { return m_vTarget; }
	void SetUpVector(DirectX::XMFLOAT3& vUp) { m_vUp = vUp; }
	void SetUpVector(float x, float y, float z) { m_vUp.x = x, m_vUp.y = y, m_vUp.z = z; }
	XMFLOAT3 GetUpVector() { return m_vUp; }
	void SetWorldMatrix(DirectX::XMFLOAT4X4& mtxWorld);
	DirectX::XMFLOAT4X4& GetWorldMatrix() { return m_mtxWorld; }
	DirectX::XMFLOAT4X4& CalcWorldMatrix();
	void SetFovY(float fFovY);
	void SetAspectRatio(float fAspect);
	void SetRangeZ(float fNearZ, float fFarZ);
	void UpdateMatrix();
	DirectX::XMFLOAT4X4& GetViewMatrix() { return m_mtxView; }
	DirectX::XMFLOAT4X4& GetProjMatrix() { return m_mtxProj; }
	DirectX::XMFLOAT3& GetAngle() { return m_vAngle; }
	void SetSkyDome(CAssimpModel* pSky) { m_pSky = pSky; }
	CAssimpModel* GetSkyDome() { return m_pSky; }
	void SetRadius(float fRadius) { m_fRadius = fRadius; }
	//void SetPlayer(CPlayer* pPlayer);
	void SetTargetObj(CGameObj* pObj);
	int CollisionViewFrustum(XMFLOAT3 vCenter, float fRadius);//可視判定(0:非表示,-1:部分, 1:表示)

	//void SetTargetObj(CGameObj* pObj) { m_pTargetObj = pObj; }
	//CGameObj* GetTargetObj() { return m_pTargetObj; }
	void SetFreeBulletMode(XMFLOAT2 fFreeBulletMode) { m_fFreeBulletMode = fFreeBulletMode; } // フリーバレットモード時の半径とカメラの高さを設定

private:
	void CreateViewFrustum();
	void UpdateViewFrustum();
};
