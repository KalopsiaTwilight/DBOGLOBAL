#pragma once

#include "CommandsBase.h"
#include "gm.h"

#define MAX_LOOKUP_RESULTS 25

class LookupCommands : CommandsBase
{
	public:
		static ACMD(LookupEntry);
		static ACMD(LookupItem);
		static ACMD(LookupNPC);
		static ACMD(LookupMob);
		static ACMD(LookupTitle);
		static ACMD(LookupSkill);
};

