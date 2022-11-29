#pragma once

namespace netcp
{
	static constexpr uint16_t CLIENT_TO_SERVER_GAME_PORT	= 22500;							// UDP port: handles game-specific traffic
	static constexpr uint16_t CLIENT_TO_SERVER_TCP_PORT		= CLIENT_TO_SERVER_GAME_PORT + 1;	// TCP port: handles file tcp packets between client and server (i.e. files)
	static constexpr uint16_t CLIENT_TO_MS_PORT				= CLIENT_TO_SERVER_TCP_PORT + 1;	// TCP port: handles comms between client and masterserver
	static constexpr uint16_t SERVER_TO_MS_PORT				= CLIENT_TO_MS_PORT + 1;			// TCP port: handles comms between server and masterserver
}