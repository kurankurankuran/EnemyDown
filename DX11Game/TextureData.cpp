#include "TextureData.h"
#include "Texture.h"

unordered_map<LPCWSTR, ID3D11ShaderResourceView*> CTextureData::m_TextureDataMap;

ID3D11ShaderResourceView* CTextureData::GetTextureResource(LPCWSTR TextureName)
{
	HRESULT hr = S_OK;

	auto itr = m_TextureDataMap.find(TextureName);
	if (itr != m_TextureDataMap.end())
	{
		// ���ɓǂݍ��܂�Ă���

	}
	else
	{
		// �ǂݍ��݊J�n
		ID3D11Device* pDevice = GetDevice();

		hr = CreateTextureFromFile(
			pDevice,				  // �f�o�C�X�ւ̃|�C���^
			TextureName,	      // �t�@�C���̖��O
			&m_TextureDataMap[TextureName]);// �ǂݍ��ރ������[

		if (FAILED(hr)) {
			ReleaseDataAll();
			MessageBoxW(0, L"[TextureData.cpp]�e�N�X�`���ǂݍ��ݎ��s", nullptr, MB_OK);
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