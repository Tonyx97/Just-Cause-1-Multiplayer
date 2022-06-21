#pragma once

#include <enet/interface.h>

class Server
{
private:
	std::unordered_map<NID, PlayerClient*> player_clients;

	ENetHost* sv = nullptr;

public:
	bool init();
	void destroy();
	void setup_channels();
	void add_player_client(ENetEvent& e);
	void remove_player_client(ENetEvent& e);
	void tick();
	void send_global_packets();

	PlayerClient* get_player_client_by_nid(NID nid);

	ENetHost* get_host() const { return sv; };
};

inline Server* g_sv = nullptr;