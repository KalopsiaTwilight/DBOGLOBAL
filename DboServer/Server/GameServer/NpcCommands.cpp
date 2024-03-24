#include "stdafx.h"
#include "NpcCommands.h"

#include "Monster.h"
#include "BotAiAction_DestMove.h"
#include "BotAiState_Idle.h"
#include "BotAiState_FollowPC.h"
#include "comutils.h"

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

	if (strToken.rfind("walk", 0) == 0) {
		pToken->PopToPeek();
		strToken = pToken->PeekNextToken(NULL, &iLine);

		if (strToken.rfind("here", 0) == 0) {
			Handle_WalkToPlayer(pPlayer, pToken, iLine);
			return;
		}

		if (strToken.rfind("xyz", 0) == 0) {
			Handle_WalkToCoords(pPlayer, pToken, iLine);
			return;
		}
	}

	if (strToken.rfind("run", 0) == 0) {
		pToken->PopToPeek();
		strToken = pToken->PeekNextToken(NULL, &iLine);

		if (strToken.rfind("here", 0) == 0) {
			Handle_RunToPlayer(pPlayer, pToken, iLine);
			return;
		}

		if (strToken.rfind("xyz", 0) == 0) {
			Handle_RunToCoords(pPlayer, pToken, iLine);
			return;
		}
	}

	if (strToken.rfind("turn", 0) == 0) {
		pToken->PopToPeek();
		strToken = pToken->PeekNextToken(NULL, &iLine);

		if (strToken.rfind("here", 0) == 0) {
			Handle_TurnToPlayer(pPlayer, pToken, iLine);
			return;
		}

		if (strToken.rfind("xyz", 0) == 0) {
			Handle_TurnToCoords(pPlayer, pToken, iLine);
			return;
		}
	}

	if (strToken.rfind("follow", 0) == 0) {
		Handle_Follow(pPlayer, pToken, iLine);
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

static bool Move_Creature(CCharacter* pTarget, CNtlVector loc, bool run)
{
	pTarget->GetBotController()->GetContolObject()->GetPathFinder()->SetDestLoc(loc);
	CBotPathFinder::ePATH_FINDING_RESULT pathfindres = pTarget->GetBotController()->GetContolObject()->GetPathFinder()->PathFind();
	if (pathfindres == CBotPathFinder::PATH_FINDING_RESULT_FAILED || pathfindres == CBotPathFinder::PATH_FINDING_RESULT_SRC_FAILED)
	{
		return false;
	}
	sVECTOR3 destLoc[DBO_MAX_NEXT_DEST_LOC_COUNT];
	BYTE byDestLocCount = pTarget->GetBotController()->GetContolObject()->GetPathFinder()->GetAllNextNavLoc(destLoc);
	CBotAiState_Idle* pState = new CBotAiState_Idle(pTarget->GetBotController()->GetContolObject());
	CBotAiAction_DestMove* action = new CBotAiAction_DestMove(pTarget->GetBotController()->GetContolObject(), -1, byDestLocCount, destLoc, run, false, CNtlVector::ZERO);
	pState->AddSubControlList(action, true);
	pTarget->GetBotController()->ChangeAiState(pState);
	return true;
}

ACMD(NpcCommands::Handle_WalkToPlayer)
{
	CCharacter* pTarget = g_pObjectManager->GetChar(pPlayer->GetTargetHandle());
	if (!pTarget || pTarget->IsPC())
	{
		do_feedback(pPlayer, L"You do not have a valid target selected.");
		return;
	}

	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);

	if (strToken.size() > 0) {
		float fSpeed;

		if (!comutils::parse_float(strToken, fSpeed)) {
			do_feedback(pPlayer, L"The $speed argument must be a valid floating point number. '%S' is not valid.", strToken);
			return;
		}

		pTarget->UpdateMoveSpeed(fSpeed, pTarget->GetRunSpeed());
	}

	if (!Move_Creature(pTarget, pPlayer->GetCurLoc(), false))
	{
		do_feedback(pPlayer, L"Unable to find a path to your current position.");
		return;
	}
}

ACMD(NpcCommands::Handle_WalkToCoords)
{
	CCharacter* pTarget = g_pObjectManager->GetChar(pPlayer->GetTargetHandle());
	if (!pTarget || pTarget->IsPC())
	{
		do_feedback(pPlayer, L"You do not have a valid target selected.");
		return;
	}

	float x, y, z;

	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	if (strToken == "-") {
		pToken->PopToPeek();
		strToken += pToken->PeekNextToken(NULL, &iLine);
	}
	if (!comutils::parse_float(strToken, x)) {
		do_feedback(pPlayer, L"The $x argument must be a valid floating point number. '%S' is not valid.", strToken);
		return;
	}

	pToken->PopToPeek();
	strToken = pToken->PeekNextToken(NULL, &iLine);
	if (strToken == "-") {
		pToken->PopToPeek();
		strToken += pToken->PeekNextToken(NULL, &iLine);
	}
	if (!comutils::parse_float(strToken, y)) {
		do_feedback(pPlayer, L"The $y argument must be a valid floating point number. '%S' is not valid.", strToken);
		return;
	}

	pToken->PopToPeek();
	strToken = pToken->PeekNextToken(NULL, &iLine);
	if (strToken == "-") {
		pToken->PopToPeek();
		strToken += pToken->PeekNextToken(NULL, &iLine);
	}
	if (!comutils::parse_float(strToken, z)) {
		do_feedback(pPlayer, L"The $z argument must be a valid floating point number. '%S' is not valid.", strToken);
		return;
	}

	pToken->PopToPeek();
	strToken = pToken->PeekNextToken(NULL, &iLine);
	if (strToken.size() > 0) {
		float fSpeed;

		if (!comutils::parse_float(strToken, fSpeed)) {
			do_feedback(pPlayer, L"The $speed argument must be a valid floating point number. '%S' is not valid.", strToken);
			return;
		}

		pTarget->UpdateMoveSpeed(fSpeed, pTarget->GetRunSpeed());
	}

	CNtlVector* targetLoc = new CNtlVector(x, y, z);
	
	if (!Move_Creature(pTarget, *targetLoc, false))
	{
		do_feedback(pPlayer, L"Unable to find a path to the position x: '%f2', y: '%f2', z: '%f2'.", x, y, z);
		return;
	}
}

ACMD(NpcCommands::Handle_RunToPlayer)
{
	CCharacter* pTarget = g_pObjectManager->GetChar(pPlayer->GetTargetHandle());
	if (!pTarget || pTarget->IsPC())
	{
		do_feedback(pPlayer, L"You do not have a valid target selected.");
		return;
	}

	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);

	if (strToken.size() > 0) {
		float fSpeed;

		if (!comutils::parse_float(strToken, fSpeed)) {
			do_feedback(pPlayer, L"The $speed argument must be a valid floating point number. '%S' is not valid.", strToken);
			return;
		}

		pTarget->UpdateMoveSpeed(pTarget->GetWalkingSpeed(), fSpeed);
	}

	if (!Move_Creature(pTarget, pPlayer->GetCurLoc(), true))
	{
		do_feedback(pPlayer, L"Unable to find a path to your current position.");
		return;
	}
}

ACMD(NpcCommands::Handle_RunToCoords)
{
	CCharacter* pTarget = g_pObjectManager->GetChar(pPlayer->GetTargetHandle());
	if (!pTarget || pTarget->IsPC())
	{
		do_feedback(pPlayer, L"You do not have a valid target selected.");
		return;
	}

	float x, y, z;

	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	if (strToken == "-") {
		pToken->PopToPeek();
		strToken += pToken->PeekNextToken(NULL, &iLine);
	}
	if (!comutils::parse_float(strToken, x)) {
		do_feedback(pPlayer, L"The $x argument must be a valid floating point number. '%S' is not valid.", strToken);
		return;
	}

	pToken->PopToPeek();
	strToken = pToken->PeekNextToken(NULL, &iLine);
	if (strToken == "-") {
		pToken->PopToPeek();
		strToken += pToken->PeekNextToken(NULL, &iLine);
	}
	if (!comutils::parse_float(strToken, y)) {
		do_feedback(pPlayer, L"The $y argument must be a valid floating point number. '%S' is not valid.", strToken);
		return;
	}

	pToken->PopToPeek();
	strToken = pToken->PeekNextToken(NULL, &iLine);
	if (strToken == "-") {
		pToken->PopToPeek();
		strToken += pToken->PeekNextToken(NULL, &iLine);
	}
	if (!comutils::parse_float(strToken, z)) {
		do_feedback(pPlayer, L"The $z argument must be a valid floating point number. '%S' is not valid.", strToken);
		return;
	}

	pToken->PopToPeek();
	strToken = pToken->PeekNextToken(NULL, &iLine);
	if (strToken.size() > 0) {
		float fSpeed;

		if (!comutils::parse_float(strToken, fSpeed)) {
			do_feedback(pPlayer, L"The $speed argument must be a valid floating point number. '%S' is not valid.", strToken);
			return;
		}

		pTarget->UpdateMoveSpeed(pTarget->GetWalkingSpeed(), fSpeed);
	}

	CNtlVector* targetLoc = new CNtlVector(x, y, z);


	if (!Move_Creature(pTarget, *targetLoc, true))
	{
		do_feedback(pPlayer, L"Unable to find a path to the position x: '%f2', y: '%f2', z: '%f2'.", x, y, z);
		return;
	}
}


static bool Turn_Creature(CCharacter* pTarget, CNtlVector toLoc)
{
	CNtlVector vDestDir(toLoc.operator-(pTarget->GetCurLoc()));

	vDestDir.y = 0.0f;
	if (vDestDir.SafeNormalize())
	{
		return Move_Creature(pTarget, pTarget->GetCurLoc() + vDestDir * 0.1f, false);
	}
	return false;
}

ACMD(NpcCommands::Handle_TurnToPlayer)
{
	CCharacter* pTarget = g_pObjectManager->GetChar(pPlayer->GetTargetHandle());
	if (!pTarget || pTarget->IsPC())
	{
		do_feedback(pPlayer, L"You do not have a valid target selected.");
	}


	do_feedback(pPlayer, L"NPC started at: x: %5.2f, z: %5.2f.", pTarget->GetCurDir().x, pTarget->GetCurDir().z);
	if (!Turn_Creature(pTarget, pPlayer->GetCurLoc())) {
		do_feedback(pPlayer, L"Unable to turn NPC.");
	}
	else {
		CNtlVector vDestDir(pPlayer->GetCurLoc().operator-(pTarget->GetCurLoc()));
		do_feedback(pPlayer, L"NPC turnt to: x: %5.2f, z: %5.2f.", pTarget->GetCurDir().x, pTarget->GetCurDir().z);
	}
}

ACMD(NpcCommands::Handle_TurnToCoords)
{
	CCharacter* pTarget = g_pObjectManager->GetChar(pPlayer->GetTargetHandle());
	if (!pTarget || pTarget->IsPC())
	{
		do_feedback(pPlayer, L"You do not have a valid target selected.");
	}

	float x, y, z;

	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	if (strToken == "-") {
		pToken->PopToPeek();
		strToken += pToken->PeekNextToken(NULL, &iLine);
	}
	if (!comutils::parse_float(strToken, x)) {
		do_feedback(pPlayer, L"The $x argument must be a valid floating point number. '%S' is not valid.", strToken);
		return;
	}

	pToken->PopToPeek();
	strToken = pToken->PeekNextToken(NULL, &iLine);
	if (strToken == "-") {
		pToken->PopToPeek();
		strToken += pToken->PeekNextToken(NULL, &iLine);
	}
	if (!comutils::parse_float(strToken, y)) {
		do_feedback(pPlayer, L"The $y argument must be a valid floating point number. '%S' is not valid.", strToken);
		return;
	}

	pToken->PopToPeek();
	strToken = pToken->PeekNextToken(NULL, &iLine);
	if (strToken == "-") {
		pToken->PopToPeek();
		strToken += pToken->PeekNextToken(NULL, &iLine);
	}
	if (!comutils::parse_float(strToken, z)) {
		do_feedback(pPlayer, L"The $z argument must be a valid floating point number. '%S' is not valid.", strToken);
		return;
	}

	CNtlVector* targetLoc = new CNtlVector(x, y, z);

	if (!Turn_Creature(pTarget, *targetLoc)) {
		do_feedback(pPlayer, L"Unable to turn NPC.");
	}
}

ACMD(NpcCommands::Handle_Follow)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	if (!strcmp(strToken.c_str(), "stop"))
	{
		Handle_StopFollow(pPlayer, pToken, iLine);
		return;
	}

	CCharacter* pTarget = g_pObjectManager->GetChar(pPlayer->GetTargetHandle());
	if (!pTarget || pTarget->IsPC())
	{
		do_feedback(pPlayer, L"You do not have a valid target selected.");
	}

	CBotAiState_FollowPC* pState = new CBotAiState_FollowPC(pTarget->GetBotController()->GetContolObject(), pPlayer->GetID());
	pTarget->GetBotController()->ChangeAiState(pState);
}

ACMD(NpcCommands::Handle_StopFollow)
{
	CCharacter* pTarget = g_pObjectManager->GetChar(pPlayer->GetTargetHandle());
	if (!pTarget || pTarget->IsPC())
	{
		do_feedback(pPlayer, L"You do not have a valid target selected.");
	}

	CBotAiState_Idle* pState = new CBotAiState_Idle(pTarget->GetBotController()->GetContolObject());
	pTarget->GetBotController()->ChangeAiState(pState);
}