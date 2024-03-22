#include "stdafx.h"
#include "CharacterCommands.h"
#include "comutils.h"

ACMD(CharacterCommands::SetSpeed)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	float fSpeed;

	if (!comutils::parse_float(strToken, fSpeed)) {
		do_feedback(pPlayer, L"The $speed argument must be a valid floating point number. '%s' is not valid.", strToken);
		return;
	}

	pPlayer->UpdateMoveSpeed(fSpeed, fSpeed);

	do_feedback(pPlayer, L"Your movement speed has been set to '%f'.", fSpeed);
}