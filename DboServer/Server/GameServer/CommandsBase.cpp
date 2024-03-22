#include "stdafx.h"
#include "CommandsBase.h"

//template<typename ... Args>
//static void CommandsBase::do_feedback(CPlayer* pTarget, const std::string& format, Args ... args)
//{
//	int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
//	if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
//	auto size = static_cast<size_t>(size_s);
//	std::unique_ptr<char[]> buf(new char[size]);
//	std::snprintf(buf.get(), size, format.c_str(), args ...);
//	std::string text = std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
//
//
//	CNtlPacket packet(sizeof(sGU_SYSTEM_DISPLAY_TEXT));
//	sGU_SYSTEM_DISPLAY_TEXT* res = (sGU_SYSTEM_DISPLAY_TEXT*)packet.GetPacketData();
//	res->wOpCode = GU_SYSTEM_DISPLAY_TEXT;
//	res->wMessageLengthInUnicode = (WORD)text.length();
//	res->byDisplayType = SERVER_TEXT_SYSTEM;
//	wcscpy_s(res->awchMessage, NTL_MAX_LENGTH_OF_CHAT_MESSAGE + 1, s2ws(text).c_str());
//	packet.SetPacketLen(sizeof(sGU_SYSTEM_DISPLAY_TEXT));
//	pTarget->SendPacket(&packet);
//}