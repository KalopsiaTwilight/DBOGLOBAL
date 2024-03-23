#include "stdafx.h"
#include "NpcCommands.h"

#include "Monster.h"

ACMD(NpcCommands::Entry)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);

	if (strToken.rfind("say", 0) == 0) {
		HandleSay(pPlayer, pToken, iLine);
		return;
	}

	if (strToken.rfind("emote", 0) == 0) {
		HandleEmote(pPlayer, pToken, iLine);
		return;
	}

	if (strToken.rfind("shout", 0) == 0) {
		HandleShout(pPlayer, pToken, iLine);
		return;
	}

	if (strToken.rfind("info", 0) == 0) {
		HandleInfo(pPlayer, pToken, iLine);
		return;
	}

	if (strToken.rfind("delete", 0) == 0) {
		HandleDelete(pPlayer, pToken, iLine);
		return;
	}

	do_feedback(pPlayer, L"Unknown subcommand '%S'.", strToken);
}

ACMD(NpcCommands::HandleSay) {
	CCharacter* pTarget = g_pObjectManager->GetChar(pPlayer->GetTargetHandle());
	if (!pTarget || pTarget->IsPC())
	{
		do_feedback(pPlayer, L"You do not have a valid target selected.");
	}

	pToken->PopToPeek();
	std::string text = "";

	while (text.length() < NTL_MAX_LENGTH_OF_CHAT_MESSAGE)
	{
		std::string mtext = pToken->PeekNextToken(NULL, &iLine);
		if (mtext == ";" || mtext == "")
		{
			break;
		}
		else
		{
			text += mtext; text += " ";
		}
	}

	if (text.size() == 0) {
		do_feedback(pPlayer, L"You must supply a message to the npc say command.");
		return;
	}

	CNtlPacket packet(sizeof(sGU_NPC_SAY));
	sGU_NPC_SAY* res = (sGU_NPC_SAY*)packet.GetPacketData();
	res->wOpCode = GU_NPC_SAY;
	res->wTextLen = (WORD)text.length();
	res->hSubject = pPlayer->GetTargetHandle();
	wcscpy_s(res->msgText, NTL_MAX_LENGTH_OF_CHAT_MESSAGE + 1, s2ws(text).c_str());
	packet.SetPacketLen(sizeof(sGU_NPC_SAY));
	pTarget->SendPacket(&packet);

	g_pObjectManager->SendToPlayersInRangeFrom(pTarget, &packet, NTL_SAY_MESSAGE_RANGE_IN_METER);
}


ACMD(NpcCommands::HandleEmote) {
	CCharacter* pTarget = g_pObjectManager->GetChar(pPlayer->GetTargetHandle());
	if (!pTarget || pTarget->IsPC())
	{
		do_feedback(pPlayer, L"You do not have a valid target selected.");
	}

	pToken->PopToPeek();
	std::string text = "";

	while (text.length() < NTL_MAX_LENGTH_OF_CHAT_MESSAGE)
	{
		std::string mtext = pToken->PeekNextToken(NULL, &iLine);
		if (mtext == ";" || mtext == "")
		{
			break;
		}
		else
		{
			text += mtext; text += " ";
		}
	}

	if (text.size() == 0) {
		do_feedback(pPlayer, L"You must supply a message to the npc say command.");
		return;
	}

	CNtlPacket packet(sizeof(sGU_NPC_EMOTE));
	sGU_NPC_EMOTE* res = (sGU_NPC_EMOTE*)packet.GetPacketData();
	res->wOpCode = GU_NPC_EMOTE;
	res->wTextLen = (WORD)text.length();
	res->hSubject = pPlayer->GetTargetHandle();
	wcscpy_s(res->msgText, NTL_MAX_LENGTH_OF_CHAT_MESSAGE + 1, s2ws(text).c_str());
	packet.SetPacketLen(sizeof(sGU_NPC_EMOTE));
	pTarget->SendPacket(&packet);

	g_pObjectManager->SendToPlayersInRangeFrom(pTarget, &packet, NTL_SAY_MESSAGE_RANGE_IN_METER);
}


ACMD(NpcCommands::HandleShout) {
	CCharacter* pTarget = g_pObjectManager->GetChar(pPlayer->GetTargetHandle());
	if (!pTarget || pTarget->IsPC())
	{
		do_feedback(pPlayer, L"You do not have a valid target selected.");
	}

	pToken->PopToPeek();
	std::string text = "";

	while (text.length() < NTL_MAX_LENGTH_OF_CHAT_MESSAGE)
	{
		std::string mtext = pToken->PeekNextToken(NULL, &iLine);
		if (mtext == ";" || mtext == "")
		{
			break;
		}
		else
		{
			text += mtext; text += " ";
		}
	}

	if (text.size() == 0) {
		do_feedback(pPlayer, L"You must supply a message to the npc say command.");
		return;
	}

	CNtlPacket packet(sizeof(sGU_NPC_SHOUT));
	sGU_NPC_SHOUT* res = (sGU_NPC_SHOUT*)packet.GetPacketData();
	res->wOpCode = GU_NPC_SHOUT;
	res->wTextLen = (WORD)text.length();
	res->hSubject = pPlayer->GetTargetHandle();
	wcscpy_s(res->msgText, NTL_MAX_LENGTH_OF_CHAT_MESSAGE + 1, s2ws(text).c_str());
	packet.SetPacketLen(sizeof(sGU_NPC_SHOUT));
	pTarget->SendPacket(&packet);

	g_pObjectManager->SendToPlayersInRangeFrom(pTarget, &packet, NTL_SHOUT_MESSAGE_RANGE_IN_METER);
}


ACMD(NpcCommands::HandleDelete) {
	CCharacter* pTarget = g_pObjectManager->GetChar(pPlayer->GetTargetHandle());
	if (pTarget && !pTarget->IsPC())
	{
		g_pObjectManager->DestroyCharacter(pTarget);

		do_feedback(pPlayer, L"Target removed!");
	}
	else {
		do_feedback(pPlayer, L"You do not have a valid target selected.");
	}
}


ACMD(NpcCommands::HandleInfo) {
	CCharacter* pTarget = g_pObjectManager->GetChar(pPlayer->GetTargetHandle());
	if (pTarget && (pTarget->IsMonster() || pTarget->IsNPC()))
	{
		if (pTarget->IsNPC()) {
			CNpc* pNpc = g_pObjectManager->GetNpc(pPlayer->GetTargetHandle());
			sNPC_TBLDAT* tblDat = pNpc->GetTbldat();

			//do_feedback(pPlayer, L"Name: %s", pNpc->GetTbldat()->Name);
			if (tblDat) {
				do_feedback(pPlayer, L"Entry ID: %u", tblDat->tblidx);
			}
			do_feedback(pPlayer, L"Creature ID: %u", pTarget->GetID());
		}
		else
		{
			CMonster* pMob = g_pObjectManager->GetMob(pPlayer->GetTargetHandle());
			sMOB_TBLDAT* tblDat = pMob->GetTbldat();
			if (tblDat) {
				//do_feedback(pPlayer, L"Name: %s", pMob->GetTbldat()->Name);
				do_feedback(pPlayer, L"Entry ID: %u", tblDat->tblidx);
			}
			do_feedback(pPlayer, L"Creature ID: %u", pTarget->GetID());
		}
	}
	else {
		do_feedback(pPlayer, L"You do not have a valid target selected.");
	}
}