#include "stdafx.h"
#include "LookupCommands.h"
#include "comutils.h"
#include "TableContainerManager.h"
#include "TextAllTable.h"
#include "CharTitleTable.h"

ACMD(LookupCommands::LookupEntry)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);

	if (strToken.rfind("item", 0) == 0) {
		LookupItem(pPlayer, pToken, iLine);
		return;
	}

	if (strToken.rfind("npc", 0) == 0) {
		LookupNPC(pPlayer, pToken, iLine);
		return;
	}

	if (strToken.rfind("mob", 0) == 0) {
		LookupMob(pPlayer, pToken, iLine);
		return;
	}

	if (strToken.rfind("title", 0) == 0) {
		LookupTitle(pPlayer, pToken, iLine);
		return;
	}

	if (strToken.rfind("skill", 0) == 0) {
		LookupSkill(pPlayer, pToken, iLine);
		return;
	}

	do_feedback(pPlayer, L"Unknown subcommand '%S'.", strToken);
}

ACMD(LookupCommands::LookupItem)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	std::wstring namePart = std::wstring(strToken.begin(), strToken.end());
	transform(namePart.begin(), namePart.end(), namePart.begin(), towlower);

	std::vector<sITEM_TBLDAT*> results;

	CItemTable* itemTable = g_pTableContainer->GetItemTable();
	for (auto it = itemTable->Begin(); it != itemTable->End(); it++) {
		sITEM_TBLDAT* tblDat = (sITEM_TBLDAT*)(it->second);
		std::wstring name = g_pTableContainer->GetTextAllTable()->GetItemTbl()->GetText(tblDat->Name);
		transform(name.begin(), name.end(), name.begin(),towlower);
		if (name.rfind(namePart) != std::string::npos) {
			results.push_back(tblDat);
			if (results.size() == MAX_LOOKUP_RESULTS) {
				break;
			}
		}
	}

	do_feedback(pPlayer, L"Found %u results for items with name containing '%S'.", results.size(), strToken);

	for (sITEM_TBLDAT* result : results) {
		std::wstring name = g_pTableContainer->GetTextAllTable()->GetItemTbl()->GetText(result->Name);
		do_feedback(pPlayer, L"%u - %s", result->tblidx, name.c_str());
	}

	if (results.size() == MAX_LOOKUP_RESULTS) {
		do_feedback(pPlayer, L"Total results were truncated. More results may be available with a more specific search.");
	}
}

ACMD(LookupCommands::LookupNPC)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	std::wstring namePart = std::wstring(strToken.begin(), strToken.end());
	transform(namePart.begin(), namePart.end(), namePart.begin(), towlower);

	std::vector<sNPC_TBLDAT*> results;

	CNPCTable* npcTable = g_pTableContainer->GetNpcTable();
	for (auto it = npcTable->Begin(); it != npcTable->End(); it++) {
		sNPC_TBLDAT* tblDat = (sNPC_TBLDAT*)(it->second);
		std::wstring name = g_pTableContainer->GetTextAllTable()->GetNPCTbl()->GetText(tblDat->Name);
		transform(name.begin(), name.end(), name.begin(), towlower);
		if (name.rfind(namePart) != std::string::npos) {
			results.push_back(tblDat);
			if (results.size() == MAX_LOOKUP_RESULTS) {
				break;
			}
		}
	}

	do_feedback(pPlayer, L"Found %u results for NPCs with name containing '%S'.", results.size(), strToken);

	for (sNPC_TBLDAT* result : results) {
		std::wstring name = g_pTableContainer->GetTextAllTable()->GetNPCTbl()->GetText(result->Name);
		do_feedback(pPlayer, L"%u - %s", result->tblidx, name.c_str());
	}

	if (results.size() == MAX_LOOKUP_RESULTS) {
		do_feedback(pPlayer, L"Total results were truncated. More results may be available with a more specific search.");
	}
}

ACMD(LookupCommands::LookupMob)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	std::wstring namePart = std::wstring(strToken.begin(), strToken.end());
	transform(namePart.begin(), namePart.end(), namePart.begin(), towlower);

	std::vector<sMOB_TBLDAT*> results;

	CMobTable* mobTable = g_pTableContainer->GetMobTable();
	for (auto it = mobTable->Begin(); it != mobTable->End(); it++) {
		sMOB_TBLDAT* tblDat = (sMOB_TBLDAT*)(it->second);
		std::wstring name = g_pTableContainer->GetTextAllTable()->GetMobTbl()->GetText(tblDat->Name);
		transform(name.begin(), name.end(), name.begin(), towlower);
		if (name.rfind(namePart) != std::string::npos) {
			results.push_back(tblDat);
			if (results.size() == MAX_LOOKUP_RESULTS) {
				break;
			}
		}
	}

	do_feedback(pPlayer, L"Found %u results for monsters with name containing '%S'.", results.size(), strToken);

	for (sMOB_TBLDAT* result : results) {
		std::wstring name = g_pTableContainer->GetTextAllTable()->GetMobTbl()->GetText(result->Name);
		do_feedback(pPlayer, L"%u - %s", result->tblidx, name.c_str());
	}

	if (results.size() == MAX_LOOKUP_RESULTS) {
		do_feedback(pPlayer, L"Total results were truncated. More results may be available with a more specific search.");
	}
}

ACMD(LookupCommands::LookupTitle)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	std::wstring namePart = std::wstring(strToken.begin(), strToken.end());
	transform(namePart.begin(), namePart.end(), namePart.begin(), towlower);

	std::vector<sCHARTITLE_TBLDAT*> results;

	CCharTitleTable* titleTable = g_pTableContainer->GetCharTitleTable();
	for (auto it = titleTable->Begin(); it != titleTable->End(); it++) {
		sCHARTITLE_TBLDAT* tblDat = (sCHARTITLE_TBLDAT*)(it->second);
		std::wstring name = g_pTableContainer->GetTextAllTable()->GetCharTitleTbl()->GetText(tblDat->tblNameIndex);
		transform(name.begin(), name.end(), name.begin(), towlower);
		if (name.rfind(namePart) != std::string::npos) {
			results.push_back(tblDat);
			if (results.size() == MAX_LOOKUP_RESULTS) {
				break;
			}
		}
	}

	do_feedback(pPlayer, L"Found %u results for titles with name containing '%S'.", results.size(), strToken);

	for (sCHARTITLE_TBLDAT* result : results) {
		std::wstring name = g_pTableContainer->GetTextAllTable()->GetCharTitleTbl()->GetText(result->tblNameIndex);
		do_feedback(pPlayer, L"%u - %s", result->tblidx, name.c_str());
	}

	if (results.size() == MAX_LOOKUP_RESULTS) {
		do_feedback(pPlayer, L"Total results were truncated. More results may be available with a more specific search.");
	}
}

ACMD(LookupCommands::LookupSkill)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	std::wstring namePart = std::wstring(strToken.begin(), strToken.end());
	transform(namePart.begin(), namePart.end(), namePart.begin(), towlower);

	std::vector<sSKILL_TBLDAT*> results;

	CSkillTable* titleTable = g_pTableContainer->GetSkillTable();
	for (auto it = titleTable->Begin(); it != titleTable->End(); it++) {
		sSKILL_TBLDAT* tblDat = (sSKILL_TBLDAT*)(it->second);
		std::wstring name = g_pTableContainer->GetTextAllTable()->GetSkillTbl()->GetText(tblDat->Skill_Name);
		transform(name.begin(), name.end(), name.begin(), towlower);
		if (name.rfind(namePart) != std::string::npos) {
			results.push_back(tblDat);
			if (results.size() == MAX_LOOKUP_RESULTS) {
				break;
			}
		}
	}

	do_feedback(pPlayer, L"Found %u results for skills with name containing '%S'.", results.size(), strToken);

	for (sSKILL_TBLDAT* result : results) {
		std::wstring name = g_pTableContainer->GetTextAllTable()->GetSkillTbl()->GetText(result->Skill_Name);
		do_feedback(pPlayer, L"%u - %s", result->tblidx, name.c_str());
	}

	if (results.size() == MAX_LOOKUP_RESULTS) {
		do_feedback(pPlayer, L"Total results were truncated. More results may be available with a more specific search.");
	}
}
