#pragma once

namespace jc::script::globals
{
#ifdef JC_CLIENT
	// client globals

	static constexpr auto LOCALPLAYER = "localPlayer";
#else
	// server globals
#endif

	// shared globals

	static constexpr auto SCRIPT_INSTANCE = "SCRIPT_INSTANCE";
	static constexpr auto RESOURCE = "RESOURCE";
	static constexpr auto RSRC_PATH = "RESOURCE_PATH";
	static constexpr auto RSRC_NAME = "RESOURCE_NAME";
}