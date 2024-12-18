//////////////////////////////////
//
// CSaveObjBaseクラス実装部
//     2006. 4. 18
//     IKD
//


//#include "StdAfx.h"
#include "SaveObjBase.h"

CSaveObjBase::CSaveObjBase(void)
{
	m_eObjID = EObjID::OI_NONE;
}

CSaveObjBase::~CSaveObjBase(void)
{
}


// セーブ
int CSaveObjBase::Save( CSaveManager* mgr )
{
   return mgr->Write( GetDataRecord(), this );   // 書き込み
}


// ロード
int CSaveObjBase::Load( CSaveManager* mgr )
{
   return mgr->Read( GetDataRecord(), this );   // 読み込み
}