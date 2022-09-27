#pragma once

namespace netcp
{
	static constexpr uint16_t CLIENT_TO_SERVER_GAME_PORT			= 50500;							// UDP port: handles game-specific traffic
	static constexpr uint16_t CLIENT_TO_SERVER_TCP_PORT				= CLIENT_TO_SERVER_GAME_PORT + 1;	// TCP port: handles file tcp packets between client and server (i.e. files)
	static constexpr uint16_t CLIENT_OR_SERVER_TO_MS_PORT			= CLIENT_TO_SERVER_TCP_PORT + 1;	// TCP port: handles comms between client/server and masterserver
}