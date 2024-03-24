#include "stdafx.h"
#include "BotAiState_FollowPC.h"
#include "BotAiAction_NavMove.h"


CBotAiState_FollowPC::CBotAiState_FollowPC(CNpc* pBot, HOBJECT pTargetId)
	:CBotAiState(pBot, BOTCONTROL_STATE_FOLLOWPC, "BOTCONTROL_STATE_FOLLOWPC")
{
	m_pTargetId = pTargetId;
}

CBotAiState_FollowPC::~CBotAiState_FollowPC()
{
}


void CBotAiState_FollowPC::OnEnter()
{
	GetBot()->SetActionStatusFlag(BOT_ACTIONSTATUS_PATROL, true);
}

void CBotAiState_FollowPC::OnExit()
{
	GetBot()->SetActionStatusFlag(BOT_ACTIONSTATUS_PATROL, false);
}

int CBotAiState_FollowPC::OnUpdate(DWORD dwTickDiff, float fMultiple)
{
	CCharacter* pTarget = g_pObjectManager->GetChar(m_pTargetId);
	if (!pTarget || pTarget->GetWorldID() != GetBot()->GetWorldID()) {
		return COMPLETED;
	}
	if (!GetBot()->IsMoveAble()) //not sure
	{
		return m_status;
	}

	CNtlVector vDestLoc(pTarget->GetCurLoc());
	vDestLoc.y = 0;
	vDestLoc.x -= NPC_FOLLOW_DISTANCE / 2;
	vDestLoc.z -= NPC_FOLLOW_DISTANCE / 2;

	if (GetBot()->IsInRange(vDestLoc, 0) == false)
	{
		RemoveAllSubControl();
		CBotAiAction_NavMove* pNavMove = new CBotAiAction_NavMove(GetBot(), vDestLoc, true, false, CNtlVector::INVALID_XZ, CNtlVector::ZERO, INVALID_FLOAT);
		if (!AddSubControlQueue(pNavMove, true))
			m_status = FAILED;
	} 
	UpdateSubControlQueue(dwTickDiff, fMultiple);

	return m_status;
}
