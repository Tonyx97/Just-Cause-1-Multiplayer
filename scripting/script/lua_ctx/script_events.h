#pragma once

namespace script::event
{
#ifdef JC_CLIENT
	static inline std::string ON_RENDER = "onRender";
	static inline std::string ON_PRE_RENDER = "onPreRender";
	static inline std::string ON_PLAYER_JOIN = "onClientPlayerJoin";
	static inline std::string ON_PLAYER_QUIT= "onClientPlayerQuit";
	static inline std::string ON_PLAYER_KILLED = "onClientPlayerKilled";
	static inline std::string ON_RSRC_START = "onClientResourceStart";
	static inline std::string ON_RSRC_STOP = "onClientResourceStop";
	static inline std::string ON_PICKUP_HIT = "onClientPickupHit";
#else
	static inline std::string ON_TICK = "onTick";
	static inline std::string ON_PLAYER_JOIN = "onPlayerJoin";
	static inline std::string ON_PLAYER_QUIT = "onPlayerQuit";
	static inline std::string ON_PLAYER_KILLED = "onPlayerKilled";
	static inline std::string ON_RSRC_START = "onResourceStart";
	static inline std::string ON_RSRC_STOP = "onResourceStop";
#endif
}