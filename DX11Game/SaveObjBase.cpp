//////////////////////////////////
//
// CSaveObjBase�N���X������
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


// �Z�[�u
int CSaveObjBase::Save( CSaveManager* mgr )
{
   return mgr->Write( GetDataRecord(), this );   // ��������
}


// ���[�h
int CSaveObjBase::Load( CSaveManager* mgr )
{
   return mgr->Read( GetDataRecord(), this );   // �ǂݍ���
}