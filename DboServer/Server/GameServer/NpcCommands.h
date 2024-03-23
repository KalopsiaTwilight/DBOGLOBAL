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
};

