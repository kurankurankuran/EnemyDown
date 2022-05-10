//=============================================================================
//
// エディター操作（オブジェクト配置）インスタンス クラス定義 [ES_PutObj.h]
// Author : KAZUKI KURA
//
//=============================================================================
#pragma once
#include"ES_Base.h"
#include"GameObj.h"
#include"CreateObj.h"
#include<vector>
#include<map>

class CES_PutObj : public CES_Base
{
public:
	CES_PutObj();


	void Init();
	void Fin();
	void Update();
	void Draw();

	void PrintjTransformInfo(CGameObj* pGameObj = nullptr);
private:
	// 画面表示ステート列挙
	enum class ESelectTF {
		PosX = 0,
		PosY,
		PosZ,
		AngleX,
		AngleY,
		AngleZ,
		ScaleX,
		ScaleY,
		ScaleZ,

		Max_Select
	};

private:
	ESelectTF m_eSelectTF;
    std::map<ESelectTF, std::string> m_mapESelectTFToString;
	float m_vectorTransFormData[(int)ESelectTF::Max_Select];
	EObjID m_eCurPutObjID;
	CGameObj* m_pTargetGameObj;
	//int m_nCurSelectIndex;
	std::vector<int> m_vectorTextIndex;
	std::vector<int> m_vectorSelectTextIndex;
};
