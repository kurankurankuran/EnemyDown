//////////////////////////////////
//
// CSaveManager�N���X������
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


// �Z�[�u�I�u�W�F�N�g��o�^
void CSaveManager::AddSaveData( CSaveObjBase *pobj )
{
	if(pobj)
		m_ObjList.push_back( pobj );
}

void CSaveManager::ResetSaveData()
{
	m_ObjList.clear();
}


// �Z�[�u
int CSaveManager::Save( string filepath )
{
	// �Z�[�u�t�@�C���I�[�v��
	fs.open( filepath.c_str(), ios_base::out | ios_base::binary );
	if( !fs.is_open() )
	  return 0;

	// �f�[�^�̏�������
	list< CSaveObjBase* >::iterator it;
	for(it = m_ObjList.begin(); it!=m_ObjList.end(); it++)
		(*it)->Save( this );

	fs.close();

	return SAVE_OK;
}



// ���[�h
int CSaveManager::Load( string filepath )
{
   // �Z�[�u�t�@�C���I�[�v��
   fs.open( filepath.c_str(), ios_base::in | ios_base::binary );
   if( !fs.is_open() )
      return NO_SAVEFILE;

   while( !fs.eof() )
   {
		// �f�[�^�̓ǂݍ���
		// �N���XID�̎擾
		int ClassID;
		fs.read( (char*)&ClassID, sizeof(int) );

		// �ǂݍ��ݎ��ɃG���[���N�����ꍇ�̓��[�h���I��
		if(fs.fail() != 0)	break;

		// �w��N���XID�̃I�u�W�F�N�g�𐶐����ĕێ�
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

		// �����I�u�W�F�N�g�������ꍇ�̓f�[�^���΂�
		if(!pobj){	LoadSkip();	continue; }

		// �I�u�W�F�N�g�����[�h
		pobj->Load( this );
   }

   fs.close();

   return LOAD_OK;  // ���[�h����
}



// �ǂݍ��݃f�[�^��1�I�u�W�F�N�g���X�L�b�v
int CSaveManager::LoadSkip()
{
	int type, size;
	while(!fs.eof()){
		fs.read( (char*)&type, sizeof(char) );	// �f�[�^�^�C�v
		if(type == TYPE_END)
			break;
		fs.read( (char*)&size, sizeof(int) );	// �f�[�^�T�C�Y
		fs.ignore(size);			// �T�C�Y���f�[�^���΂�
	}

	return 1;
}


// �t�@�N�g���֐�
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
//   return NULL;   // �w��̃N���X������
//}



// �f�[�^��������
int CSaveManager::Write( DATARECORD* list, CSaveObjBase *pobj )
{
	// �N���XID�̏�������
	//int ID = pobj->GetClassID();
	int ID = pobj->GetObjID();
	fs.write((char*)&ID, sizeof(int) );

	while(list->type != TYPE_END)
	{
		// �f�[�^�̈ʒu���Z�o
		char *pos = (char*)pobj + list->offset;
		// ��������
		fs.write((char*)&list->type, sizeof(char));	// �f�[�^�^�C�v
		switch (list->type)
		{
		case TYPE_LOCAL:
			// �f�[�^�T�C�Y
			fs.write((char*)&list->MemorySize, sizeof(int));	                    
			// �f�[�^�{��
		    fs.write((char*)pos, list->MemorySize);			                    
		    break;
		case TYPE_MEM:
		{
			// �f�[�^�T�C�Y
			fs.write((char*)&list->MemorySize, sizeof(int));
			// �w��A�h���X�ɒ�`����Ă���A��������������vector�Ƃ��Ē�`
			std::vector<char>& vector = *(std::vector<char>*)pos;
			// vector�̓��e����������
			// �f�[�^�{��
			fs.write(reinterpret_cast<char*>(&vector[0]), list->MemorySize);

		}   break;
		default:
		    break;
		}
		
	   // ���̃��X�g��
		list++;
	}

	// �Z�p���[�^��1�͂���
	char sep = TYPE_END;
	fs.write( (char*)&sep, sizeof(char) );

	return SAVE_OK;
}


// �f�[�^�ǂݍ���
int CSaveManager::Read( DATARECORD* list, CSaveObjBase *pobj )
{
	while(list->type != TYPE_END)
	{
		// �f�[�^�̈ʒu���Z�o
		char *pos = (char*)pobj + list->offset;
		// �^�C�v���΂�
		fs.ignore(sizeof(char));
		// �������T�C�Y���擾
		int nSize;
		fs.read((char*)&nSize, sizeof(int));
		switch (list->type)
		{
		case TYPE_LOCAL:
			// �f�[�^�{��
			fs.read((char*)pos, nSize);			    
			break;
		case TYPE_MEM:
		{   
			// �w��A�h���X�ɒ�`����Ă���vector���Q��
			std::vector<char>& vector = *(std::vector<char>*)pos;
			// �������T�C�Y���v�f���m��
			vector.resize(nSize);
			// �f�[�^�{��
			fs.read(reinterpret_cast<char*>(&vector[0]), nSize);			
		}	break;
		default:
			break;
		}
		
		// ���̃��X�g��
		list++;
	}

	// �Z�p���[�^���΂�
	fs.ignore( sizeof(char) );

	return LOAD_OK;
}
