#include "TextureData.h"
#include "Texture.h"

unordered_map<LPCWSTR, ID3D11ShaderResourceView*> CTextureData::m_TextureDataMap;

ID3D11ShaderResourceView* CTextureData::GetTextureResource(LPCWSTR TextureName)
{
	HRESULT hr = S_OK;

	auto itr = m_TextureDataMap.find(TextureName);
	if (itr != m_TextureDataMap.end())
	{
		// 既に読み込まれている

	}
	else
	{
		// 読み込み開始
		ID3D11Device* pDevice = GetDevice();

		hr = CreateTextureFromFile(
			pDevice,				  // デバイスへのポインタ
			TextureName,	      // ファイルの名前
			&m_TextureDataMap[TextureName]);// 読み込むメモリー

		if (FAILED(hr)) {
			ReleaseDataAll();
			MessageBoxW(0, L"[TextureData.cpp]テクスチャ読み込み失敗", nullptr, MB_OK);
			return nullptr;
		}
	}


	return m_TextureDataMap[TextureName];
}


void CTextureData::ReleaseDataAll()
{
	for (auto itr = m_TextureDataMap.begin(); itr != m_TextureDataMap.end();)
	{
		SAFE_RELEASE(itr->second);
		++itr;
	}
}