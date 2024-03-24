#pragma once
#include "CommandsBase.h"
#include "gm.h"

class MiscCommands : CommandsBase
{
	public:
		static ACMD(Entry);
	private:
		static ACMD(Handle_GPS);
};

