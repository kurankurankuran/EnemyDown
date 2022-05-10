//////////////////////////////////
//
// CSaveManagerクラス実装部
//     2006. 4. 18
//     IKD
//


//#include "StdAfx.h"
#include "SaveManager.h"
#include "Scene.h"
//#include "Player.h"
//#include "EnemyManager.h"
#include "CreateObj.h"
#include "Model.h"

CSaveManager::CSaveManager(void)
{
}

CSaveManager::~CSaveManager(void)
{
}


// セーブオブジェクトを登録
void CSaveManager::AddSaveData( CSaveObjBase *pobj )
{
	if(pobj)
		m_ObjList.push_back( pobj );
}

void CSaveManager::ResetSaveData()
{
	m_ObjList.clear();
}


// セーブ
int CSaveManager::Save( string filepath )
{
	// セーブファイルオープン
	fs.open( filepath.c_str(), ios_base::out | ios_base::binary );
	if( !fs.is_open() )
	  return 0;

	// データの書き込み
	list< CSaveObjBase* >::iterator it;
	for(it = m_ObjList.begin(); it!=m_ObjList.end(); it++)
		(*it)->Save( this );

	fs.close();

	return SAVE_OK;
}



// ロード
int CSaveManager::Load( string filepath )
{
   // セーブファイルオープン
   fs.open( filepath.c_str(), ios_base::in | ios_base::binary );
   if( !fs.is_open() )
      return NO_SAVEFILE;

   while( !fs.eof() )
   {
		// データの読み込み
		// クラスIDの取得
		int ClassID;
		fs.read( (char*)&ClassID, sizeof(int) );

		// 読み込み時にエラーが起きた場合はロードを終了
		if(fs.fail() != 0)	break;

		// 指定クラスIDのオブジェクトを生成して保持
		CSaveObjBase *pobj;
		pobj = (CSaveObjBase*)CCreateObj::CreateObj(static_cast<EObjID>(ClassID))/*CreateObj( ClassID )*/;

		//if(ClassID != EObjID::OI_NONE)
		//{
		//	pobj = (CSaveObjBase*)CCreateObj::CreateObj(static_cast<EObjID>(ClassID))/*CreateObj( ClassID )*/;
		//}
		//else
		//{
		//	//pobj = (CSaveObjBase*)CCreateObj::CreateObj(static_cast<EModelType>(ClassID))/*CreateObj( ClassID )*/;
		//}
		AddSaveData( pobj );

		// 生成オブジェクトが無い場合はデータを飛ばす
		if(!pobj){	LoadSkip();	continue; }

		// オブジェクトをロード
		pobj->Load( this );
   }

   fs.close();

   return LOAD_OK;  // ロード完了
}



// 読み込みデータを1オブジェクト分スキップ
int CSaveManager::LoadSkip()
{
	int type, size;
	while(!fs.eof()){
		fs.read( (char*)&type, sizeof(char) );	// データタイプ
		if(type == TYPE_END)
			break;
		fs.read( (char*)&size, sizeof(int) );	// データサイズ
		fs.ignore(size);			// サイズ分データを飛ばす
	}

	return 1;
}


// ファクトリ関数
//CSaveObjBase *CSaveManager::CreateObj( int classID )
//{
//	switch (classID)
//	{
//	case 0:
//		return new CPlayer(CScene::GetCurrent());
//	case 1:
//		return new CEnemyManager(CScene::GetCurrent());
//	default:
//
//		break;
//	}
//
//   return NULL;   // 指定のクラスが無い
//}



// データ書き込み
int CSaveManager::Write( DATARECORD* list, CSaveObjBase *pobj )
{
	// クラスIDの書き込み
	//int ID = pobj->GetClassID();
	int ID = pobj->GetObjID();
	fs.write((char*)&ID, sizeof(int) );

	while(list->type != TYPE_END)
	{
		// データの位置を算出
		char *pos = (char*)pobj + list->offset;
		// 書き込み
		fs.write((char*)&list->type, sizeof(char));	// データタイプ
		switch (list->type)
		{
		case TYPE_LOCAL:
			// データサイズ
			fs.write((char*)&list->MemorySize, sizeof(int));	                    
			// データ本体
		    fs.write((char*)pos, list->MemorySize);			                    
		    break;
		case TYPE_MEM:
		{
			// データサイズ
			fs.write((char*)&list->MemorySize, sizeof(int));
			// 指定アドレスに定義されている連続したメモリをvectorとして定義
			std::vector<char>& vector = *(std::vector<char>*)pos;
			// vectorの内容を書き込む
			// データ本体
			fs.write(reinterpret_cast<char*>(&vector[0]), list->MemorySize);

		}   break;
		default:
		    break;
		}
		
	   // 次のリストへ
		list++;
	}

	// セパレータを1つはさむ
	char sep = TYPE_END;
	fs.write( (char*)&sep, sizeof(char) );

	return SAVE_OK;
}


// データ読み込み
int CSaveManager::Read( DATARECORD* list, CSaveObjBase *pobj )
{
	while(list->type != TYPE_END)
	{
		// データの位置を算出
		char *pos = (char*)pobj + list->offset;
		// タイプを飛ばす
		fs.ignore(sizeof(char));
		// メモリサイズを取得
		int nSize;
		fs.read((char*)&nSize, sizeof(int));
		switch (list->type)
		{
		case TYPE_LOCAL:
			// データ本体
			fs.read((char*)pos, nSize);			    
			break;
		case TYPE_MEM:
		{   
			// 指定アドレスに定義されているvectorを参照
			std::vector<char>& vector = *(std::vector<char>*)pos;
			// メモリサイズ分要素を確保
			vector.resize(nSize);
			// データ本体
			fs.read(reinterpret_cast<char*>(&vector[0]), nSize);			
		}	break;
		default:
			break;
		}
		
		// 次のリストへ
		list++;
	}

	// セパレータを飛ばす
	fs.ignore( sizeof(char) );

	return LOAD_OK;
}
