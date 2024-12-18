//////////////////////////////////
//
// CSaveManager�N���X�錾��
//     2006. 4. 18
//     IKD
//


#pragma once


#include <string>
#include <list>
#include <fstream>
#include "SaveObjBase.h"

class CSaveObjBase;
struct DATARECORD;

using namespace std;

enum SAVE_FLAG
{
	SAVE_OK,
	LOAD_OK,
	NO_SAVEFILE
};


///////////////////////////
//
// �Z�[�u�Ǘ��N���X
//
class CSaveManager
{
protected:
	list<CSaveObjBase*> m_ObjList;
	fstream fs;

public:
	CSaveManager(void);

public:
	virtual ~CSaveManager(void);
	int Save(string filepath);
	int Load(string filepath);
	void AddSaveData(CSaveObjBase *pobj);
	void ResetSaveData();
	//virtual CSaveObjBase *CreateObj(int classID);
	int Write(DATARECORD* list, CSaveObjBase *pobj);
	int Read(DATARECORD* list, CSaveObjBase *pobj);

protected:
	int LoadSkip();		// �ǂݍ��݃f�[�^��1�X�L�b�v
};
