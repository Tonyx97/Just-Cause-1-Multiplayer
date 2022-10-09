#pragma once

namespace jc::script::globals
{
	static constexpr auto SCRIPT_INSTANCE = "SCRIPT_INSTANCE";
	static constexpr auto RSRC_PATH = "RESOURCE_PATH";
	static constexpr auto RSRC_NAME = "RESOURCE_NAME";

#ifdef JC_CLIENT
#else
#endif
}