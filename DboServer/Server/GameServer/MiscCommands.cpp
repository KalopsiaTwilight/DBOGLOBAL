#include "stdafx.h"
#include "MiscCommands.h"

ACMD(MiscCommands::Entry)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);

	if (strToken.rfind("gps", 0) == 0) {
		Handle_GPS(pPlayer, pToken, iLine);
		return;
	}

	do_feedback(pPlayer, L"Unknown subcommand '%S'.", strToken);
}

ACMD(MiscCommands::Handle_GPS)
{
	do_feedback(pPlayer, L"Your current position is x: '%5.2f', y: '%5.2f', z: '%5.2f'.", pPlayer->GetCurLoc().x, pPlayer->GetCurLoc().y, pPlayer->GetCurLoc().z);
	do_feedback(pPlayer, L"Your orientation is x: '%5.2f', y: '%5.2f', z: '%5.2f'.", pPlayer->GetCurDir().x, pPlayer->GetCurDir().y, pPlayer->GetCurDir().z);
}