//#pragma once
//#include "GameObj.h"
//#include "Model.h"
//#include "Scene.h"
//#include <map>
//#include <vector>
//
//struct ObjData
//{
//	EModelType t_Type;
//	XMFLOAT3   t_vPos;
//
//	ObjData(EModelType eType, XMFLOAT3 vPos)
//	{
//		t_Type = eType;
//		t_vPos = vPos;
//	}
//};
//
//class CCreateGameObj
//{
//private:
//	//static std::map<int/*オブジェ番号*/, ObjData/*情報*/> m_mapObjData;
//
//public:
//	CCreateGameObj();
//	~CCreateGameObj();
//
//	void Init();
//
//	static void Save(EScene SceneNum);
//	static void Load(EScene SceneNum);
//	static void Create();
//	static void CreateMethod(CScene* pScene,EModelType eType, XMFLOAT3 vPos);
//};
