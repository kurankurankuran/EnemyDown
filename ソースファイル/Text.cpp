#include "Text.h"
#include "Texture.h"
#include "Polygon.h"

// �ÓI�����o
ID3D11ShaderResourceView*	                                                    CText::m_pTextureText;
unordered_map<int/*�e�L�X�g�C���f�b�N�X*/, TextParam/*�e�L�X�g�p�����[�^*/>     CText::m_TextMap;
vector<bool/*�C���f�b�N�X�ԍ��g�p���t���O*/>                                    CText::m_TextIndexVector;
unordered_map<char/*���͕���*/, XMFLOAT2/*UV*/>                                 CText::m_UVMap;
CCamera*                                                                        CText::m_pCamera;

// �萔
namespace {
	const int     MAX_TEXT = 100;// ��ʏ�ɓ����ɕ`��\�ȃe�L�X�g���i�C���f�b�N�X�̊Ǘ�������j
	const LPCWSTR g_pszPathTexText = L"data/texture/eisuuziTip.png";

	const int     TEXT_NUM_X = 26;
	const int     TEXT_NUM_Y = 3;
}

//********************
// �R���X�g���N�^
//********************
CText::CText()
{
	m_pTextureText = nullptr;
	m_pCamera = nullptr;
}

//********************
// �f�X�g���N�^�@
//********************
CText::~CText()
{

}

//********************
// �e�L�X�g������
//********************
HRESULT CText::Init()
{
	HRESULT hr;
	ID3D11Device* pDevice = GetDevice();

	// �����e�N�X�`���̈ꕶ�����Ƃ�UV��ݒ�
	UVSet();

	// �e�L�X�g�p�����[�^�N���A
	m_TextMap.clear();

	// �C���f�b�N�X������
	for (int i = 0; i < MAX_TEXT; ++i) {
		m_TextIndexVector.push_back(false);
	}
	
	// �e�N�X�`���ǂݍ���
	// �p�����p
	hr = CreateTextureFromFile(
		pDevice,					   // �f�o�C�X�ւ̃|�C���^
		g_pszPathTexText,		       // �t�@�C���̖��O
		&m_pTextureText);			   // �ǂݍ��ރ������[
	if (FAILED(hr)) {
		Uninit();
		return false;
	}

	return hr;
}

//********************
// �e�L�X�g�I������
//********************
void CText::Uninit()
{
	// �e�N�X�`�����
	SAFE_RELEASE(m_pTextureText);
	// �C���f�b�N�X�N���A
	m_TextIndexVector.clear();
	// �e�L�X�g�N���A
	m_TextMap.clear();
	// UV�N���A
	m_UVMap.clear();
}

//********************
// �e�L�X�g�X�V
//********************
void CText::Update()
{
	// �J�E���g��0���傫���p�����[�^�S�ẴJ�E���g�X�V
	for (auto itr = m_TextMap.begin(); itr != m_TextMap.end();)
	{
		if (0 < itr->second.nCnt) {
			--itr->second.nCnt;
			if (itr->second.nCnt == 0)
			{
				// �C���f�b�N�X���g�p�ɕύX
				m_TextIndexVector[itr->first] = false;
				// �e�L�X�g�폜
				itr = m_TextMap.erase(itr);
				continue;
			}

			if (m_pCamera) 
			{
				// �RD��ԗp�e�L�X�g�̓X�N���[����̈ʒu���X�V
				if (itr->second.bSolid)
				{
					// 3D��2D�ϊ�
					XMMATRIX mProj = XMLoadFloat4x4(&m_pCamera->GetProjMatrix());
					XMMATRIX mView = XMLoadFloat4x4(&m_pCamera->GetViewMatrix());
					XMMATRIX mWorld = XMMatrixIdentity();
					XMVECTOR vecPos = XMVector3Project(
						XMLoadFloat3(&itr->second.vPosSolid),
						0.0f, 0.0f,
						SCREEN_WIDTH, SCREEN_HEIGHT,
						0.0f, 1.0f,
						mProj,
						mView,
						mWorld);

					XMFLOAT3 vScreenPos;
					XMStoreFloat3(&vScreenPos, vecPos);
					vScreenPos.x -= SCREEN_CENTER_X;
					vScreenPos.y -= SCREEN_CENTER_Y;

					itr->second.vPos.x = vScreenPos.x;
					itr->second.vPos.y = -vScreenPos.y;

					// �`��t���O�X�V
					if (vScreenPos.z <= 1.0f)
					{
						itr->second.bDraw = true;
					}
					else
					{
						// ��ʊO
						itr->second.bDraw = false;
					}

				}
			}
		}
		++itr;
	}
}

//********************
// �e�L�X�g�`��
//********************
void CText::Draw()
{
	SetBlendState(BS_ALPHABLEND);	// �A���t�@�u�����h�L��
	SetZWrite(false);				// Z�o�b�t�@�X�V���Ȃ�

	ID3D11DeviceContext* pDC = GetDeviceContext();

	// Map�S�Đ���
	for (auto itr = m_TextMap.begin(); itr != m_TextMap.end();)
	{
		// �`��t���O
		if (itr->second.bDraw)
		{
			// �ꕶ�����`��
			for (unsigned int i = 0; i < itr->second.text.size(); ++i)
			{
				CPolygon::SetTexture(m_pTextureText);
				CPolygon::SetPos(itr->second.vPos.x + i * itr->second.vScale.x, itr->second.vPos.y);
				CPolygon::SetSize(itr->second.vScale.x, itr->second.vScale.y);
				CPolygon::SetColor(itr->second.vColor.x, itr->second.vColor.y, itr->second.vColor.z);
				CPolygon::SetAlpha(itr->second.vColor.w);
				CPolygon::SetFrameSize(1.0f / TEXT_NUM_X, 1.0f / TEXT_NUM_Y);
				XMFLOAT2 UV = XMFLOAT2(m_UVMap[itr->second.text[i]].x, m_UVMap[itr->second.text[i]].y);
				CPolygon::SetUV(m_UVMap[itr->second.text[i]].x, m_UVMap[itr->second.text[i]].y);
				CPolygon::Draw(pDC);
			}
		}
		++itr;
	}

	SetZWrite(true);							// Z�o�b�t�@�X�V����
	SetBlendState(BS_NONE);		                // �A���t�@�u�����h����
}

//********************
// �e�L�X�g�i�[(2D�p)
//********************
int CText::OutPutText(std::string text, XMFLOAT2 vPos, XMFLOAT2 vScale, XMFLOAT4 vColor, int nCnt)
{
	int nIndex = -1;
	for (int i = 0; i < MAX_TEXT; ++i)
	{
		if (!m_TextIndexVector[i])
		{
			nIndex = i;
			m_TextIndexVector[i] = true;
			break;
		}
	}
	// �S�Ďg�p�ς�
	if (nIndex < 0)return nIndex;

	TextParam _TextParam;
	_TextParam.bDraw = true;
	_TextParam.text = text;
	_TextParam.vPos = vPos;
	_TextParam.vScale = vScale;
	_TextParam.vColor = vColor;
	_TextParam.nCnt = nCnt;
	_TextParam.bSolid = false;
	_TextParam.vPosSolid = XMFLOAT3(0, 0, 0);// �g�p���Ȃ����ꉞ������
	m_TextMap[nIndex] = _TextParam;

	return nIndex;
}

//********************
// �e�L�X�g�i�[(3D���UI�p)
//********************
int CText::OutPutText(std::string text, XMFLOAT3 vPosSolid, XMFLOAT2 vScale, XMFLOAT4 vColor, int nCnt)
{
	int nIndex = -1;
	for (int i = 0; i < MAX_TEXT; ++i)
	{
		if (!m_TextIndexVector[i])
		{
			nIndex = i;
			m_TextIndexVector[i] = true;
			break;
		}
	}
	// �S�Ďg�p�ς�
	if (nIndex < 0)return nIndex;

	// 3D��2D�ϊ�
	XMMATRIX mProj = XMLoadFloat4x4(&m_pCamera->GetProjMatrix());
	XMMATRIX mView = XMLoadFloat4x4(&m_pCamera->GetViewMatrix());
	XMMATRIX mWorld = XMMatrixIdentity();
	XMVECTOR vecPos = XMVector3Project(
		XMLoadFloat3(&vPosSolid),
		0.0f, 0.0f,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		0.0f, 1.0f,
		mProj,
		mView,
		mWorld);

	XMFLOAT3 vScreenPos;
	XMStoreFloat3(&vScreenPos, vecPos);
	vScreenPos.x -= SCREEN_CENTER_X;
	vScreenPos.y -= SCREEN_CENTER_Y;

	TextParam _TextParam;
	_TextParam.bDraw = true;
	_TextParam.text = text;
	_TextParam.vPos = XMFLOAT2(vScreenPos.x, -vScreenPos.y);
	_TextParam.vScale = vScale;
	_TextParam.vColor = vColor;
	_TextParam.nCnt = nCnt;
	_TextParam.bSolid = true;
	_TextParam.vPosSolid = vPosSolid;

	m_TextMap[nIndex] = _TextParam;

	return nIndex;
}

//********************
// �e�L�X�g�폜
//********************
int CText::DeleteText(int nIndex)
{
	if (nIndex < 0)return nIndex;
	auto itr = m_TextMap.find(nIndex);
	if (itr != m_TextMap.end())         // �L�[�̗v�f�����݂��Ă���ꍇ
	{
		m_TextMap.erase(itr);
		m_TextIndexVector[nIndex] = false;

		return -1;
	}
	return nIndex;
}

void CText::DeleteTextAll()
{
	// �C���f�b�N�X�N���A
	for (int i = 0; i < MAX_TEXT; ++i)
	{
    	m_TextIndexVector[i] = false;	
	}
	
	// �e�L�X�g�N���A
	m_TextMap.clear();
}

void CText::SetPos(int nIndex, XMFLOAT2 vPos)
{
	m_TextMap[nIndex].vPos = vPos;
}

void CText::SetScale(int nIndex, XMFLOAT2 vScale)
{
	m_TextMap[nIndex].vScale = vScale;
}

void CText::SetColor(int nIndex, XMFLOAT4 vColor)
{
	m_TextMap[nIndex].vColor = vColor;
}

//********************
// UV�Z�b�e�B���O
//********************
void CText::UVSet()
{
	int j = 0;
	for (int i = 0; i < TEXT_NUM_X; ++i) {
		m_UVMap[('a' + i)] = XMFLOAT2((1.0f / TEXT_NUM_X) * i, (j * TEXT_NUM_X) / (float)(TEXT_NUM_X * TEXT_NUM_Y));
	}

	j++;
	for (int i = 0; i < TEXT_NUM_X; ++i)
	{
		m_UVMap[('A' + i)] = XMFLOAT2((1.0f / TEXT_NUM_X) * i, (j * TEXT_NUM_X) / (float)(TEXT_NUM_X * TEXT_NUM_Y));
	}

	j++;
	for (int i = 0; i < TEXT_NUM_X; ++i)
	{
		char test;
		test = (char)('0' + i - 16);
		m_UVMap[('0' + i - 16)] = XMFLOAT2((1.0f / TEXT_NUM_X) * i, (j * TEXT_NUM_X) / (float)(TEXT_NUM_X * TEXT_NUM_Y));
	}
	
	int test;
	test = 0;
}

