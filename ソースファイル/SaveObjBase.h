//////////////////////////////////
//
// CSaveObjBase�N���X�錾��
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

// �񋓌^
// ��������I�u�W�F�N�g�̎�ނ��Ƃ�ID
enum EObjID
{
	OI_NONE = 0,        // �ݒ薳��
	OI_PLAYER,          // �v���C���[
	OI_PLADIN,          // �R�m���f��
	OI_ENEMY_MANAGER,   // �G�}�l�[�W���[
	OI_ENEMY_MUTANT,    // �G�~���[�^���g
	OI_MUTANT,          // �~���[�^���g���f��
	OI_ENEMY_WIZARD,    // �G���@�g��
	OI_WIZARD,          // ���@�g�����f��
	OI_ENEMY_MARIA,     // �G�}���A
	OI_MARIA,           // �}���A���f��
	OI_LAND,            // �n�`
	OI_CLIFF,           // �R
	OI_ROCK,            // ��
	OI_DEATHOBJ,        // �����I�u�W�F�N�g

	MAX_OBJID
};



/////////////////////////////////
//
// �Z�[�u�f�[�^�I�t�Z�b�g�\����
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
// �Z�[�u���N���X
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
	EObjID    m_eObjID;     // �I�u�W�F�N�gID
};
