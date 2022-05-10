#pragma once
#include "main.h"
#include <string>
#include <map>
#include <unordered_map>  
#include <vector>
#include "Camera.h"
using namespace std;         

typedef struct {
	bool     bDraw;       // �`�悷��t���O
	string   text;        // �`�悷�镶����
	XMFLOAT2 vPos;        // �X�N���[����ɕ`�悷��ʒu
	XMFLOAT2 vScale;      // �傫��
	XMFLOAT4 vColor;      // �F�i���܂ށj
	int      nCnt;        // �`�掞��(�f�t�H���g�ŉi��)
	bool     bSolid;      // 3D��ԏ�ɕ`�悷�邩
	XMFLOAT3 vPosSolid;   // 3D��ԏ�̈ʒu
}TextParam;

class CText
{
private:
	// �p�����e�N�X�`��
	static ID3D11ShaderResourceView*	m_pTextureText;
	static unordered_map<int/*�e�L�X�g�C���f�b�N�X*/, TextParam/*�e�L�X�g�p�����[�^*/> m_TextMap;
	static vector<bool/*�C���f�b�N�X�ԍ��g�p���t���O*/> m_TextIndexVector;

	// �������Ƃ�UV�i�[�p
	static unordered_map<char/*���͕���*/, XMFLOAT2/*UV*/> m_UVMap;

	static CCamera* m_pCamera;
public:
	CText();
	~CText();

	static HRESULT Init();
	static void Uninit();
	static void Update();
	static void Draw();
	
	// UI�p
	static int OutPutText(std::string text, XMFLOAT2 vPos, XMFLOAT2 vScale, XMFLOAT4 vColor = XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f), int nCnt = -1);
	// 3D���UI�p
	static int OutPutText(std::string text, XMFLOAT3 vPosSolid, XMFLOAT2 vScale, XMFLOAT4 vColor = XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f), int nCnt = -1);
	static int DeleteText(int nIndex);
	static void DeleteTextAll();

	static void SetPos(int nIndex, XMFLOAT2 vPos);
	static void SetScale(int nIndex, XMFLOAT2 vScale);
	static void SetColor(int nIndex, XMFLOAT4 vColor);


	static void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
private:
	static void UVSet();
};
