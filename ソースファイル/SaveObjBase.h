//////////////////////////////////
//
// CSaveObjBaseクラス宣言部
//     2006. 4. 18
//     IKD
//


#pragma once

#include"SaveManager.h"

#define TYPE_END (0)
#define TYPE_LOCAL (1)
#define TYPE_MEM (2)


#define DATA_END { TYPE_END, 0, 0}

#define DATA_LOCAL( CLASSNAME, MEMBERNAME ) \
{\
    TYPE_LOCAL,  \
    ( (__int64)&((CLASSNAME*)0)->MEMBERNAME ),           \
    sizeof( ((CLASSNAME*)0)->MEMBERNAME )            \
}

#define DATA_MEM(CLASSNAME, POINTERNAME, SIZENUM) \
{\
    TYPE_MEM,  \
    ( (__int64)&((CLASSNAME*)0)->POINTERNAME ),           \
    SIZENUM/*( (__int64)&((CLASSNAME*)0)->SIZENAME ),*/            \
}

// 列挙型
// 生成するオブジェクトの種類ごとのID
enum EObjID
{
	OI_NONE = 0,        // 設定無し
	OI_PLAYER,          // プレイヤー
	OI_PLADIN,          // 騎士モデル
	OI_ENEMY_MANAGER,   // 敵マネージャー
	OI_ENEMY_MUTANT,    // 敵ミュータント
	OI_MUTANT,          // ミュータントモデル
	OI_ENEMY_WIZARD,    // 敵魔法使い
	OI_WIZARD,          // 魔法使いモデル
	OI_ENEMY_MARIA,     // 敵マリア
	OI_MARIA,           // マリアモデル
	OI_LAND,            // 地形
	OI_CLIFF,           // 崖
	OI_ROCK,            // 岩
	OI_DEATHOBJ,        // 即死オブジェクト

	MAX_OBJID
};



/////////////////////////////////
//
// セーブデータオフセット構造体
//
struct DATARECORD
{
	char type;
	int offset;
	int MemorySize;
};


class CSaveManager;

///////////////////////////
//
// セーブ基底クラス
//
class CSaveObjBase
{
	friend class CSaveManager;

public:
	CSaveObjBase(void);
public:
	virtual ~CSaveObjBase(void);
	virtual DATARECORD *GetDataRecord() = 0;
	EObjID GetObjID() { return m_eObjID; }
	void SetObjID(EObjID eObjID) { m_eObjID = eObjID; }
	//virtual int GetClassID() = 0;
protected:
	virtual int Save(CSaveManager* mgr);
	virtual int Load(CSaveManager* mgr);

private:
	EObjID    m_eObjID;     // オブジェクトID
};
