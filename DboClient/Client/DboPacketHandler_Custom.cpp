#include "precomp_dboclient.h"
#include "DboPacketHandler.h"

// simulation
#include "NtlNetSender.h"
#include "NtlSLEventFunc.h"
#include "NtlSLPacketGenerator.h"
#include "NtlSobNpcAttr.h"
#include "NtlSobMonsterAttr.h"

#include "NPCTable.h"
#include "MobTable.h"

void PacketHandler_TSChatMsgEmote(void* pPacket)
{
	sTU_CHAT_MESSAGE_EMOTE* pMsgSay = (sTU_CHAT_MESSAGE_EMOTE*)pPacket;

	// 블랙리스트(차단) 체크 
	if (GetNtlSLGlobal()->GetSobAvatar()->GetBlackList()->GetMemberbyName(pMsgSay->awchSenderCharName))
		return;

	CDboEventGenerator::SayMessage(CHAT_TYPE_EMOTE, pMsgSay->awchSenderCharName, pMsgSay->wMessageLengthInUnicode, pMsgSay->awchMessage, pMsgSay->hSubject);
}

void PacketHandler_GUNpcSay(void* pPacket)
{
	sGU_NPC_SAY* pMsgNpcSay = (sGU_NPC_SAY*)pPacket;

	CNtlSob* pObject = GetNtlSobManager()->GetSobObject(pMsgNpcSay->hSubject);
	if (!pObject)
		return;

	const WCHAR* npcName;

	if (pObject->GetClassID() == SLCLASS_NPC)
	{
		CNtlSobNpcAttr* pNPCAttr = reinterpret_cast<CNtlSobNpcAttr*>(pObject->GetSobAttr());
		sNPC_TBLDAT* pNPCData = pNPCAttr->GetNpcTbl();
		CTextTable* pNPCText = API_GetTableContainer()->GetTextAllTable()->GetNPCTbl();
		npcName = pNPCText->GetText(pNPCData->Name).c_str();
	}
	else if (pObject->GetClassID() == SLCLASS_MONSTER) {
		CNtlSobMonsterAttr* pMobAttr = reinterpret_cast<CNtlSobMonsterAttr*>(pObject->GetSobAttr());
		sMOB_TBLDAT* pMobData = pMobAttr->GetMobTbl();
		CTextTable* pMobText = API_GetTableContainer()->GetTextAllTable()->GetMobTbl();
		npcName = pMobText->GetText(pMobData->Name).c_str();
	}
	else {
		return;
	}

	CDboEventGenerator::NpcMsgEvent(CHAT_TYPE_GENERAL, npcName, pMsgNpcSay->wTextLen, pMsgNpcSay->msgText, pMsgNpcSay->hSubject);
}

void PacketHandler_GUNpcEmote(void* pPacket)
{
	sGU_NPC_EMOTE* pMsgNpcSay = (sGU_NPC_EMOTE*)pPacket;

	CNtlSob* pObject = GetNtlSobManager()->GetSobObject(pMsgNpcSay->hSubject);
	if (!pObject)
		return;

	const WCHAR* npcName;

	if (pObject->GetClassID() == SLCLASS_NPC)
	{
		CNtlSobNpcAttr* pNPCAttr = reinterpret_cast<CNtlSobNpcAttr*>(pObject->GetSobAttr());
		sNPC_TBLDAT* pNPCData = pNPCAttr->GetNpcTbl();
		CTextTable* pNPCText = API_GetTableContainer()->GetTextAllTable()->GetNPCTbl();
		npcName = pNPCText->GetText(pNPCData->Name).c_str();
	}
	else if (pObject->GetClassID() == SLCLASS_MONSTER) {
		CNtlSobMonsterAttr* pMobAttr = reinterpret_cast<CNtlSobMonsterAttr*>(pObject->GetSobAttr());
		sMOB_TBLDAT* pMobData = pMobAttr->GetMobTbl();
		CTextTable* pMobText = API_GetTableContainer()->GetTextAllTable()->GetMobTbl();
		npcName = pMobText->GetText(pMobData->Name).c_str();
	}
	else {
		return;
	}

	CDboEventGenerator::NpcMsgEvent(CHAT_TYPE_EMOTE, npcName, pMsgNpcSay->wTextLen, pMsgNpcSay->msgText, pMsgNpcSay->hSubject);
}

void PacketHandler_GUNpcShout(void* pPacket)
{
	sGU_NPC_SHOUT* pMsgNpcSay = (sGU_NPC_SHOUT*)pPacket;

	CNtlSob* pObject = GetNtlSobManager()->GetSobObject(pMsgNpcSay->hSubject);
	if (!pObject)
		return;

	const WCHAR* npcName;

	if (pObject->GetClassID() == SLCLASS_NPC)
	{
		CNtlSobNpcAttr* pNPCAttr = reinterpret_cast<CNtlSobNpcAttr*>(pObject->GetSobAttr());
		sNPC_TBLDAT* pNPCData = pNPCAttr->GetNpcTbl();
		CTextTable* pNPCText = API_GetTableContainer()->GetTextAllTable()->GetNPCTbl();
		npcName = pNPCText->GetText(pNPCData->Name).c_str();
	}
	else if (pObject->GetClassID() == SLCLASS_MONSTER) {
		CNtlSobMonsterAttr* pMobAttr = reinterpret_cast<CNtlSobMonsterAttr*>(pObject->GetSobAttr());
		sMOB_TBLDAT* pMobData = pMobAttr->GetMobTbl();
		CTextTable* pMobText = API_GetTableContainer()->GetTextAllTable()->GetMobTbl();
		npcName = pMobText->GetText(pMobData->Name).c_str();
	}
	else {
		return;
	}

	CDboEventGenerator::NpcMsgEvent(CHAT_TYPE_SHOUT, npcName, pMsgNpcSay->wTextLen, pMsgNpcSay->msgText, pMsgNpcSay->hSubject);
}