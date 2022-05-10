#pragma once
#include "main.h"

#define	VIEW_ANGLE			(45.0f)										// 視野角
#define	VIEW_ASPECT			((float)SCREEN_WIDTH/(float)SCREEN_HEIGHT)	// アスペクト比
#define	VIEW_NEAR_Z			(1.0f)										// NearZ値
#define	VIEW_FAR_Z			(50000.0f)									// FarZ値




class CAssimpModel;
class CPlayer;
class CGameObj;
class CCamera
{
protected:
	static DirectX::XMFLOAT3 m_vStartPos;		// 初期視点
	static DirectX::XMFLOAT3 m_vPos;		    // 視点
	static DirectX::XMFLOAT3 m_vAngle;		    // 回転量
	static DirectX::XMFLOAT3 m_vStartTarget;    // 初期注視点
	static DirectX::XMFLOAT3 m_vTarget;	        // 注視点
	static DirectX::XMFLOAT3 m_vUp;		            // 上方ベクトル
	static DirectX::XMFLOAT4X4 m_mtxWorld;	// ワールド マトリックス
	static DirectX::XMFLOAT4X4 m_mtxView;	// ビュー マトリックス
	static DirectX::XMFLOAT4X4 m_mtxProj;	// プロジェクション マトリックス
	static float m_fNearZ;					// 前方クリップ距離
	static float m_fFarZ;					// 後方クリップ距離
	static CAssimpModel* m_pSky;			// スカイ ドーム
	static CGameObj* m_pTargetObj;				// 基準となるオブジェクト
	static int m_nReeticleTextIndex;

	float m_fAspectRatio;			// 縦横比
	float m_fFovY;					// 視野角(Degree)
	

	

	float m_fRadius;				// 半径

	//CPlayer* m_pPlayer;				// 基準となるオブジェクト
	
	float m_fAngle;					// カメラアングル

	DirectX::XMFLOAT4 m_frus[6];	// 視錐台
	DirectX::XMFLOAT4 m_frusw[6];	// 視錐台(ワールド空間)

	DirectX::XMFLOAT2 m_fFreeBulletMode;
public:
	CCamera();

	virtual void Init();
	virtual void Fin() {};
	virtual void Update();
	virtual void Draw() {};
	virtual void Clear();

	virtual void SetPos(DirectX::XMFLOAT3& vPos) { m_vPos = vPos; }
	virtual DirectX::XMFLOAT3& GetPos() { return m_vPos; }
	virtual void SetPos(float x, float y, float z) { m_vPos.x = x, m_vPos.y = y, m_vPos.z = z; }
	virtual void SetTarget(DirectX::XMFLOAT3& vTarget) { m_vTarget = vTarget; }
	virtual void SetTarget(float x, float y, float z) { m_vTarget.x = x, m_vTarget.y = y, m_vTarget.z = z; }
	virtual XMFLOAT3 GetTargetPos() { return m_vTarget; }
	virtual void SetUpVector(DirectX::XMFLOAT3& vUp) { m_vUp = vUp; }
	virtual void SetUpVector(float x, float y, float z) { m_vUp.x = x, m_vUp.y = y, m_vUp.z = z; }
	virtual XMFLOAT3 GetUpVector() { return m_vUp; }
	virtual void SetWorldMatrix(DirectX::XMFLOAT4X4& mtxWorld);
	virtual DirectX::XMFLOAT4X4& GetWorldMatrix() { return m_mtxWorld; }
	virtual DirectX::XMFLOAT4X4& CalcWorldMatrix();
	virtual void SetFovY(float fFovY);
	virtual void SetAspectRatio(float fAspect);
	virtual void SetRangeZ(float fNearZ, float fFarZ);
	virtual void UpdateMatrix();
	virtual DirectX::XMFLOAT4X4& GetViewMatrix() { return m_mtxView; }
	virtual DirectX::XMFLOAT4X4& GetProjMatrix() { return m_mtxProj; }
	virtual DirectX::XMFLOAT3& GetAngle() { return m_vAngle; }
	virtual void SetSkyDome(CAssimpModel* pSky) { m_pSky = pSky; }
	virtual CAssimpModel* GetSkyDome() { return m_pSky; }
	virtual void SetRadius(float fRadius) { m_fRadius = fRadius; }
	virtual void SetTargetObj(CGameObj* pObj);
	virtual int CollisionViewFrustum(XMFLOAT3 vCenter,float fRadius);//可視判定(0:非表示,-1:部分, 1:表示)
	
	virtual void SetFreeBulletMode(XMFLOAT2 fFreeBulletMode) { m_fFreeBulletMode = fFreeBulletMode; } // フリーバレットモード時の半径とカメラの高さを設定
protected:
	virtual void CreateViewFrustum();
	virtual void UpdateViewFrustum();
};
