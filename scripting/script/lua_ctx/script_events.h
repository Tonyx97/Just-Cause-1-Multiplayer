#pragma once

namespace jc::script::event
{
#ifdef JC_CLIENT
	static constexpr std::string_view ON_JOIN = "onClientJoin";
#else
	static constexpr std::string_view ON_JOIN = "onPlayerJoin";
#endif
}