#include"ES_PutObj.h"
#include"Input.h"
#include"Text.h"
#include"Scene.h"
#include"Editor_Manager.h"

namespace {
	const XMFLOAT2 TEXT_SCALE = XMFLOAT2(15, 30);
	const float TEXT_POS_X = -SCREEN_CENTER_X + TEXT_SCALE.x;
	const float TEXT_POS_Y = SCREEN_CENTER_Y - 120;
}

CES_PutObj::CES_PutObj()
{
	m_mapESelectTFToString[ESelectTF::PosX] = std::string("PosX");
	m_mapESelectTFToString[ESelectTF::PosY] = std::string("PosY");
	m_mapESelectTFToString[ESelectTF::PosZ] = std::string("PosZ");
	m_mapESelectTFToString[ESelectTF::AngleX] = std::string("AngleX");
	m_mapESelectTFToString[ESelectTF::AngleY] = std::string("AngleY");
	m_mapESelectTFToString[ESelectTF::AngleZ] = std::string("AngleZ");
	m_mapESelectTFToString[ESelectTF::ScaleX] = std::string("ScaleX");
	m_mapESelectTFToString[ESelectTF::ScaleY] = std::string("ScaleY");
	m_mapESelectTFToString[ESelectTF::ScaleZ] = std::string("ScaleZ");

	
}

void CES_PutObj::Init()
{
	m_pTargetGameObj = nullptr;
	m_eSelectTF = ESelectTF::PosX;
	m_eCurPutObjID = EObjID::OI_ROCK;

	m_vectorTextIndex.push_back(CText::OutPutText(std::string("Transform"), XMFLOAT2(TEXT_POS_X, TEXT_POS_Y), TEXT_SCALE));
	m_vectorTextIndex.push_back(CText::OutPutText("Pos", XMFLOAT2(TEXT_POS_X, TEXT_POS_Y - TEXT_SCALE.y), TEXT_SCALE));

	PrintjTransformInfo();

}

void CES_PutObj::Fin()
{
	for (unsigned int i = 0; i < m_vectorTextIndex.size(); ++i)
	{
		CText::DeleteText(m_vectorTextIndex[i]);
	}
	for (unsigned int i = 0; i < m_vectorSelectTextIndex.size(); ++i)
	{
		CText::DeleteText(m_vectorSelectTextIndex[i]);
	}
}

void CES_PutObj::Update()
{
	if (!CEditor_Manager::GetStateSelectFlag())
	{
		// 選択中トランスフォーム切替
		if (CInput::GetKeyTrigger(VK_W))
		{
		    m_eSelectTF = (ESelectTF)((int)m_eSelectTF - 1);
			if ((int)m_eSelectTF < 0)
			{
				// 一番下に行く
				m_eSelectTF = ESelectTF::Max_Select;
			}
		}
		else if (CInput::GetKeyTrigger(VK_S))
		{
			m_eSelectTF = (ESelectTF)((int)m_eSelectTF + 1);
			if ((int)ESelectTF::Max_Select <= (int)m_eSelectTF)
			{
				// 一番上に行く
				m_eSelectTF = (ESelectTF)0;
			}
		}
	}
	

	if (CInput::GetKeyTrigger(VK_SPACE))
	{
		// オブジェクト生成
		CGameObj* pGameObj = CCreateObj::CreateObj(m_eCurPutObjID);
		// カメラの注視点座標
		pGameObj->SetStartPos(CScene::GetCurrent()->GetCameraManager()->GetCamera()->GetTargetPos());
		// カメラの回転量
		pGameObj->SetStartAngle(CScene::GetCurrent()->GetCameraManager()->GetCamera()->GetAngle());
		// スケール
		pGameObj->SetStartScale(XMFLOAT3(1, 1, 1));
		// 初期化　
		pGameObj->Init();

		m_pTargetGameObj = pGameObj;

		PrintjTransformInfo(m_pTargetGameObj);
	}
}

void CES_PutObj::Draw()
{

}

/*
    ターゲットのトランスフォームをテキストで表示する
*/
void CES_PutObj::PrintjTransformInfo(CGameObj* pGameObj)
{
	for (unsigned int i = 0; i < m_vectorSelectTextIndex.size(); ++i)
	{
		CText::DeleteText(m_vectorSelectTextIndex[i]);
	}

	std::string str1;
	std::string str2;
	std::string str3;

	if (pGameObj)
	{
		m_vectorTransFormData[(int)ESelectTF::PosX] = pGameObj->GetTransform()->GetPos().x;
		m_vectorTransFormData[(int)ESelectTF::PosY] = pGameObj->GetTransform()->GetPos().y;
		m_vectorTransFormData[(int)ESelectTF::PosZ] = pGameObj->GetTransform()->GetPos().z;
		m_vectorTransFormData[(int)ESelectTF::AngleX] = pGameObj->GetTransform()->GetAngle().x;
		m_vectorTransFormData[(int)ESelectTF::AngleY] = pGameObj->GetTransform()->GetAngle().y;
		m_vectorTransFormData[(int)ESelectTF::AngleZ] = pGameObj->GetTransform()->GetAngle().z;
		m_vectorTransFormData[(int)ESelectTF::ScaleX] = pGameObj->GetTransform()->GetScale().x;
		m_vectorTransFormData[(int)ESelectTF::ScaleY] = pGameObj->GetTransform()->GetScale().y;
		m_vectorTransFormData[(int)ESelectTF::ScaleZ] = pGameObj->GetTransform()->GetScale().z;


		for (int i = 0; i < (int)ESelectTF::Max_Select; ++i)
		{
			str1 = m_mapESelectTFToString[(ESelectTF)i];
			str2 = std::to_string(m_vectorTransFormData[i]);
			str3 = str1 + str2;
			// テキスト配列に追加
			m_vectorSelectTextIndex.push_back(CText::OutPutText(str3, XMFLOAT2(TEXT_POS_X, TEXT_POS_Y - TEXT_SCALE.y * (i + 2)), TEXT_SCALE));

		}
	}
	else
	{
		for (int i = 0; i < (int)ESelectTF::Max_Select; ++i)
		{
			str1 = m_mapESelectTFToString[(ESelectTF)i];
			str2 = std::to_string(0.0f);
			str3 = str1 + str2;
			// テキスト配列に追加
			m_vectorSelectTextIndex.push_back(CText::OutPutText(str3, XMFLOAT2(TEXT_POS_X, TEXT_POS_Y - TEXT_SCALE.y * (i + 2)), TEXT_SCALE));

		}
	}
}