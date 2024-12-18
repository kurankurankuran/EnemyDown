#pragma once
#include "main.h"
#include <unordered_map>  
using namespace std;

// ‹¤’Ê
const LPCWSTR TITLE_LOG = L"data/texture/TitleName.png";
const LPCWSTR PINCHI_WAKU_RED = L"data/texture/Pinchi_Waku_Red.png";
const LPCWSTR GAGE_WAKU = L"data/texture/GageWaku(Kari).png";
const LPCWSTR GAGE_HAIKEI = L"data/texture/GageHaikei(Kari).png";
const LPCWSTR LIFEGAGE_GREEN = L"data/texture/LifeGage(Kari).png";
const LPCWSTR LIFEGAGE_RED = L"data/texture/LifeGage_Red(Kari).png";
const LPCWSTR MP_GAGE = L"data/texture/WeaponGage(Kari).png";
const LPCWSTR GAMECLEAR_CONGRATULATIONS = L"data/texture/Congratulations.png";
const LPCWSTR GAMECLEAR_THANKYOUFORPLAYING = L"data/texture/ThankyouforPlaying.png";
const LPCWSTR PRESSSPACEKEY = L"data/texture/PressSpaceKey.png";
const LPCWSTR GAMECLEAR_BG = L"data/texture/BG00z_80.jpg";
const LPCWSTR GAMEOVER_GAMEOVER = L"data/texture/GameOver.png";
const LPCWSTR GAMEOVER_BG = L"data/texture/BG00g1_80.jpg";
// Player

// Enemy

// Weapon



class CTextureData
{
private:
	static unordered_map<LPCWSTR, ID3D11ShaderResourceView*> m_TextureDataMap;

public:
	//HRESULT Init();
	
	static ID3D11ShaderResourceView* GetTextureResource(LPCWSTR TextureName); 
	static void ReleaseDataAll();

};