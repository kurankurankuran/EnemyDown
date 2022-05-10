#pragma once
#include "Scene.h"
#include "Player.h"
#include "Camera.h"
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include <EffekseerSoundXAudio2.h>
#include <unordered_map>

#pragma comment(lib, "Effekseer")
#pragma comment(lib, "EffekseerRendererDX11")
#pragma comment(lib, "EffekseerSoundXAudio2")


enum  EfkType
{
	SMOKE_EFK = 0,                                 // 雲
	HIT_EXPLOSION_ENEMY_EFK,                       // 敵攻撃の爆発
	HIT_EXPLOSION_PLAYER_EFK,                      // プレイヤー攻撃の爆発
	DEATH_EXPLOSION_ENEMY_EFK,                     // 敵死亡時の爆発
	DEATH_EXPLOSION_PLAYER_EFK,                    // プレイヤー死亡時の爆発
	MACHINGUN_GENERATER_PLAYER_EFK,                // プレイヤーマシンガン発射源        *50
	MACHINGUN_GENERATER_PLAYER_EXPLOSION_EFK,      // プレイヤーマシンガン発射源の爆発  *25
	MACHINGUN_GENERATER_ENEMY_EFK,                // プレイヤーマシンガン発射源        *50
	MACHINGUN_GENERATER_ENEMY_EXPLOSION_EFK,      // プレイヤーマシンガン発射源の爆発  *25
	MACHINGUN_PLAYER_EFK,                          // プレイヤーマシンガンそのもの      *50
	MACHINGUN_ENEMY_EFK,                           // エネミーマシンガンそのもの*50
	SLASHING_EFK,                                  // 剣の波動攻撃？
	ENEMY_CREATE_EFK,                                  // 敵生成時エフェクト

	MAX_EFK
};
// EFK再生時間
const int	TIMER_SMOKE_EFK = 9999;                                   // 雲
const int	TIMER_HIT_EXPLOSION_ENEMY_EFK = 60;                       // 敵攻撃の爆発
const int	TIMER_HIT_EXPLOSION_PLAYER_EFK = 60;                      // プレイヤー攻撃の爆発
const int	TIMER_DEATH_EXPLOSION_ENEMY_EFK = 100;                    // 敵死亡時の爆発
const int	TIMER_DEATH_EXPLOSION_PLAYER_EFK = 120;                   // プレイヤー死亡時の爆発
const int	TIMER_MACHINGUN_GENERATER_PLAYER_EFK = 60;                // プレイヤーマシンガン発射源
const int	TIMER_MACHINGUN_GENERATER_PLAYER_EXPLOSION_EFK = 120;     // プレイヤーマシンガン発射源の爆発
const int	TIMER_MACHINGUN_GENERATER_ENEMY_EFK = 60;                // プレイヤーマシンガン発射源
const int	TIMER_MACHINGUN_GENERATER_ENEMY_EXPLOSION_EFK = 120;     // プレイヤーマシンガン発射源の爆発
const int	TIMER_MACHINGUN_PLAYER_EFK = 10;                          // プレイヤーマシンガンそのもの
const int	TIMER_MACHINGUN_ENEMY_EFK = 10;                           // エネミーマシンガンそのもの
const int	TIMER_SLASHING_EFK = 60;                                  // 剣の波動攻撃？
const int	TIMER_ENEMY_CREATE_EFK = 120;                                  // 敵生成時エフェクト

// Efkローカルパラメーター
const int   SCALE_ENEMY_CREATE_EFK = 5;                              // 敵生成時エフェクト


// Efkオブジェクトパラメータ
struct EfkObj{

	Effekseer::Handle t_Handle;// ハンドル
	const char16_t* t_szPath;
	int t_nTime;               // １サイクル時間
	int t_nCnt;                // サイクルカウント
	EfkObj()
	{
	}

	EfkObj(const char16_t* path, Effekseer::Handle _Handle,int nTime,  int nCnt = 0)
	{
		t_Handle = _Handle;
		t_szPath = path;
		t_nTime = nTime;
		t_nCnt = nCnt;
	}
	
};

class CEffekseerManager
{
public:
	CEffekseerManager(CScene*);
	~CEffekseerManager();

	bool Init();
	void Uninit();
	void Update();
	void Draw();

	// 個別Update
	static void Update(const int nHandle, bool bLoop);

	//void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }

	
	// Effekseerの位置セット
	static void SetLocation(Effekseer::Handle _Handle, XMFLOAT3 vPos);
	// Effekseerの拡縮セット
	static void SetScale(int nIdx, bool bLoop, XMFLOAT3 vScl);
	// Effekseerの回転セット
	static void SetRotation(Effekseer::Handle _Handle, XMFLOAT3 vRot);

	// Effekseer内カメラマトリクス更新
	void UpdateCameraMatrix();


	// 回転、座標
	static void SetMatrix(/*Effekseer::Handle _Handle*/int nIdx, bool bLoop, XMFLOAT4X4& world);
	// Efkファイル読み込み
	static bool Load(const char16_t* path);
	// Efk再生開始
	static /*::Effekseer::Handle*/int Play(const char16_t* path, bool bLoop, int32_t nTime);
	// Efk終了
	static void Stop(/*Effekseer::Handle*/int,bool);

	static int FindRegisterIdx(std::unordered_map<int, EfkObj>, int);

	// Efkパス取得
	static const char16_t* GetPath(EfkType);
private:
	EffekseerSound::SoundRef m_Sound;
	IXAudio2* m_xa2;
	IXAudio2MasteringVoice* m_xa2_master;
	
	// Effekseer内カメラ視点位置を確定
	Effekseer::Vector3D m_Position;
	
	// カメラポインタ
	CCamera* m_pCamera;

	// レンダー
	static EffekseerRendererDX11::RendererRef m_Renderer;
	// マネージャー
	static Effekseer::ManagerRef m_Manager;
	// 読み込んだファイルにインデックス付して管理
	static std::unordered_map<const char16_t*, Effekseer::EffectRef> m_RegisteredEffectRef;
	//ループさせる再生中のエフェクトが入る
	static std::unordered_map</*::Effekseer::Handle*/int, EfkObj> m_RegisteredEffectsLoop;
	//ループさせない再生中のエフェクトが入る
	static std::unordered_map</*::Effekseer::Handle*/int, EfkObj> m_RegisteredEffectsNoLoop;
};


