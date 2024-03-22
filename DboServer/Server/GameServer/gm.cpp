#include "stdafx.h"
#include "gm.h"
#include "GameServer.h"
#include "CPlayer.h"
#include "Monster.h"
#include "ItemManager.h"
#include "NtlPacketUG.h"
#include "NtlPacketGU.h"
#include "NtlPacketGQ.h"
#include "NtlPacketGT.h"
#include "NtlPacketGM.h"
#include "NtlResultCode.h"
#include "NtlMrPoPoMsg.h"
#include "SubNeighborServerInfoManager.h"
#include "NtlTokenizer.h"
#include "TableContainerManager.h"
#include "ItemTable.h"
#include "ExpTable.h"
#include "GameProcessor.h"
#include "HLSItemTable.h"
#include "SystemEffectTable.h"
#include "TriggerObject.h"
#include "GameMain.h"
#include "DragonballScramble.h"
#include "HoneyBeeEvent.h"
#include "ItemDrop.h"
#include "BotAiState_Idle.h"
#include "CharacterCommands.h"
#include "LookupCommands.h"
#include "CharTitleTable.h"
#include "comutils.h"

template<typename ... Args>
static void do_feedback(CPlayer* pTarget, const std::wstring& format, Args ... args)
{
	int size_s = std::swprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
	auto size = static_cast<size_t>(size_s);
	std::unique_ptr<wchar_t[]> buf(new wchar_t[size]);
	std::swprintf(buf.get(), size, format.c_str(), args ...);
	std::wstring text = std::wstring(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside


	CNtlPacket packet(sizeof(sGU_SYSTEM_DISPLAY_TEXT));
	sGU_SYSTEM_DISPLAY_TEXT* res = (sGU_SYSTEM_DISPLAY_TEXT*)packet.GetPacketData();
	res->wOpCode = GU_SYSTEM_DISPLAY_TEXT;
	res->wMessageLengthInUnicode = (WORD)text.length();
	res->byDisplayType = SERVER_TEXT_SYSTEM;
	wcscpy_s(res->awchMessage, NTL_MAX_LENGTH_OF_CHAT_MESSAGE + 1, text.c_str());
	packet.SetPacketLen(sizeof(sGU_SYSTEM_DISPLAY_TEXT));
	pTarget->SendPacket(&packet);
}

void gm_read_command(sUG_SERVER_COMMAND* sPacket, CPlayer* pPlayer)
{
	CGameServer* app = (CGameServer*)g_pApp;

	char chBuffer[1024];
	WideCharToMultiByte(GetACP(), 0, sPacket->awchCommand, -1, chBuffer, 1024, NULL, NULL);

	ERR_LOG(LOG_USER, "User %u uses gm command %s", pPlayer->GetCharID(), chBuffer);

	CNtlTokenizer lexer(chBuffer);
	int iLine;
	int icmd;

	std::string strCommand = lexer.PeekNextToken(NULL, &iLine);

	for (icmd = 0; icmd < NTL_MAX_LENGTH_OF_CHAT_MESSAGE; icmd++)
	{
		if (!strcmp(cmd_info[icmd].command, "@qwasawedsadas"))
		{
			do_feedback(pPlayer, L"No command found with name '%S'.", strCommand.c_str());
			return;
		}
		else if (!strcmp(cmd_info[icmd].command, strCommand.c_str()))
			break;
	}

	if (cmd_info[icmd].eAdminLevel > pPlayer->GetGMLevel()) //check if player gm level high enough to use command
		return;

	if (cmd_info[icmd].eAdminLevel > ADMIN_LEVEL_EARLY_ACCESS && pPlayer->IsGameMaster() == false)
		return;

	//log
	CNtlPacket packetQry(sizeof(sGQ_GM_LOG));
	sGQ_GM_LOG* resQry = (sGQ_GM_LOG*)packetQry.GetPacketData();
	resQry->wOpCode = GQ_GM_LOG;
	resQry->byLogType = 0;
	resQry->charId = pPlayer->GetCharID();
	strcpy(resQry->chBuffer, chBuffer);
	packetQry.SetPacketLen(sizeof(sGQ_GM_LOG)); // to avoid having huge packet
	app->SendTo(app->GetQueryServerSession(), &packetQry);

	((*cmd_info[icmd].command_pointer) (pPlayer, &lexer, iLine));
}

ACMD(do_setspeed);
ACMD(do_addmob);
ACMD(do_addmobgroup);
ACMD(do_addnpc);
ACMD(do_additem);
ACMD(do_addmasteritem); // this function will give
ACMD(do_addskill);
ACMD(do_addskill2); // this function add missing master class passive. Example.. if player is swordsman and dont have swordsman masterclass skill, then this can be used.
ACMD(do_r);
ACMD(do_addhtb);
ACMD(do_setzenny);
ACMD(do_setlevel);
ACMD(do_hide);
ACMD(do_notice);
ACMD(do_pm);
ACMD(do_teleport);
ACMD(do_world);
ACMD(do_warp);
ACMD(do_call);
ACMD(do_shutdown);
ACMD(do_setadult);
ACMD(do_setclass);
ACMD(do_dc);
ACMD(do_kill);
ACMD(do_delallitems);
ACMD(do_god);
ACMD(do_invincible);
ACMD(do_bann);
ACMD(do_dbann);
ACMD(do_purge);
ACMD(do_unstuck);
ACMD(do_warfog);
ACMD(do_upgrade);
ACMD(do_setitemrank);
ACMD(do_mute);
ACMD(do_unmute);
ACMD(do_go);
ACMD(do_addtitle);
ACMD(do_deltitle);
ACMD(do_setitemduration);
ACMD(do_bind);
ACMD(do_exp);
ACMD(do_resetexp);
ACMD(do_starthoneybee);
ACMD(do_stophoneybee);
ACMD(do_deleteguild);
ACMD(do_cancelah);
ACMD(do_addmudosa);
ACMD(do_start);
ACMD(do_createloot);
ACMD(do_test);
ACMD(do_delnpc);
ACMD(do_setnpcfriendly);
ACMD(do_getnpcinfo);

struct command_info cmd_info[] =
{
	{ "@setspeed", CharacterCommands::SetSpeed, ADMIN_LEVEL_GAME_MASTER },
	{ "@addmob", do_addmob, ADMIN_LEVEL_ADMIN },
	{ "@addmobgroup", do_addmobgroup, ADMIN_LEVEL_ADMIN },
	{ "@addnpc", do_addnpc, ADMIN_LEVEL_ADMIN },
	{ "@additem", do_additem, ADMIN_LEVEL_ADMIN },
	{ "@addmasteritem", do_addmasteritem, ADMIN_LEVEL_NONE },
	{ "@addskill", do_addskill, ADMIN_LEVEL_ADMIN },
	{ "@addskill2", do_addskill2, ADMIN_LEVEL_NONE },
	{ "@r", do_r, ADMIN_LEVEL_DEV_SERVER },
	{ "@addhtb", do_addhtb, ADMIN_LEVEL_NONE },
	{ "@setzenny", do_setzenny, ADMIN_LEVEL_ADMIN },
	{ "@setlevel", do_setlevel, ADMIN_LEVEL_ADMIN },
	{ "@hide", do_hide, ADMIN_LEVEL_GAME_MASTER },
	{ "@notice", do_notice, ADMIN_LEVEL_GAME_MASTER },
	{ "@pm", do_pm, ADMIN_LEVEL_GAME_MASTER },
	{ "@teleport", do_teleport, ADMIN_LEVEL_GAME_MASTER },
	{ "@world", do_world, ADMIN_LEVEL_GAME_MASTER },
	{ "@warp", do_warp, ADMIN_LEVEL_GAME_MASTER },
	{ "@call", do_call, ADMIN_LEVEL_GAME_MASTER },
	{ "@shutdown", do_shutdown, ADMIN_LEVEL_ADMIN },
	{ "@setadult", do_setadult, ADMIN_LEVEL_GAME_MASTER },
	{ "@setclass", do_setclass, ADMIN_LEVEL_ADMIN },
	{ "@dc", do_dc, ADMIN_LEVEL_GAME_MASTER },
	{ "@kill", do_kill, ADMIN_LEVEL_ADMIN },
	{ "@delallitems", do_delallitems, ADMIN_LEVEL_GAME_MASTER },
	{ "@god", do_god, ADMIN_LEVEL_ADMIN },
	{ "@invincible", do_invincible, ADMIN_LEVEL_ADMIN },
	{ "@bann", do_bann, ADMIN_LEVEL_GAME_MASTER },
	{ "@dbann", do_dbann, ADMIN_LEVEL_GAME_MASTER },
	{ "@purge", do_purge, ADMIN_LEVEL_COMMUNITY_MANAGER },
	{ "@unstuck", do_unstuck, ADMIN_LEVEL_NONE },
	{ "@warfog", do_warfog, ADMIN_LEVEL_GAME_MASTER },
	{ "@upgrade", do_upgrade, ADMIN_LEVEL_ADMIN },
	{ "@setitemrank", do_setitemrank, ADMIN_LEVEL_ADMIN },
	{ "@mute", do_mute, ADMIN_LEVEL_GAME_MASTER },
	{ "@unmute", do_unmute, ADMIN_LEVEL_GAME_MASTER },
	{ "@go", do_go, ADMIN_LEVEL_GAME_MASTER },
	{ "@addtitle", do_addtitle, ADMIN_LEVEL_GAME_MASTER },
	{ "@deltitle", do_deltitle, ADMIN_LEVEL_GAME_MASTER },
	{ "@setitemduration", do_setitemduration, ADMIN_LEVEL_ADMIN },
	{ "@bind", do_bind, ADMIN_LEVEL_GAME_MASTER },
	{ "@exp", do_exp, ADMIN_LEVEL_NONE },
	{ "@resetexp", do_resetexp, ADMIN_LEVEL_NONE },
	{ "@starthoneybee", do_starthoneybee, ADMIN_LEVEL_COMMUNITY_MANAGER },
	{ "@stophoneybee", do_stophoneybee, ADMIN_LEVEL_COMMUNITY_MANAGER },
	{ "@deleteguild", do_deleteguild, ADMIN_LEVEL_COMMUNITY_MANAGER },
	{ "@cancelah", do_cancelah, ADMIN_LEVEL_GAME_MASTER },
	{ "@addmudosa", do_addmudosa, ADMIN_LEVEL_ADMIN },
	{ "@startgm", do_start, ADMIN_LEVEL_GAME_MASTER },
	{ "@createloot", do_createloot, ADMIN_LEVEL_ADMIN },
	{ "@test", do_test, ADMIN_LEVEL_ADMIN },

	{ "@delnpc", do_delnpc, ADMIN_LEVEL_ADMIN },
	{ "@setfriendly", do_setnpcfriendly, ADMIN_LEVEL_ADMIN },
	{ "@npcinfo", do_getnpcinfo, ADMIN_LEVEL_ADMIN },
	{ "@lookup", LookupCommands::LookupEntry, ADMIN_LEVEL_ADMIN },

	{ "@qwasawedsadas", NULL, ADMIN_LEVEL_ADMIN }
};


ACMD(do_setspeed)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	float fSpeed;

	if (!comutils::parse_float(strToken, fSpeed)) {
		do_feedback(pPlayer, L"The $speed argument must be a valid floating point number. '%S' is not valid.", strToken.c_str());
		return;
	}

	pPlayer->UpdateMoveSpeed(fSpeed, fSpeed);

	do_feedback(pPlayer, L"Your movement speed has been set to '%f'.", fSpeed);
}

ACMD(do_addmob)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	TBLIDX MobId;
	if (!comutils::parse_uint(strToken, MobId)) {
		do_feedback(pPlayer, L"Monster entry ids must be whole numbers '%S' is not valid.", strToken.c_str());
		return;
	}

	sMOB_TBLDAT* pMOBTblData = (sMOB_TBLDAT*)g_pTableContainer->GetMobTable()->FindData(MobId);
	if (!pMOBTblData)
	{
		do_feedback(pPlayer, L"Could not find mob with entry id: '%u'.", MobId);
		return;
	}

	sVECTOR3 spawnloc;
	spawnloc.x = pPlayer->GetCurLoc().x + rand() % 5;
	spawnloc.y = 0.0f;// pPlayer->GetCurLoc().y;
	spawnloc.z = pPlayer->GetCurLoc().z + rand() % 5;

	sVECTOR3 spawndir;
	spawndir.x = pPlayer->GetCurDir().x + rand() % 5;
	spawndir.y = 0.0f;// pPlayer->GetCurDir().y;
	spawndir.z = pPlayer->GetCurDir().z + rand() % 5;

	sSPAWN_TBLDAT sMobSpawn;
	sMobSpawn.vSpawn_Dir.CopyFrom(spawndir);
	sMobSpawn.vSpawn_Loc.CopyFrom(spawnloc);
	sMobSpawn.dwParty_Index = INVALID_DWORD;
	sMobSpawn.byMove_Range = 30;
	sMobSpawn.bySpawn_Move_Type = SPAWN_MOVE_WANDER;
	sMobSpawn.bySpawn_Loc_Range = 30;
	sMobSpawn.byWander_Range = 30;
	sMobSpawn.path_Table_Index = INVALID_TBLIDX;
	sMobSpawn.playScript = INVALID_TBLIDX;
	sMobSpawn.playScriptScene = INVALID_TBLIDX;
	sMobSpawn.aiScript = INVALID_TBLIDX;
	sMobSpawn.aiScriptScene = INVALID_TBLIDX;
	sMobSpawn.actionPatternTblidx = 1;

	printf("dwFormulaOffset: %u, wUseRace: %u, wMonsterClass: %u, LP:%u, fSettingRate_LP:%f, EP:%u, Str:%u, Con:%u, Foc:%u, Dex:%u, Sol:%u, Eng:%u\n",
		pMOBTblData->dwFormulaOffset, pMOBTblData->wUseRace, pMOBTblData->wMonsterClass, pMOBTblData->dwBasic_LP, pMOBTblData->fSettingRate_LP,
		pMOBTblData->wBasic_EP,
		pMOBTblData->wBasicStr, pMOBTblData->wBasicCon, pMOBTblData->wBasicFoc, pMOBTblData->wBasicDex, pMOBTblData->wBasicSol, pMOBTblData->wBasicEng);

	CMonster* pMob = (CMonster*)g_pObjectManager->CreateCharacter(OBJTYPE_MOB);
	pMob->CreateDataAndSpawn(pPlayer->GetWorldID(), pMOBTblData, &sMobSpawn, false, 0);
	do_feedback(pPlayer, L"Spawned a new mob with entry id: '%u'", MobId);
}

ACMD(do_addmobgroup)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	TBLIDX groupId;
	if (!comutils::parse_uint(strToken, groupId)) {
		do_feedback(pPlayer, L"Mobgroup entry ids must be whike numbers '%S' is not valid.", strToken.c_str());
		return;
	}

	DWORD spawnCount = g_pTableContainer->GetMobSpawnTable(pPlayer->GetCurWorld()->GetIdx())->GetSpawnGroupCount(groupId);
	sSPAWN_TBLDAT* spawnTbldat = g_pTableContainer->GetMobSpawnTable(pPlayer->GetCurWorld()->GetIdx())->GetSpawnGroupFirst(groupId);

	if (!spawnTbldat) {
		do_feedback(pPlayer, L"Could not find mobgroup with entryId '%u'.", groupId);
	}

	for (DWORD i = 0; i < spawnCount; i++)
	{
		if (spawnTbldat)
		{
			for (int ii = 0; ii < spawnTbldat->bySpawn_Quantity; ii++)
			{
				sMOB_DATA data;
				InitMobData(data);

				data.spawnGroupId = groupId;
				data.worldID = pPlayer->GetCurWorld()->GetID();
				data.worldtblidx = pPlayer->GetCurWorld()->GetIdx();
				data.tblidx = spawnTbldat->mob_Tblidx;
				spawnTbldat->vSpawn_Loc.CopyTo(data.vCurLoc);
				spawnTbldat->vSpawn_Loc.CopyTo(data.vSpawnLoc);
				spawnTbldat->vSpawn_Dir.CopyTo(data.vCurDir);
				spawnTbldat->vSpawn_Dir.CopyTo(data.vSpawnDir);
				data.bySpawnFuncFlag = 0;
				data.sScriptData.playScript = spawnTbldat->playScript;
				data.sScriptData.playScriptScene = spawnTbldat->playScriptScene;
				data.sScriptData.tblidxAiScript = spawnTbldat->aiScript;
				data.sScriptData.tblidxAiScriptScene = spawnTbldat->aiScriptScene;
				data.qwCharConditionFlag = 0;
				data.partyID = spawnTbldat->dwParty_Index;
				data.bPartyLeader = spawnTbldat->bParty_Leader;
				data.byImmortalMode = eIMMORTAL_MODE_OFF;
				data.actionpatternTblIdx = spawnTbldat->actionPatternTblidx;
				data.bySpawnRange = spawnTbldat->bySpawn_Loc_Range;
				data.wSpawnTime = spawnTbldat->wSpawn_Cool_Time;
				data.byMoveType = spawnTbldat->bySpawn_Move_Type;
				data.byWanderRange = spawnTbldat->byWander_Range;
				data.byMoveRange = spawnTbldat->byMove_Range;
				data.pathTblidx = spawnTbldat->path_Table_Index;
				data.hTargetFixedExecuter = INVALID_HOBJECT;
				data.sBotSubData.byNestRange = 20;
				data.sBotSubData.byNestType = NPC_NEST_TYPE_DEFAULT;

				sMOB_TBLDAT* pTbldat = (sMOB_TBLDAT*)g_pTableContainer->GetMobTable()->FindData(data.tblidx);
				if (pTbldat)
				{
					CMonster* pMob = (CMonster*)g_pObjectManager->CreateCharacter(OBJTYPE_MOB);
					if (pMob)
					{
						if (pMob->CreateDataAndSpawn(data, pTbldat))
						{
							pMob->SetStandAlone(false);
						}
						else
						{
							break;
						}
					}
					else
					{
						break;
					}
				}
				else
				{
					ERR_LOG(LOG_SCRIPT, "Could not find MOB-TBLDAT. Tblidx %u Grouptblidx %u", data.tblidx, groupId);
					break;
				}
			}
		}

		spawnTbldat = g_pTableContainer->GetMobSpawnTable(pPlayer->GetCurWorld()->GetIdx())->GetSpawnGroupNext(groupId);
	}
}

ACMD(do_addnpc)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	TBLIDX npcid;
	if (!comutils::parse_uint(strToken, npcid)) {
		do_feedback(pPlayer, L"NPC entry ids must be whole numbers '%S' is not valid.", strToken.c_str());
		return;
	}

	sNPC_TBLDAT* pTblData = (sNPC_TBLDAT*)g_pTableContainer->GetNpcTable()->FindData(npcid);
	if (!pTblData)
	{

		do_feedback(pPlayer, L"NPC entry ids must be whole numbers '%S' is not valid.", strToken.c_str());
		return;
	}

	sVECTOR3 spawnloc;
	spawnloc.x = pPlayer->GetCurLoc().x + rand() % 5;
	spawnloc.y = pPlayer->GetCurLoc().y;
	spawnloc.z = pPlayer->GetCurLoc().z + rand() % 5;

	sVECTOR3 spawndir;
	spawndir.x = pPlayer->GetCurDir().x + rand() % 5;
	spawndir.y = pPlayer->GetCurDir().y;
	spawndir.z = pPlayer->GetCurDir().z + rand() % 5;

	sSPAWN_TBLDAT sSpawn;
	sSpawn.vSpawn_Dir.CopyFrom(spawndir);
	sSpawn.vSpawn_Loc.CopyFrom(spawnloc);
	sSpawn.dwParty_Index = INVALID_DWORD;
	sSpawn.byMove_Range = INVALID_BYTE;
	sSpawn.bySpawn_Move_Type = SPAWN_MOVE_UNKNOWN;
	sSpawn.bySpawn_Loc_Range = 10;
	sSpawn.byWander_Range = INVALID_BYTE;
	sSpawn.path_Table_Index = INVALID_TBLIDX;
	sSpawn.playScript = INVALID_TBLIDX;
	sSpawn.playScriptScene = INVALID_TBLIDX;
	sSpawn.aiScript = INVALID_TBLIDX;
	sSpawn.aiScriptScene = INVALID_TBLIDX;
	sSpawn.actionPatternTblidx = 1;

	CNpc* pNpc = (CNpc*)g_pObjectManager->CreateCharacter(OBJTYPE_NPC);
	pNpc->CreateDataAndSpawn(pPlayer->GetWorldID(), pTblData, &sSpawn, false, 0);
	pNpc->SetStandAlone(false);
	do_feedback(pPlayer, L"Spawned a new NPC with entry id: '%u'", npcid);
}

ACMD(do_additem)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	TBLIDX ItemId;
	if (!comutils::parse_uint(strToken, ItemId)) {
		do_feedback(pPlayer, L"Item ids must be whole numbers '%S' is not valid.", strToken.c_str());
		return;
	}

	pToken->PopToPeek();
	strToken = pToken->PeekNextToken(NULL, &iLine);
	BYTE amount = 1;
	if (!comutils::parse_byte(strToken, amount) && strToken.size() > 0) {
		do_feedback(pPlayer, L"Amounts must be whole numbers (up to 255), '%S' is not valid.", strToken.c_str());
		return;
	}

	CPlayer* pTarget = pPlayer;

	if (pTarget->GetPlayerItemContainer()->CountEmptyInventory() < 1)
	{
		do_feedback(pPlayer, L"You do not have the inventory space to add this item to your inventory.");
		return;
	}

	sITEM_TBLDAT* pTblData = (sITEM_TBLDAT*)g_pTableContainer->GetItemTable()->FindData(ItemId);
	if (!pTblData)
	{
		do_feedback(pPlayer, L"Could not find an item with id '%u'.", ItemId);
		return;
	}

	if (!pTblData->bValidity_Able)
	{
		do_feedback(pPlayer, L"This item can not be created.");
		return;
	}

	if (amount > pTblData->byMax_Stack)
		amount = pTblData->byMax_Stack;

	g_pItemManager->CreateItem(pTarget, ItemId, amount, INVALID_BYTE, INVALID_BYTE, pTblData->Item_Option_Tblidx == INVALID_TBLIDX);
}

ACMD(do_addmasteritem)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	BYTE bySecondClass = (BYTE)atof(strToken.c_str());

	TBLIDX itemTblidx = INVALID_TBLIDX;

	if (pPlayer->GetClass() > PC_CLASS_1_LAST)
	{
		do_feedback(pPlayer, L"You already have a mastery class.");
		return;
	}

	switch (pPlayer->GetClass())
	{
		case PC_CLASS_HUMAN_FIGHTER:
		{
			switch (bySecondClass)
			{
				case PC_CLASS_STREET_FIGHTER: itemTblidx = 99078; break;
				case PC_CLASS_SWORD_MASTER: itemTblidx = 99079; break;

				default: do_feedback(pPlayer, L"As a martial artist the $classId argument must be either '%u' (Fighter) or '%u' (Swordsman).", PC_CLASS_STREET_FIGHTER, PC_CLASS_SWORD_MASTER); return;
			}
		}
		break;
		case PC_CLASS_HUMAN_MYSTIC:
		{
			switch (bySecondClass)
			{
				case PC_CLASS_CRANE_ROSHI: itemTblidx = 99081; break;
				case PC_CLASS_TURTLE_ROSHI: itemTblidx = 99080; break;

				default: do_feedback(pPlayer, L"As a spiritualist the $classId argument must be either '%u' (Crane Hermit) or '%u' (Turtle Hermit).", PC_CLASS_CRANE_ROSHI, PC_CLASS_TURTLE_ROSHI); return;
			}
		}
		break;
		case PC_CLASS_NAMEK_FIGHTER:
		{
			switch (bySecondClass)
			{
				case PC_CLASS_DARK_WARRIOR: itemTblidx = 99082; break;
				case PC_CLASS_SHADOW_KNIGHT: itemTblidx = 99083; break;

				default: do_feedback(pPlayer, L"As a warrior the $classId argument must be either '%u' (Dark Warrior) or '%u' (Shadow Knight).", PC_CLASS_DARK_WARRIOR, PC_CLASS_SHADOW_KNIGHT); return;
			}
		}
		break;
		case PC_CLASS_NAMEK_MYSTIC:
		{
			switch (bySecondClass)
			{
				case PC_CLASS_DENDEN_HEALER: itemTblidx = 99084; break;
				case PC_CLASS_POCO_SUMMONER: itemTblidx = 99085; break;

				default: do_feedback(pPlayer, L"As a dragon clan the $classId argument must be either '%u' (Dende Priest) or '%u' (Poko Priest).", PC_CLASS_DENDEN_HEALER, PC_CLASS_POCO_SUMMONER); return;
			}
		}
		break;
		case PC_CLASS_MIGHTY_MAJIN:
		{
			switch (bySecondClass)
			{
				case PC_CLASS_ULTI_MA: itemTblidx = 99086; break;
				case PC_CLASS_GRAND_MA: itemTblidx = 99087; break;

				default: do_feedback(pPlayer, L"As a mighty majin the $classId argument must be either '%u' (Ultimate Majin) or '%u' (Grand Chef Majin).", PC_CLASS_ULTI_MA, PC_CLASS_GRAND_MA); return;
			}
		}
		break;
		case PC_CLASS_WONDER_MAJIN:
		{
			switch (bySecondClass)
			{
				case PC_CLASS_PLAS_MA: itemTblidx = 99088; break;
				case PC_CLASS_KAR_MA: itemTblidx = 99089; break;

				default: do_feedback(pPlayer, L"As a wonder majin the $classId argument must be either '%u' (Plasma Majin) or '%u' (Karma Majin).", PC_CLASS_PLAS_MA, PC_CLASS_KAR_MA); return;
			}
		}
		break;

		default: return; break;
	}

	sITEM_TBLDAT* pTblData = (sITEM_TBLDAT*)g_pTableContainer->GetItemTable()->FindData(itemTblidx);
	if (pTblData)
	{
		if (pPlayer->GetPlayerItemContainer()->GetItemByIdx(itemTblidx) == NULL)
		{
			std::pair<BYTE, BYTE> inv = pPlayer->GetPlayerItemContainer()->GetEmptyInventory();
			if (inv.first != INVALID_BYTE && inv.second != INVALID_BYTE)
				g_pItemManager->CreateItem(pPlayer, itemTblidx, 1, inv.first, inv.second);
		}
		else 
		{
			do_feedback(pPlayer, L"You already have this mastery item in your inventory.");
		}
	}
}

ACMD(do_addskill)
{
	CGameServer* app = (CGameServer*)g_pApp;

	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	TBLIDX SkillId;
	if (!comutils::parse_uint(strToken, SkillId)) {
		do_feedback(pPlayer, L"Skill ids must be whole numbers '%S' is not valid.", strToken.c_str());
		return;
	}

	WORD result;
	CSkillManagerPc* skillMgr = pPlayer->GetSkillManager();

	if (!skillMgr) {
		do_feedback(pPlayer, L"You can not learn skills.");
		return;
	}

	skillMgr->LearnSkill(SkillId, result, false);
	if (result == GAME_SUCCESS) {
		do_feedback(pPlayer, L"You have now learned skill '%u'!", SkillId);
		return;
	}
	if (result == GAME_SKILL_NOT_ENOUGH_SP_POINT) {
		do_feedback(pPlayer, L"You do not have enough SP points.");
		return;
	}
	if (result == GAME_SKILL_NO_PREREQUISITE_SKILLS_YOU_HAVE)
	{
		do_feedback(pPlayer, L"You do not have the prequisite skills for this skill.");
		return;
	}
	if (result == GAME_SKILL_TRIED_TO_LEARN_WRONG_CLASS_SKILL)
	{
		do_feedback(pPlayer, L"You are not the right class for this skill.");
		return;
	}
	if (result == GAME_SKILL_TOO_HIGH_LEVEL_TO_TRAIN)
	{
		do_feedback(pPlayer, L"You are not the required level for this skill.");
		return;
	}
	if (result == GAME_SKILL_ALREADY_MASTERED_SKILL)
	{
		do_feedback(pPlayer, L"You have already mastered this skill.");
		return;
	}
	if (result == GAME_SKILL_CANT_BE_GAINED_BY_YOURSELF)
	{
		do_feedback(pPlayer, L"You can not learn this skill by yourself.");
		return;
	}
	if (result == GAME_SKILL_NOT_EXISTING_SKILL)
	{
		do_feedback(pPlayer, L"No skill with id '%u' exists.", SkillId);
		return;
	}
	if (result == GAME_SKILL_NO_EMPTY_SKILL_SLOT) {
		do_feedback(pPlayer, L"You do not have any empty skill slot.");
		return;
	}
}

ACMD(do_addskill2)
{
	CGameServer* app = (CGameServer*)NtlSfxGetApp();

	TBLIDX SkillId = INVALID_TBLIDX;

	switch (pPlayer->GetClass())
	{
		case PC_CLASS_STREET_FIGHTER: SkillId = 729991; break;
		case PC_CLASS_SWORD_MASTER: SkillId = 829991; break;
		case PC_CLASS_CRANE_ROSHI: SkillId = 929991; break;
		case PC_CLASS_TURTLE_ROSHI: SkillId = 1029991; break;
		case PC_CLASS_DARK_WARRIOR: SkillId = 1329991; break;
		case PC_CLASS_SHADOW_KNIGHT: SkillId = 1429991; break;
		case PC_CLASS_DENDEN_HEALER: SkillId = 1529991; break;
		case PC_CLASS_POCO_SUMMONER: SkillId = 1629991; break;
		case PC_CLASS_ULTI_MA: SkillId = 1729991; break;
		case PC_CLASS_GRAND_MA: SkillId = 1829991; break;
		case PC_CLASS_PLAS_MA: SkillId = 1929991; break;
		case PC_CLASS_KAR_MA: SkillId = 2029991; break;

		default: do_feedback(pPlayer, L"You do not have a mastery class."); return;
	}

	WORD wTemp;


	if (pPlayer->GetSkillManager())
		pPlayer->GetSkillManager()->LearnSkill(SkillId, wTemp, false);
}

ACMD(do_r)
{
	pPlayer->UpdateCurLpEp(pPlayer->GetMaxLP(), pPlayer->GetMaxEP(), true, false);
}

ACMD(do_addhtb)
{
	CGameServer* app = (CGameServer*)NtlSfxGetApp();

	WORD wTemp;
	TBLIDX id;

	switch (pPlayer->GetClass())
	{
		case PC_CLASS_STREET_FIGHTER:
		case PC_CLASS_SWORD_MASTER:
			id = 30611;
			break;

		case PC_CLASS_CRANE_ROSHI:
		case PC_CLASS_TURTLE_ROSHI:
			id = 130411;
			break;

		case PC_CLASS_DARK_WARRIOR:
		case PC_CLASS_SHADOW_KNIGHT:
			id = 330611;
			break;

		case PC_CLASS_DENDEN_HEALER:
		case PC_CLASS_POCO_SUMMONER:
			id = 430411;
			break;

		case PC_CLASS_ULTI_MA:
		case PC_CLASS_GRAND_MA:
			id = 532011;
			break;

		case PC_CLASS_PLAS_MA:
		case PC_CLASS_KAR_MA:
			id = 632011;
			break;
		default:
			do_feedback(pPlayer, L"You must have a mastery class for this command.");
			return;
	}

	pPlayer->GetHtbSkillManager()->LearnHtbSkill(id, wTemp);

	switch (wTemp) {
		case GAME_SKILL_TRIED_TO_LEARN_WRONG_CLASS_SKILL: do_feedback(pPlayer, L"This skill does not belong to your class."); break;
		case GAME_SKILL_TOO_HIGH_LEVEL_TO_TRAIN: do_feedback(pPlayer, L"This skill requires your character to be of a higher level."); break;
		case GAME_SKILL_ALREADY_MASTERED_SKILL: do_feedback(pPlayer, L"You already have this skill."); break;
		case GAME_SKILL_NOT_EXISTING_SKILL: do_feedback(pPlayer, L"HTB skill with id: '%u' does not exist.", id); break;
		case GAME_SKILL_NO_EMPTY_SKILL_SLOT: do_feedback(pPlayer, L"You are at the limit of HTB skills."); break;
	}
}

ACMD(do_setzenny)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	DWORD zeni;
	if (!comutils::parse_ulong(strToken, zeni)) {
		do_feedback(pPlayer, L"Zenny amounts be whole numbers. '%S' is not valid.", strToken.c_str());
		return;
	}


	ERR_LOG(LOG_USER, "Player: %u receive %u zeni from gm command", pPlayer->GetCharID(), zeni);

	pPlayer->UpdateZeni(ZENNY_CHANGE_TYPE_CHEAT, zeni, true);
}

ACMD(do_setlevel)
{
	CGameServer* app = (CGameServer*)NtlSfxGetApp();

	CPlayer* pTarget = pPlayer;

	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	BYTE level;
	if (!comutils::parse_byte(strToken, level)) {
		do_feedback(pPlayer, L"Levels be whole numbers. '%S' is not valid.", strToken.c_str());
		return;
	}

	RwUInt32 curlv = pTarget->GetLevel();
	sEXP_TBLDAT* ExpData = (sEXP_TBLDAT*)g_pTableContainer->GetExpTable()->FindData(level);
	if (!ExpData)
	{
		do_feedback(pPlayer, L"There is no experience data for level '%u', this makes it invalid.", level);
		return;
	}

	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_LEVEL));
	sGU_UPDATE_CHAR_LEVEL* res = (sGU_UPDATE_CHAR_LEVEL*)packet.GetPacketData();
	res->wOpCode = GU_UPDATE_CHAR_LEVEL;
	res->byCurLevel = level;
	res->byPrevLevel = curlv;
	res->dwMaxExpInThisLevel = ExpData->dwNeed_Exp;
	res->handle = pTarget->GetID();
	packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_LEVEL));
	pTarget->Broadcast(&packet);

	DWORD getsp = Dbo_GetLevelUpGainSP(level, level - curlv);
	pTarget->UpdateCharSP(pTarget->GetSkillPoints() + getsp);
	pTarget->SetLevel(level);
	pTarget->GetCharAtt()->CalculateAll();

	pTarget->UpdateCurLpEp(pTarget->GetMaxLP(), pTarget->GetMaxEP(), true, false);

	pTarget->UpdateMaxRpBalls();

	//send to chat server
	app->GetChatServerSession()->SendUpdatePcLevel(pTarget);

	//update party
	if (pTarget->GetPartyID() != INVALID_PARTYID && pTarget->GetParty())
		pTarget->GetParty()->UpdateMemberLevel(pTarget);

	//send to query server
	CNtlPacket packetQry(sizeof(sGQ_PC_UPDATE_LEVEL_REQ));
	sGQ_PC_UPDATE_LEVEL_REQ* resQry = (sGQ_PC_UPDATE_LEVEL_REQ*)packetQry.GetPacketData();
	resQry->wOpCode = GQ_PC_UPDATE_LEVEL_REQ;
	resQry->handle = pTarget->GetID();
	resQry->charId = pTarget->GetCharID();
	resQry->dwEXP = 0;
	resQry->byLevel = pTarget->GetLevel();
	resQry->dwSP = pTarget->GetSkillPoints();
	packetQry.SetPacketLen(sizeof(sGQ_PC_UPDATE_LEVEL_REQ));
	app->SendTo(app->GetQueryServerSession(), &packetQry);
}

ACMD(do_hide)
{
	if (pPlayer->GetStateManager()->IsCharCondition(CHARCOND_TRANSPARENT))
		pPlayer->GetStateManager()->RemoveConditionState(CHARCOND_TRANSPARENT, NULL, true);
	else
		pPlayer->GetStateManager()->AddConditionState(CHARCOND_TRANSPARENT, NULL, true);
}

ACMD(do_notice)
{
	/*
		@notice TYPE(0-6) TEXT(MAX 256 CHARACTERS)
	*/

	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	BYTE byDisType;
	if (!comutils::parse_byte(strToken, byDisType)) {
		do_feedback(pPlayer, L"'%S' is not a valid $noticetype. Valid noticetypes go from 0-6.", strToken.c_str());
		return;
	}

	if (byDisType >= SERVER_TEXT_UNKNOWN)
	{
		do_feedback(pPlayer, L"Unknown noticetype '%u'. Valid noticetypes go from 0-6.", byDisType);
		return;
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
		do_feedback(pPlayer, L"You must supply a message to sent with the notice.", byDisType);
		return;
	}


	CGameServer* app = (CGameServer*)g_pApp;

	CNtlPacket packet(sizeof(sGT_SYSTEM_DISPLAY_TEXT));
	sGT_SYSTEM_DISPLAY_TEXT* res = (sGT_SYSTEM_DISPLAY_TEXT*)packet.GetPacketData();
	res->wOpCode = GT_SYSTEM_DISPLAY_TEXT;
	res->serverChannelId = INVALID_SERVERCHANNELID;
	res->byDisplayType = byDisType;
	wcscpy_s(res->wszMessage, NTL_MAX_LENGTH_OF_CHAT_MESSAGE + 1, s2ws(text).c_str());
	packet.SetPacketLen(sizeof(sGT_SYSTEM_DISPLAY_TEXT));
	app->SendTo(app->GetChatServerSession(), &packet);
}

ACMD(do_pm)
{
	pToken->PopToPeek();
	std::string strName = pToken->PeekNextToken(NULL, &iLine);

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

	if (CPlayer* pTarget = g_pObjectManager->FindByName(s2ws(strName).c_str()))
	{
		CNtlPacket packet(sizeof(sGU_SYSTEM_DISPLAY_TEXT));
		sGU_SYSTEM_DISPLAY_TEXT* res = (sGU_SYSTEM_DISPLAY_TEXT*)packet.GetPacketData();
		res->wOpCode = GU_SYSTEM_DISPLAY_TEXT;
		res->wMessageLengthInUnicode = (WORD)text.length();
		res->byDisplayType = SERVER_TEXT_EMERGENCY;
		wcscpy_s(res->awchMessage, NTL_MAX_LENGTH_OF_CHAT_MESSAGE + 1, s2ws(text).c_str());
		packet.SetPacketLen(sizeof(sGU_SYSTEM_DISPLAY_TEXT));
		pTarget->SendPacket(&packet);
	}
}

ACMD(do_teleport)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	BYTE byMap;

	if (!comutils::parse_byte(strToken, byMap)) {
		do_feedback(pPlayer, L"'%S' is not a valid map id. Valid map ids go from 0-18.", strToken.c_str());
		return;
	}
	WORLDID worldid = 1;

	CNtlVector destLoc;
	switch (byMap)
	{
		case MPP_TELE_YAHOI: {
			destLoc.x = 4493; destLoc.z = 4032;
		}break;
		case MPP_TELE_YUREKA: {
			destLoc.x = 6586; destLoc.z = 3254;
		}break;
		case MPP_TELE_DALPANG: {
			destLoc.x = 3246; destLoc.z = -2749;
		}break;
		case MPP_TELE_DRAGON: {
			destLoc.x = 4620; destLoc.z = -1729;
		}break;
		case MPP_TELE_BAEE: {
			destLoc.x = 4509; destLoc.z = 4031;
		}break;
		case MPP_TELE_AJIRANG: {
			destLoc.x = 4509; destLoc.z = 4031;
		}break;
		case MPP_TELE_KARINGA_1: {
			destLoc.x = 5928; destLoc.z = 658;
		}break;
		case MPP_TELE_KARINGA_2: {
			destLoc.x = 5884; destLoc.z = 827;
		}break;
		case MPP_TELE_GREAT_TREE: {
			destLoc.x = 5903; destLoc.z = 1711;
		}break;
		case MPP_TELE_KARINGA_3: {
			destLoc.x = 7527; destLoc.z = -518;
		}break;
		case MPP_TELE_MERMAID: {
			destLoc.x = 4651; destLoc.z = -206;
		}break;
		case MPP_TELE_GANNET: {
			destLoc.x = 3690; destLoc.z = 1396;
		}break;
		case MPP_TELE_EMERALD: {
			destLoc.x = 3390; destLoc.z = -564;
		}break;
		case MPP_TELE_TEMBARIN: {
			destLoc.x = 1964; destLoc.z = 1072;
		}break;
		case MPP_TELE_CELL: {
			destLoc.x = -480; destLoc.z = 1646;
		}break;
		case MPP_TELE_BUU: {
			destLoc.x = 2481; destLoc.z = 3384;
		}break;
		case MPP_TELE_CC: {
			destLoc.x = -1796; destLoc.z = 1106;
		}break;
		case MPP_TELE_MUSHROOM: {
			destLoc.x = -1408; destLoc.z = -1408;
		}break;

		case MPP_TELE_PAPAYA: {
			destLoc.x = -5336; destLoc.y = -66; destLoc.z = -6658;
			worldid = 15;
		}break;
	default:
		do_feedback(pPlayer, L"'%S' is not a valid map id. Valid map ids go from 0-18.", strToken.c_str());
		return;
	}

	pPlayer->StartTeleport(destLoc, pPlayer->GetCurDir(), worldid, TELEPORT_TYPE_COMMAND);
}

ACMD(do_world)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	WORLDID worldId;

	if (!comutils::parse_uint(strToken, worldId))
	{
		do_feedback(pPlayer, L"'%S' is not a valid world id. Only numbers are valid.", strToken.c_str());
		return;
	}

	CGameServer* app = (CGameServer*)g_pApp;

	sWORLD_TBLDAT* pWorldTbldat = (sWORLD_TBLDAT*)g_pTableContainer->GetWorldTable()->FindData(worldId);
	if (pWorldTbldat == NULL)
	{
		do_feedback(pPlayer, L"Could not find a world entry for world id '%u'.", worldId);
		return;
	}

	if (CWorld* pWorld = app->GetGameMain()->GetWorldManager()->FindWorld(worldId))
	{
		pPlayer->StartTeleport(pWorld->GetTbldat()->vDefaultLoc, pPlayer->GetCurDir(), worldId, TELEPORT_TYPE_COMMAND);
	}
	else
	{
		CWorld* pWorld2 = app->GetGameMain()->GetWorldManager()->CreateWorld(pWorldTbldat);
		if (pWorld2 == NULL)
			return;

		pPlayer->StartTeleport(pWorld2->GetTbldat()->vStart1Loc, pPlayer->GetCurDir(), pWorld2->GetID(), TELEPORT_TYPE_COMMAND);
	}
}

ACMD(do_warp)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);

	std::wstring name = std::wstring(strToken.begin(), strToken.end());
	const wchar_t* wname = name.c_str();

	CCharacter* target = g_pObjectManager->FindByName(wname);
	if (!target) {
		do_feedback(pPlayer, L"Could not find a player with name '%S'", strToken.c_str());
		return;
	}

	if (!target->GetCurWorld()) {
		do_feedback(pPlayer, L"Player '%S' is not currently in a world.", strToken.c_str());
		return;
	}

	if (target->GetCurWorld()->GetTbldat()->bDynamic) {
		do_feedback(pPlayer, L"Player '%S' is currently in a dungeon.", strToken.c_str());
		return;
	}

	pPlayer->StartTeleport(target->GetCurLoc(), target->GetCurDir(), target->GetWorldID(), TELEPORT_TYPE_COMMAND);
}

ACMD(do_call)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);

	std::wstring name = std::wstring(strToken.begin(), strToken.end());
	const wchar_t* wname = name.c_str();

	CCharacter* target = g_pObjectManager->FindByName(wname);
	if (!target) {
		do_feedback(pPlayer, L"Could not find a player with name '%S'", strToken.c_str());
		return;
	}

	if (!pPlayer->GetCurWorld()) {
		do_feedback(pPlayer, L"You are not currently in a world.");
		return;
	}
	if (target->GetCurWorld()->GetTbldat()->bDynamic) {
		do_feedback(pPlayer, L"You are currently in a dungeon.");
		return;
	}

	target->StartTeleport(pPlayer->GetCurLoc(), pPlayer->GetCurDir(), pPlayer->GetWorldID(), TELEPORT_TYPE_COMMAND);
}

ACMD(do_shutdown)
{
	CGameServer* app = (CGameServer*)g_pApp;

	app->GetGameProcessor()->StartServerShutdownEvent();
	do_feedback(pPlayer, L"Server shutdown initiated.");
}

ACMD(do_setadult)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	int n = (int)atof(strToken.c_str());
	bool bAdultSet = n != 0;

	pPlayer->UpdateAdult(bAdultSet);
}

ACMD(do_setclass)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	BYTE byClass;
	if (!comutils::parse_byte(strToken, byClass)) {
		do_feedback(pPlayer, L"'%S' is not a valid class id. Class ids can go from 0-255.", strToken.c_str());
		return;
	}

	sPC_TBLDAT* pcdata = (sPC_TBLDAT*)g_pTableContainer->GetPcTable()->GetPcTbldat(pPlayer->GetTbldat()->byRace, byClass, pPlayer->GetTbldat()->byGender);
	if (!pcdata) {
		do_feedback(pPlayer, L"Class '%u' is not valid for your race/gender or does not exist.", byClass);
	}

	pPlayer->UpdateClass(byClass);
}

ACMD(do_dc)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	std::wstring name = std::wstring(strToken.begin(), strToken.end());
	const wchar_t* wname = name.c_str();

	CPlayer* target = g_pObjectManager->FindByName(wname);
	if (!target) {
		do_feedback(pPlayer, L"Could not find a player with name '%S'.", strToken.c_str());
		return;
	}

	if (!target->IsInitialized()) {
		do_feedback(pPlayer, L"Player '%S' is still loading in.", strToken.c_str());
		return;
	}

	target->GetClientSession()->Disconnect(false);
}

ACMD(do_kill)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	std::wstring name = std::wstring(strToken.begin(), strToken.end());
	const wchar_t* wname = name.c_str();

	CPlayer* target = g_pObjectManager->FindByName(wname);
	if (!target) {
		do_feedback(pPlayer, L"Could not find a player with name '%S'", strToken.c_str());
		return;
	}

	if (!target->IsInitialized()) {
		do_feedback(pPlayer, L"Player '%S' is still loading in.", strToken.c_str());
		return;
	}

	if (target && target->IsInitialized())
		target->Faint(pPlayer, FAINT_REASON_COMMAND);
}

ACMD(do_delallitems)
{
	pPlayer->GetPlayerItemContainer()->DeleteAllItems();
}

ACMD(do_god)
{
	pPlayer->GetCharAtt()->SetPhysicalOffence(INVALID_WORD);
	pPlayer->GetCharAtt()->SetPhysicalDefence(INVALID_WORD);
	pPlayer->GetCharAtt()->SetEnergyOffence(INVALID_WORD);

	pPlayer->UpdateAttackSpeed(100);

	if (pPlayer->GetImmortalMode() == eIMMORTAL_MODE_OFF)
		pPlayer->SetImmortalMode(eIMMORTAL_MODE_NORMAL);
	else
		pPlayer->SetImmortalMode(eIMMORTAL_MODE_OFF);

	pPlayer->UpdateMoveSpeed(25.f, 25.f);
}

ACMD(do_invincible)
{
	/*
		@invincible DURATION(SECONDS)
	*/

	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	int nSeconds = (int)atof(strToken.c_str());

	if (nSeconds == 0 || nSeconds > 3600)
		nSeconds = 3600;

	sDBO_BUFF_PARAMETER aBuffParameter[NTL_MAX_EFFECT_IN_SKILL];
	sSKILL_TBLDAT* pTempSkillTbldat = (sSKILL_TBLDAT*)g_pTableContainer->GetSkillTable()->FindData(2385);

	eSYSTEM_EFFECT_CODE aeEffectCode[NTL_MAX_EFFECT_IN_SKILL];
	aeEffectCode[0] = g_pTableContainer->GetSystemEffectTable()->GetEffectCodeWithTblidx(pTempSkillTbldat->skill_Effect[0]);
	aeEffectCode[1] = INVALID_SYSTEM_EFFECT_CODE;

	aBuffParameter[0].byBuffParameterType = DBO_BUFF_PARAMETER_TYPE_DEFAULT;
	aBuffParameter[0].buffParameter.fParameter = 0;
	aBuffParameter[0].buffParameter.dwRemainValue = 0;

	DWORD dwDurationInMs = nSeconds * 1000;

	pPlayer->GetBuffManager()->RegisterBuff(dwDurationInMs, aeEffectCode, aBuffParameter, INVALID_HOBJECT, BUFF_TYPE_BLESS, pTempSkillTbldat);
}

ACMD(do_bann)
{
	CGameServer* app = (CGameServer*)g_pApp;

	/*
		@bann USERNAME DURATION(DAYS. 255 = PERMA) REASON(MAX 256 CHARACTERS)
	*/
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	std::wstring name = std::wstring(strToken.begin(), strToken.end());

	CPlayer* target = g_pObjectManager->FindByName(name.c_str());
	if (!target) {
		do_feedback(pPlayer, L"Could not find a player with name '%S'.", strToken.c_str());
		return;
	}

	if (!target->IsInitialized()) {
		do_feedback(pPlayer, L"Player '%S' is still loading in.", strToken.c_str());
		return;
	}

	pToken->PopToPeek();
	strToken = pToken->PeekNextToken(NULL, &iLine);
	BYTE byDuration;
	if (!comutils::parse_byte(strToken, byDuration)) {
		do_feedback(pPlayer, L"'%S' is not a valid bann duration. Duration can go from 0-255.", strToken.c_str());
		return;
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
			text += mtext;
			text += " ";
		}
	}

	target->Bann(text, byDuration, pPlayer->GetAccountID());
}

ACMD(do_dbann)
{
	CGameServer* app = (CGameServer*)g_pApp;

	/*
		@dbann ACCOUNT_ID DURATION(DAYS. 255 = PERMA) REASON(MAX 256 CHARACTERS)
	*/
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	ACCOUNTID accid;
	if (!comutils::parse_uint(strToken, accid)) {
		do_feedback(pPlayer, L"'%S' is not a valid account id. Account ids must be numbers.", strToken.c_str());
		return;
	}
	pToken->PopToPeek();
	strToken = pToken->PeekNextToken(NULL, &iLine);
	BYTE byDuration;
	if (!comutils::parse_byte(strToken, byDuration)) {
		do_feedback(pPlayer, L"'%S' is not a valid bann duration. Duration can go from 0-255.", strToken.c_str());
		return;
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
			text += mtext;
			text += " ";
		}
	}

	CNtlPacket pQry(sizeof(sGQ_ACCOUNT_BANN));
	sGQ_ACCOUNT_BANN* qRes = (sGQ_ACCOUNT_BANN*)pQry.GetPacketData();
	qRes->wOpCode = GQ_ACCOUNT_BANN;
	qRes->gmAccountID = pPlayer->GetAccountID();
	qRes->targetAccountID = accid;
	strcpy_s(qRes->szReason, NTL_MAX_LENGTH_OF_CHAT_MESSAGE + 1, text.c_str());
	qRes->byDuration = byDuration;
	pQry.SetPacketLen(sizeof(sGQ_ACCOUNT_BANN));
	app->SendTo(app->GetQueryServerSession(), &pQry);
}

ACMD(do_purge) //despawn all monster around player
{
	CWorldCell* pWorldCell = pPlayer->GetCurWorldCell();
	if (!pWorldCell)
		return;

	CMonster* pNextMob = NULL;
	CNpc* pNextNpc = NULL;

	CWorldCell::QUADPAGE page = pWorldCell->GetCellQuadPage(pPlayer->GetCurLoc());
	for (int dir = CWorldCell::QUADPAGE_FIRST; dir < CWorldCell::QUADPAGE_COUNT; dir++)
	{
		CWorldCell* pWorldCellSibling = pWorldCell->GetQuadSibling(page, (CWorldCell::QUADDIR)dir);
		if (pWorldCellSibling)
		{
			CMonster* pMobTarget = (CMonster*)pWorldCellSibling->GetObjectList()->GetFirst(OBJTYPE_MOB);
			while (pMobTarget)
			{
				pNextMob = (CMonster*)pWorldCellSibling->GetObjectList()->GetNext(pMobTarget->GetWorldCellObjectLinker());

				if (pMobTarget->GetCurWorld() && !pMobTarget->IsFainting())
					pMobTarget->Faint(pPlayer);

				pMobTarget = pNextMob;
			}

			CNpc* pNpcTarget = (CMonster*)pWorldCellSibling->GetObjectList()->GetFirst(OBJTYPE_NPC);
			while (pNpcTarget)
			{
				pNextNpc = (CMonster*)pWorldCellSibling->GetObjectList()->GetNext(pNpcTarget->GetWorldCellObjectLinker());

				if (pNpcTarget->GetCurWorld() && !pNpcTarget->IsFainting() && !pNpcTarget->GetStandAlone())
					pNpcTarget->Faint(pPlayer);

				pNpcTarget = pNextNpc;
			}
		}
	}
}

ACMD(do_unstuck)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);

	if (!pPlayer->GetClientSession())
		return;

	if (pPlayer->IsFainting())
	{
		do_feedback(pPlayer, L"You can't use @unstuck when you are fainting.");
		return;
	}
	if (pPlayer->GetFreeBattleID() != INVALID_DWORD)
	{
		do_feedback(pPlayer, L"You can't use @unstuck while in a free battle.");
		return;
	}
	if (pPlayer->IsPvpZone() || pPlayer->GetFightMode())
	{
		do_feedback(pPlayer, L"You can't use @unstuck in a PvP zone or while in a fight.");
		return;
	}
	if (pPlayer->GetCurWorld() && pPlayer->GetCurWorld()->GetTbldat()->bDynamic)
	{
		do_feedback(pPlayer, L"You can't use @unstuck in a dynamic (?) world.");
		return;
	}
	if (pPlayer->GetDragonballScrambleBallFlag() > 0)
	{
		do_feedback(pPlayer, L"You can't use @unstuck while in a dragonball scramble.");
		return;
	}

	CGameServer* app = (CGameServer*)g_pApp;

	if (app->GetGsServerId() == DOJO_CHANNEL_INDEX)
	{
		do_feedback(pPlayer, L"You can't use @unstuck on the tournament channel.");
		return;
	}

	CNtlVector vBindLoc(pPlayer->GetBindLoc());
	pPlayer->StartTeleport(vBindLoc, pPlayer->GetCurDir(), pPlayer->GetBindWorldID(), TELEPORT_TYPE_COMMAND);
}

ACMD(do_warfog)
{
	CGameServer* app = (CGameServer*)g_pApp;

	CTriggerObject* pNextObj = NULL;
	CTriggerObject* pObj = (CTriggerObject*)pPlayer->GetCurWorld()->GetObjectList()->GetFirst(OBJTYPE_TOBJECT);
	while (pObj)
	{
		pNextObj = (CTriggerObject*)pPlayer->GetCurWorld()->GetObjectList()->GetNext(pObj->GetWorldObjectLinker());

		if (pObj->GetFunc() == eDBO_TRIGGER_OBJECT_FUNC_SELECTION + eDBO_TRIGGER_OBJECT_FUNC_NAMEKAN_SIGN)
		{
			if (pPlayer->CheckWarFog(pObj->GetContent()) == false)
			{
				if (pPlayer->AddWarFogFlag(pObj->GetContent()))
				{
					CNtlPacket packetQry(sizeof(sGQ_WAR_FOG_UPDATE_REQ));
					sGQ_WAR_FOG_UPDATE_REQ* resQry = (sGQ_WAR_FOG_UPDATE_REQ*)packetQry.GetPacketData();
					resQry->wOpCode = GQ_WAR_FOG_UPDATE_REQ;
					resQry->charID = pPlayer->GetCharID();
					resQry->contentsTblidx = pObj->GetContent();
					memcpy(resQry->sInfo.achWarFogFlag, pPlayer->GetWarFogFlag(), sizeof(resQry->sInfo.achWarFogFlag));
					packetQry.SetPacketLen(sizeof(sGQ_WAR_FOG_UPDATE_REQ));
					app->SendTo(app->GetQueryServerSession(), &packetQry);
				}
			}
		}

		pObj = pNextObj;
	}
}

ACMD(do_upgrade)
{
	CGameServer* app = (CGameServer*)g_pApp;
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	int nGrade;
	if (!comutils::parse_int(strToken, nGrade)) {
		do_feedback(pPlayer, L"'%S' is not a valid container grade. Grade ids must be numbers.", strToken.c_str());
		return;
	}

	nGrade = (nGrade > NTL_ITEM_MAX_GRADE) ? NTL_ITEM_MAX_GRADE : nGrade;

	int nCount = 0;

	for (int i = 0; i < EQUIP_SLOT_TYPE_SCOUTER; i++)
	{
		CItem* pItem = pPlayer->GetPlayerItemContainer()->GetItem(CONTAINER_TYPE_EQUIP, i);
		if (pItem)
		{
			if (pItem->GetGrade() != nGrade)
			{
				pItem->SetGrade(nGrade);

				CNtlPacket packet(sizeof(sGU_ITEM_UPDATE));
				sGU_ITEM_UPDATE* res = (sGU_ITEM_UPDATE*)packet.GetPacketData();
				res->wOpCode = GU_ITEM_UPDATE;
				res->handle = pItem->GetID();
				memcpy(&res->sItemData, &pItem->GetItemData(), sizeof(sITEM_DATA));
				packet.SetPacketLen(sizeof(sGU_ITEM_UPDATE));
				pPlayer->SendPacket(&packet);

				CNtlPacket pQry(sizeof(sGQ_ITEM_UPDATE_REQ));
				sGQ_ITEM_UPDATE_REQ* rQry = (sGQ_ITEM_UPDATE_REQ*)pQry.GetPacketData();
				rQry->wOpCode = GQ_ITEM_UPDATE_REQ;
				rQry->handle = pPlayer->GetID();
				rQry->charId = pPlayer->GetCharID();
				memcpy(&rQry->sItem, &pItem->GetItemData(), sizeof(sITEM_DATA));
				pQry.SetPacketLen(sizeof(sGQ_ITEM_UPDATE_REQ));
				app->SendTo(app->GetQueryServerSession(), &pQry);

				++nCount;
			}
		}
	}

	if (nCount > 0)
		pPlayer->GetCharAtt()->CalculateAll();

	do_feedback(pPlayer, L"Your containers have been upgraded!");
	return;
}

ACMD(do_setitemrank)
{
	CGameServer* app = (CGameServer*)g_pApp;
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	int nRank;
	if (!comutils::parse_int(strToken, nRank)) {
		do_feedback(pPlayer, L"'%S' is not a valid item rank. Item ranks must be numbers up to 5.", strToken.c_str());
		return;
	}

	nRank = (nRank > ITEM_RANK_LAST) ? ITEM_RANK_LAST : nRank;

	int nCount = 0;

	for (int i = 0; i < EQUIP_SLOT_TYPE_SCOUTER; i++)
	{
		CItem* pItem = pPlayer->GetPlayerItemContainer()->GetItem(CONTAINER_TYPE_EQUIP, i);
		if (pItem)
		{
			if (pItem->GetRank() != nRank)
			{
				pItem->SetRank(nRank);

				CNtlPacket packet(sizeof(sGU_ITEM_UPDATE));
				sGU_ITEM_UPDATE* res = (sGU_ITEM_UPDATE*)packet.GetPacketData();
				res->wOpCode = GU_ITEM_UPDATE;
				res->handle = pItem->GetID();
				memcpy(&res->sItemData, &pItem->GetItemData(), sizeof(sITEM_DATA));
				packet.SetPacketLen(sizeof(sGU_ITEM_UPDATE));
				pPlayer->SendPacket(&packet);

				CNtlPacket pQry(sizeof(sGQ_ITEM_UPDATE_REQ));
				sGQ_ITEM_UPDATE_REQ* rQry = (sGQ_ITEM_UPDATE_REQ*)pQry.GetPacketData();
				rQry->wOpCode = GQ_ITEM_UPDATE_REQ;
				rQry->handle = pPlayer->GetID();
				rQry->charId = pPlayer->GetCharID();
				memcpy(&rQry->sItem, &pItem->GetItemData(), sizeof(sITEM_DATA));
				pQry.SetPacketLen(sizeof(sGQ_ITEM_UPDATE_REQ));
				app->SendTo(app->GetQueryServerSession(), &pQry);

				++nCount;
			}
		}
	}

	if (nCount > 0)
		pPlayer->GetCharAtt()->CalculateAll();

	do_feedback(pPlayer, L"Your containers have been set to rank '%u'!", nRank);
	return;
}

ACMD(do_mute)
{
	CGameServer* app = (CGameServer*)g_pApp;

	/*
		@mute CHARNAME DURATION(MINUTES) REASON(MAX 128 CHARACTERS)
	*/

	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	std::wstring name = std::wstring(strToken.begin(), strToken.end());

	CPlayer* target = g_pObjectManager->FindByName(name.c_str());
	if (!target) {
		do_feedback(pPlayer, L"Could not find a player with name '%S'.", strToken.c_str());
		return;
	}

	pToken->PopToPeek();
	strToken = pToken->PeekNextToken(NULL, &iLine);
	DWORD dwDuration;

	if (!comutils::parse_ulong(strToken, dwDuration)) {
		do_feedback(pPlayer, L"'%S' is not a valid duration. Duration must be specified in numbers.", strToken.c_str());
		return;
	}

	pToken->PopToPeek();
	std::string text = "";

	while (text.length() < NTL_MAX_LENGTH_OF_MAIL_MESSAGE - 10)
	{
		std::string mtext = pToken->PeekNextToken(NULL, &iLine);
		if (mtext == ";" || mtext == "")
		{
			break;
		}
		else
		{
			text += mtext;
			text += " ";
		}
	}

	CNtlPacket packet(sizeof(sGT_UPDATE_PUNISH));
	sGT_UPDATE_PUNISH* res = (sGT_UPDATE_PUNISH*)packet.GetPacketData();
	res->wOpCode = GT_UPDATE_PUNISH;
	res->accountId = pPlayer->GetAccountID();
	res->dwDurationInMinute = dwDuration;
	NTL_SAFE_WCSCPY(res->awchGmCharName, pPlayer->GetCharName());
	NTL_SAFE_WCSCPY(res->awchCharName, name.c_str());
	wcscpy_s(res->wchReason, NTL_MAX_LENGTH_OF_MAIL_MESSAGE + 1, s2ws(text).c_str());
	packet.SetPacketLen(sizeof(sGT_UPDATE_PUNISH));
	app->SendTo(app->GetChatServerSession(), &packet);
}

ACMD(do_unmute)
{
	CGameServer* app = (CGameServer*)g_pApp;

	/*
		@unmute CHARNAME
	*/

	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	std::wstring name = std::wstring(strToken.begin(), strToken.end());

	CPlayer* target = g_pObjectManager->FindByName(name.c_str());
	if (!target) {
		do_feedback(pPlayer, L"Could not find a player with name '%S'.", strToken.c_str());
		return;
	}

	CNtlPacket packet(sizeof(sGT_UPDATE_PUNISH));
	sGT_UPDATE_PUNISH* res = (sGT_UPDATE_PUNISH*)packet.GetPacketData();
	res->wOpCode = GT_UPDATE_PUNISH;
	res->accountId = pPlayer->GetAccountID();
	res->dwDurationInMinute = 0;
	NTL_SAFE_WCSCPY(res->awchCharName, name.c_str());
	packet.SetPacketLen(sizeof(sGT_UPDATE_PUNISH));
	app->SendTo(app->GetChatServerSession(), &packet);
}

ACMD(do_go)
{
	CGameServer* app = (CGameServer*)g_pApp;

	/*
		@go x y z
	*/

	CNtlVector vLoc;

	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	vLoc.x = (float)atof(strToken.c_str());

	pToken->PopToPeek();
	strToken = pToken->PeekNextToken(NULL, &iLine);
	vLoc.y = (float)atof(strToken.c_str());

	pToken->PopToPeek();
	strToken = pToken->PeekNextToken(NULL, &iLine);
	vLoc.z = (float)atof(strToken.c_str());

	pPlayer->StartTeleport(vLoc, pPlayer->GetCurDir(), pPlayer->GetWorldID(), TELEPORT_TYPE_COMMAND);
}

ACMD(do_addtitle)
{
	CGameServer* app = (CGameServer*)g_pApp;

	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	TBLIDX titleIdx;
	if (!comutils::parse_uint(strToken, titleIdx)) {
		do_feedback(pPlayer, L"'%S' is not a valid title id. Title ids must be numbers.", strToken.c_str());
		return;
	}

	pToken->PopToPeek();
	strToken = pToken->PeekNextToken(NULL, &iLine);
	std::wstring name = std::wstring(strToken.begin(), strToken.end());
	const wchar_t* wname = name.c_str();
	CPlayer* target = g_pObjectManager->FindByName(wname);

	if (!target) {
		do_feedback(pPlayer, L"Could not find a player with name '%S'.", strToken.c_str());
		return;
	}

	sTBLDAT* tblDat = g_pTableContainer->GetCharTitleTable()->FindData(titleIdx);

	if (!tblDat) {
		do_feedback(pPlayer, L"Could not find a title with id '%u'.", titleIdx);
		return;
	}

	if (target->CheckCharTitle(titleIdx - 1) == false) // 303 is gm title
	{
		target->AddCharTitle(titleIdx - 1);
	}
}

ACMD(do_deltitle)
{
	CGameServer* app = (CGameServer*)g_pApp;

	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	TBLIDX titleIdx;
	if (!comutils::parse_uint(strToken, titleIdx)) {
		do_feedback(pPlayer, L"'%S' is not a valid title id. Title ids must be numbers.", strToken.c_str());
		return;
	}

	sTBLDAT* tblDat = g_pTableContainer->GetCharTitleTable()->FindData(titleIdx);
	if (!tblDat) {
		do_feedback(pPlayer, L"Could not find a title with id '%u'.", titleIdx);
		return;
	}

	pToken->PopToPeek();
	strToken = pToken->PeekNextToken(NULL, &iLine);
	std::wstring name = std::wstring(strToken.begin(), strToken.end());
	const wchar_t* wname = name.c_str();
	CPlayer* target = g_pObjectManager->FindByName(wname);

	if (!target) {
		do_feedback(pPlayer, L"Could not find a player with name '%S'.", strToken.c_str());
		return;
	};

	if (target->CheckCharTitle(titleIdx - 1) == true)
	{
		target->DelCharTitle(titleIdx - 1);
	}
}

ACMD(do_setitemduration)
{
	CGameServer* app = (CGameServer*)g_pApp;

	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	DWORD dwSeconds;
	if (!comutils::parse_ulong(strToken, dwSeconds)) {
		do_feedback(pPlayer, L"Duration must be specified in seconds. '%S' is not valid.", strToken.c_str());
		return;
	}

	CItem* pItem = pPlayer->GetPlayerItemContainer()->GetItem(1, 0); //get first item from inventory

	if (!pItem) {
		do_feedback(pPlayer, L"There is no item in your first inventory slot.");
		return;
	}

	if (pItem->GetDurationtype() != eDURATIONTYPE_FLATSUM) {
		do_feedback(pPlayer, L"The item in your first inventory slot can not have its remaining duration changed.");
		return;
	}

	pItem->SetUseEndTime(time(0) + dwSeconds);

	CNtlPacket pQry(sizeof(sGQ_ITEM_CHANGE_DURATIONTIME_REQ));
	sGQ_ITEM_CHANGE_DURATIONTIME_REQ* rQry = (sGQ_ITEM_CHANGE_DURATIONTIME_REQ*)pQry.GetPacketData();
	rQry->wOpCode = GQ_ITEM_CHANGE_DURATIONTIME_REQ;
	rQry->charId = pPlayer->GetCharID();
	rQry->handle = pPlayer->GetID();
	memcpy(&rQry->sItem, &pItem->GetItemData(), sizeof(sITEM_DATA));
	pQry.SetPacketLen(sizeof(sGQ_ITEM_CHANGE_DURATIONTIME_REQ));
	app->SendTo(app->GetQueryServerSession(), &pQry);
}

ACMD(do_bind)
{
	CGameServer* app = (CGameServer*)g_pApp;
	if (!pPlayer->GetCurWorld()) {
		do_feedback(pPlayer, L"You are not currently in a world.");
		return;
	}

	if (!pPlayer->GetCurWorld()->GetTbldat()->bDynamic == false) {
		do_feedback(pPlayer, L"You are currently in an instance.");
		return;
	}

	CNtlPacket packetQry(sizeof(sGQ_PC_UPDATE_BIND_REQ));
	sGQ_PC_UPDATE_BIND_REQ* resQry = (sGQ_PC_UPDATE_BIND_REQ*)packetQry.GetPacketData();
	resQry->wOpCode = GQ_PC_UPDATE_BIND_REQ;
	resQry->charId = pPlayer->GetCharID();
	resQry->handle = pPlayer->GetID();
	resQry->byBindType = DBO_BIND_TYPE_GM_TOOL;
	resQry->bindObjectTblidx = INVALID_TBLIDX;
	resQry->bindWorldId = pPlayer->GetWorldID();
	pPlayer->GetCurLoc().CopyTo(resQry->vBindLoc);
	pPlayer->GetCurDir().CopyTo(resQry->vBindDir);
	packetQry.SetPacketLen(sizeof(sGQ_PC_UPDATE_BIND_REQ));
	app->SendTo(app->GetQueryServerSession(), &packetQry);

	pPlayer->SetBindLoc(resQry->vBindLoc);
	pPlayer->SetBindDir(resQry->vBindDir);
	pPlayer->SetBindObjectTblidx(resQry->bindObjectTblidx);
	pPlayer->SetBindWorldID(resQry->bindWorldId);
	pPlayer->SetBindType(DBO_BIND_TYPE_GM_TOOL);


	do_feedback(pPlayer, L"Popo stone bound to your current location.");
}

ACMD(do_exp)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);

	if (strToken.compare("off") == 0)
	{
		pPlayer->SetExpReceiveDisabled(true);

		do_feedback(pPlayer, L"EXP Gain is now disabled for you.");
	}
	else if (strToken.compare("on") == 0)
	{
		pPlayer->SetExpReceiveDisabled(false);

		do_feedback(pPlayer, L"EXP Gain is now enabled for you.");
	}
	else {
		do_feedback(pPlayer, L"Second argument to @exp must be either 'on' or 'off'. '%S' is not valid.", strToken.c_str());
	}
}

ACMD(do_resetexp)
{
	if (pPlayer->GetExp() > 0)
	{
		CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_EXP));
		sGU_UPDATE_CHAR_EXP* res = (sGU_UPDATE_CHAR_EXP*)packet.GetPacketData();
		res->handle = pPlayer->GetID();
		res->wOpCode = GU_UPDATE_CHAR_EXP;
		res->dwCurExp = 0;
		res->dwAcquisitionExp = 0;
		res->dwIncreasedExp = 0;
		res->dwBonusExp = 0;
		packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_EXP));
		pPlayer->SendPacket(&packet);

		pPlayer->SetExp(0);
		
		do_feedback(pPlayer, L"Character experience reset back to 0.");
	}
	else {
		do_feedback(pPlayer, L"You have no experience to reset.");
	}
}

ACMD(do_starthoneybee)
{
	CGameServer* app = (CGameServer*)g_pApp;

	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	BYTE byHours;
	if (!comutils::parse_byte(strToken, byHours)) {
		do_feedback(pPlayer, L"Duration must be specified in hours. '%S' is not valid as it is not a number <= 12.", strToken.c_str());
		return;
	}

	if (byHours > 12)
		byHours = 12;

	/*
		@starthoneybee HOURS
	*/

	g_pHoneyBeeEvent->StartEvent(byHours);
	NTL_PRINT(PRINT_APP, "HoneybeeEvent Started");
}

ACMD(do_stophoneybee)
{
	CGameServer* app = (CGameServer*)g_pApp;

	g_pHoneyBeeEvent->EndEvent();
	NTL_PRINT(PRINT_APP, "HoneybeeEvent Stopped");
}

ACMD(do_deleteguild)
{
	CGameServer* app = (CGameServer*)g_pApp;

	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	const char* chName = strToken.c_str();

	/*
		@deleteguild GUILDNAME
	*/

	WCHAR* wchName = Ntl_MB2WC((char*)chName);

	CNtlPacket cPacket(sizeof(sGT_GUILD_DELETE));
	sGT_GUILD_DELETE* cRes = (sGT_GUILD_DELETE*)cPacket.GetPacketData();
	cRes->wOpCode = GT_GUILD_DELETE;
	cRes->gmCharId = pPlayer->GetCharID();
	NTL_SAFE_WCSCPY(cRes->wszGuildName, wchName);
	cPacket.SetPacketLen(sizeof(sGT_GUILD_DELETE));
	app->SendTo(app->GetChatServerSession(), &cPacket); //Send to chat server

	//clean memory
	Ntl_CleanUpHeapStringW(wchName);

	ERR_LOG(LOG_USER, "GM %u deleted Guild %s", pPlayer->GetCharID(), chName);
	do_feedback(pPlayer, L"Guild '%S' deleted.", strToken.c_str());
}

ACMD(do_cancelah)
{
	CGameServer* app = (CGameServer*)g_pApp;

	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	std::wstring name = std::wstring(strToken.begin(), strToken.end());
	const wchar_t* wname = name.c_str();
	CPlayer* target = g_pObjectManager->FindByName(wname);

	if (!target) {
		do_feedback(pPlayer, L"Could not find a player with name '%S'.", strToken.c_str());
		return;
	};

	CNtlPacket packet(sizeof(sGT_TENKAICHIDAISIJYOU_SELL_CANCEL_REQ));
	sGT_TENKAICHIDAISIJYOU_SELL_CANCEL_REQ* res = (sGT_TENKAICHIDAISIJYOU_SELL_CANCEL_REQ*)packet.GetPacketData();
	res->wOpCode = GT_TENKAICHIDAISIJYOU_SELL_CANCEL_REQ;
	res->charId = target->GetCharID();
	res->nItem = INVALID_ITEMID;
	packet.SetPacketLen(sizeof(sGT_TENKAICHIDAISIJYOU_SELL_CANCEL_REQ));
	app->SendTo(app->GetChatServerSession(), &packet);
}

ACMD(do_addmudosa)
{
	CGameServer* app = (CGameServer*)NtlSfxGetApp();

	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	DWORD dwMudosa;
	if (!comutils::parse_ulong(strToken, dwMudosa)) {
		do_feedback(pPlayer, L"Points must be specified in whole numbers. '%S' is not valid.", strToken.c_str());
		return;
	}

	pToken->PopToPeek();
	strToken = pToken->PeekNextToken(NULL, &iLine);
	std::wstring wstrName = std::wstring(strToken.begin(), strToken.end());
	CPlayer* pTarget = pPlayer;

	if (wcslen(wstrName.c_str()) > 0)
	{
		pTarget = g_pObjectManager->FindByName(wstrName.c_str());
		if (!pTarget) {
			do_feedback(pPlayer, L"Could not find a player with name '%S'.", strToken.c_str());
			return;
		}

		if (!pTarget->IsInitialized()) {
			do_feedback(pPlayer, L"Player '%S' is still loading in.", strToken.c_str());
			return;
		}
	}

	DWORD dwFinalMudosa = pTarget->GetMudosaPoints() + dwMudosa;
	pTarget->UpdateMudosaPoints(dwFinalMudosa);

	do_feedback(pPlayer, L"Added %u Mudosa points to player '%s'!", dwMudosa, pTarget->GetCharName());
}

ACMD(do_start)
{
	CGameServer* app = (CGameServer*)NtlSfxGetApp();

	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	BYTE byClass = (BYTE)atof(strToken.c_str());

	strToken = pToken->PeekNextToken(NULL, &iLine);
	int nlv = (int)atof(strToken.c_str());

	if (nlv != 60 && nlv != 70)
	{
		do_feedback(pPlayer, L"Level argument must be 60 or 70. '%u' is not a valid argument.", nlv);
		return;
	}

	if (pPlayer->GetClass() > PC_CLASS_1_LAST)
	{
		do_feedback(pPlayer, L"You already have a mastery class.");
		return;
	}

	switch (pPlayer->GetClass())
	{
		case PC_CLASS_HUMAN_FIGHTER:
		{
			if (byClass != PC_CLASS_STREET_FIGHTER && byClass != PC_CLASS_SWORD_MASTER)
			{
				do_feedback(pPlayer, L"As a martial artist the $classId argument must be either '%u' (Fighter) or '%u' (Swordsman).", PC_CLASS_STREET_FIGHTER, PC_CLASS_SWORD_MASTER);
				return;
			}
		}
		break;
		case PC_CLASS_HUMAN_MYSTIC:
		{
			if (byClass != PC_CLASS_CRANE_ROSHI && byClass != PC_CLASS_TURTLE_ROSHI)
			{
				do_feedback(pPlayer, L"As a spiritualist the $classId argument must be either '%u' (Crane Hermit) or '%u' (Turtle Hermit).", PC_CLASS_CRANE_ROSHI, PC_CLASS_TURTLE_ROSHI);
				return;
			}
		}
		break;
		case PC_CLASS_NAMEK_FIGHTER:
		{
			if (byClass != PC_CLASS_DARK_WARRIOR && byClass != PC_CLASS_SHADOW_KNIGHT)
			{
				do_feedback(pPlayer, L"As a warrior the $classId argument must be either '%u' (Dark Warrior) or '%u' (Shadow Knight).", PC_CLASS_DARK_WARRIOR, PC_CLASS_SHADOW_KNIGHT);
				return;
			}
		}
		break;
		case PC_CLASS_NAMEK_MYSTIC:
		{
			if (byClass != PC_CLASS_DENDEN_HEALER && byClass != PC_CLASS_POCO_SUMMONER)
			{
				do_feedback(pPlayer, L"As a dragon clan the $classId argument must be either '%u' (Dende Priest) or '%u' (Poko Priest).", PC_CLASS_DENDEN_HEALER, PC_CLASS_POCO_SUMMONER);
				return;
			}
		}
		break;
		case PC_CLASS_MIGHTY_MAJIN:
		{
			if (byClass != PC_CLASS_ULTI_MA && byClass != PC_CLASS_GRAND_MA)
			{
				do_feedback(pPlayer, L"As a mighty majin the $classId argument must be either '%u' (Ultimate Majin) or '%u' (Grand Chef Majin).", PC_CLASS_ULTI_MA, PC_CLASS_GRAND_MA);
				return;
			}
		}
		break;
		case PC_CLASS_WONDER_MAJIN:
		{
			if (byClass != PC_CLASS_PLAS_MA && byClass != PC_CLASS_KAR_MA)
			{
				do_feedback(pPlayer, L"As a wonder majin the $classId argument must be either '%u' (Plasma Majin) or '%u' (Karma Majin).", PC_CLASS_PLAS_MA, PC_CLASS_KAR_MA);
				return;
			}
		}
		break;

		default: return;
	}

	if (pPlayer->GetLevel() >= nlv)
	{
		do_feedback(pPlayer, L"Your level is already higher than or equal to '%u'", nlv);
		return;
	}

	// update level
	pPlayer->LevelUp(0, nlv - pPlayer->GetLevel());

	// update class
	pPlayer->UpdateClass(byClass);

	// learn passives
	TBLIDX masterPassive = INVALID_TBLIDX;
	TBLIDX itemPassive = INVALID_TBLIDX;
	TBLIDX flightPassive = INVALID_TBLIDX;

	switch (pPlayer->GetClass())
	{
		case PC_CLASS_STREET_FIGHTER:
		{
			masterPassive = 729991;
			itemPassive = 11140026;
			flightPassive = 11120142;
		}
		break;
		case PC_CLASS_SWORD_MASTER:
		{
			masterPassive = 829991;
			itemPassive = 11140026;
			flightPassive = 11120142;
		}
		break;
		case PC_CLASS_CRANE_ROSHI:
		{
			masterPassive = 929991;
			itemPassive = 11140027;
			flightPassive = 11120143;
		}
		break;
		case PC_CLASS_TURTLE_ROSHI:
		{
			masterPassive = 1029991;
			itemPassive = 11140027;
			flightPassive = 11120143;
		}
		break;
		case PC_CLASS_DARK_WARRIOR:
		{
			masterPassive = 1329991;
			itemPassive = 11140028;
			flightPassive = 11120144;
		}
		break;
		case PC_CLASS_SHADOW_KNIGHT:
		{
			masterPassive = 1429991;
			itemPassive = 11140028;
			flightPassive = 11120144;
		}
		break;
		case PC_CLASS_DENDEN_HEALER:
		{
			masterPassive = 1529991;
			itemPassive = 11140029;
			flightPassive = 11120145;
		}
		break;
		case PC_CLASS_POCO_SUMMONER:
		{
			masterPassive = 1629991;
			itemPassive = 11140029;
			flightPassive = 11120145;
		}
		break;
		case PC_CLASS_ULTI_MA:
		{
			masterPassive = 1729991;
			itemPassive = 11140030;
			flightPassive = 11120146;
		}
		break;
		case PC_CLASS_GRAND_MA:
		{
			masterPassive = 1829991;
			itemPassive = 11140030;
			flightPassive = 11120146;
		}
		break;
		case PC_CLASS_PLAS_MA:
		{
			masterPassive = 1929991;
			itemPassive = 11140031;
			flightPassive = 11120147;
		}
		break;
		case PC_CLASS_KAR_MA:
		{
			masterPassive = 2029991;
			itemPassive = 11140031;
			flightPassive = 11120147;
		}
		break;

		default: break;
	}

	WORD wTemp;

	pPlayer->GetSkillManager()->LearnSkill(masterPassive, wTemp, false);

	g_pItemManager->CreateItem(pPlayer, itemPassive, 1);
	g_pItemManager->CreateItem(pPlayer, flightPassive, 1);
}

ACMD(do_createloot)
{
	pToken->PopToPeek();
	std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	TBLIDX ItemId;
	if (!comutils::parse_uint(strToken, ItemId)) {
		do_feedback(pPlayer, L"Item ids must be whole numbers '%S' is not valid.", strToken.c_str());
		return;
	}

	strToken = pToken->PeekNextToken(NULL, &iLine);
	int nCount;
	if (!comutils::parse_int(strToken, nCount)) {
		do_feedback(pPlayer, L"The amount of drops to create must be a whole number.", strToken);
		return;
	}

	sITEM_TBLDAT* pTblData = (sITEM_TBLDAT*)g_pTableContainer->GetItemTable()->FindData(ItemId);
	if (!pTblData)
	{
		do_feedback(pPlayer, L"Could not find an item with id '%u'.", ItemId);
		return;
	}

	if (nCount > 100)
		nCount = 100;

	for (int i = 0; i < nCount; i++)
	{
		CNtlVector vPos(pPlayer->GetCurLoc());

		sVECTOR3 vec;
		vPos.CopyTo(vec.x, vec.y, vec.z);

		vec.x += RandomRangeF(-100.0f, 100.0f);
		vec.z += RandomRangeF(-100.0f, 100.0f);

		CItemDrop* pBall = g_pItemManager->CreateSingleDrop(100.f, ItemId);
		if (pBall)
		{
			pBall->AddToGround(pPlayer->GetWorldID(), vec);
		}
	}
}

ACMD(do_test)
{
	pPlayer->SendCharStateFalling(NTL_MOVE_NONE);
}

ACMD(do_delnpc) {

	//pToken->PopToPeek();
	//std::string strToken = pToken->PeekNextToken(NULL, &iLine);
	//float fSpeed = (float)atof(strToken.c_str());

	CCharacter* pTarget = g_pObjectManager->GetChar(pPlayer->GetTargetHandle());
	if (pTarget && !pTarget->IsPC())
	{
		g_pObjectManager->DestroyCharacter(pTarget);
		//g_pObjectManager->DeleteUID(pTarget->GetID());

		do_feedback(pPlayer, L"Target removed!");
	}
	else {
		do_feedback(pPlayer, L"You do not have a valid target selected.");
	}
}

ACMD(do_setnpcfriendly)
{
	CCharacter* pTarget = g_pObjectManager->GetChar(pPlayer->GetTargetHandle());
	if (pTarget && pTarget->IsMonster())
	{
		pTarget->ChangeFightMode(false);

		CBotAiState_Idle* pState = new CBotAiState_Idle(pTarget->GetBotController()->GetContolObject());
		pTarget->GetBotController()->ChangeAiState(pState);
		pTarget->ChangeTarget(INVALID_HOBJECT);
		pTarget->GetStateManager()->AddConditionFlags(CHARCOND_FLAG_ATTACK_DISALLOW, true);
		pTarget->GetBotController()->SetControlBlock(true);

		do_feedback(pPlayer, L"Target permanently set to idle state.");
	}
	else {
		do_feedback(pPlayer, L"You do not have a valid target selected.");
	}
}

ACMD(do_getnpcinfo)
{
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
