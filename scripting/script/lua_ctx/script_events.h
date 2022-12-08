#pragma once

namespace script::event
{
#ifdef JC_CLIENT
	//
	static inline std::string ON_RENDER = "onRender";

	//
	static inline std::string ON_PRE_RENDER = "onPreRender";

	// params: Player
	//
	static inline std::string ON_PLAYER_JOIN = "onClientPlayerJoin";

	// params: Player
	//
	static inline std::string ON_PLAYER_QUIT = "onClientPlayerQuit";

	// params: Player
	//
	static inline std::string ON_PLAYER_KILLED = "onClientPlayerKilled";

	// params: Resource
	//
	static inline std::string ON_RSRC_START = "onClientResourceStart";

	// params: Resource
	//
	static inline std::string ON_RSRC_STOP = "onClientResourceStop";

	// params: Player, Pickup
	// cancellable: if event is cancelled the pickup won't be picked up
	//
	static inline std::string ON_PICKUP_HIT = "onClientPickupHit";
#else
	//
	static inline std::string ON_TICK = "onTick";

	// params: Player
	//
	static inline std::string ON_PLAYER_JOIN = "onPlayerJoin";

	// params: Player
	//
	static inline std::string ON_PLAYER_QUIT = "onPlayerQuit";

	// params: Player
	//
	static inline std::string ON_PLAYER_KILLED = "onPlayerKilled";

	// params: Resource
	//
	static inline std::string ON_RSRC_START = "onResourceStart";

	// params: Resource
	//
	static inline std::string ON_RSRC_STOP = "onResourceStop";
#endif
}