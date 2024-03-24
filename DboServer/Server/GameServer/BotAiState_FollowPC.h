#pragma once

#include "BotAiState.h"
#define NPC_FOLLOW_DISTANCE 5.0f

class CBotAiState_FollowPC : public CBotAiState
{

public:

	CBotAiState_FollowPC(CNpc* pBot, HOBJECT pTargetId);
	virtual	~CBotAiState_FollowPC();

public:

	virtual void OnEnter();
	virtual void OnExit();

	virtual int OnUpdate(DWORD dwTickDiff, float fMultiple);

private:
	HOBJECT m_pTargetId;
};


