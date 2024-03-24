#pragma once
#include "CommandsBase.h"
#include "gm.h"

class NpcCommands : CommandsBase
{
	public:
		static ACMD(Entry);
	private:
		static ACMD(HandleSay);
		static ACMD(HandleEmote);
		static ACMD(HandleShout);
		static ACMD(HandleInfo);
		static ACMD(HandleDelete);
		static ACMD(Handle_WalkToPlayer);
		static ACMD(Handle_WalkToCoords);
		static ACMD(Handle_RunToPlayer);
		static ACMD(Handle_RunToCoords);
		static ACMD(Handle_TurnToPlayer);
		static ACMD(Handle_TurnToCoords);
		static ACMD(Handle_Follow);
		static ACMD(Handle_StopFollow);
};

