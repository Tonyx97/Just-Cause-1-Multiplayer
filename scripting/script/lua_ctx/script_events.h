#pragma once

namespace jc::script::event
{
#ifdef JC_CLIENT
	static inline std::string ON_JOIN = "onClientPlayerJoin";
#else
	static inline std::string ON_JOIN = "onPlayerJoin";
#endif
}