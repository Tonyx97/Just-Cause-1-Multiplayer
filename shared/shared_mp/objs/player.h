#pragma once

#ifndef JC_LOADER

#include <shared_mp/objs/net_object.h>

class PlayerClient;

class Player : public NetObject
{
private:

	PlayerClient* client = nullptr;

public:

	static constexpr uint32_t TYPE() { return NetObject_Player; }

	uint32_t get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	Player(PlayerClient* pc, NID nid);
#else
	Player(PlayerClient* pc);
#endif
	~Player();

	PlayerClient* get_client() const { return client; }
};
#endif