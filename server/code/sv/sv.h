#pragma once

#include <shared_mp/object_lists.h>

class Server : public ObjectLists
{
private:
	ENetHost* sv = nullptr;

public:
	bool init();
	void destroy();
	void setup_channels();
	void tick();
	void send_global_packets();

	ENetHost* get_host() const { return sv; };
};

inline Server* g_sv = nullptr;