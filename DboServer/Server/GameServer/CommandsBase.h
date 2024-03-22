#pragma once
#include "CPlayer.h"
#include "NtlTokenizer.h"
#include "memory"
#include "stdexcept"
#include "NtlPacketGU.h"

class CommandsBase
{
	protected:

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
};

