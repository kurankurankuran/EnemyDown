#include "EWSetting_Base.h"
#include "Text.h"

namespace {
	const XMFLOAT2 TEXT_SCALE = XMFLOAT2(60, 120);
	const char* TEXT_TOTALWAVENUM_WORD = "TotalWaveNum %d ";
	const XMFLOAT2 TEXT_TOTALWAVENUM_POS = XMFLOAT2(0, 0);
	const char* TEXT_CURWAVENUM_WORD = "CurWaveNum %d ";
	const XMFLOAT2 TEXT_CURWAVENUM_POS = XMFLOAT2(0, -60);
}

CEWSetting_Base::CEWSetting_Base()
{
	m_nTotalWaveNum = 1;
	m_nCurWaveNum = 1;
	m_nEnemyNum = 0;
	m_Type = EModelType::MT_MUTANT;
	m_vPos = XMFLOAT3(0, 0, 0);
}

CEWSetting_Base::~CEWSetting_Base()
{

}

void CEWSetting_Base::Init()
{
	m_mapTextIndex[ETextIndex::TotalWaveNum] = CText::OutPutText(std::string(TEXT_TOTALWAVENUM_WORD, m_nTotalWaveNum), TEXT_TOTALWAVENUM_POS, TEXT_SCALE);
	m_mapTextIndex[ETextIndex::CurWaveNum] = CText::OutPutText(std::string(TEXT_CURWAVENUM_WORD, m_nCurWaveNum), TEXT_CURWAVENUM_POS, TEXT_SCALE);
}

void CEWSetting_Base::Fin()
{
	CText::DeleteTextAll();
}

void CEWSetting_Base::Update()
{
	
}

//void CEditor::Draw()
//{
//	
//}

void CEWSetting_Base::UpdateTotalWaveNum()
{
	CText::DeleteText(m_mapTextIndex[ETextIndex::TotalWaveNum]);
	m_mapTextIndex[ETextIndex::TotalWaveNum] = CText::OutPutText(std::string(TEXT_TOTALWAVENUM_WORD, m_nTotalWaveNum), TEXT_TOTALWAVENUM_POS, TEXT_SCALE);
}

void CEWSetting_Base::UpdateCurWaveNum()
{
	CText::DeleteText(m_mapTextIndex[ETextIndex::CurWaveNum]);
	m_mapTextIndex[ETextIndex::CurWaveNum] = CText::OutPutText(std::string(TEXT_CURWAVENUM_WORD, m_nCurWaveNum), TEXT_CURWAVENUM_POS, TEXT_SCALE);
}
//void CEditor::UpdateEnemyNum()
//{
//	CText::DeleteText(m_mapTextIndex[ETextIndex::TotalWaveNum]);
//	m_mapTextIndex[ETextIndex::TotalWaveNum] = CText::OutPutText(std::string(TEXT_TOTALWAVENUM_WORD, m_nTotalWaveNum), TEXT_TOTALWAVENUM_POS, TEXT_SCALE);
//}
//
//void CEditor::UpdateType();
//void CEditor::UpdatePos();
