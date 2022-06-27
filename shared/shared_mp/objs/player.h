#pragma once

#include <shared_mp/objs/net_object.h>

#include <game/transform/transform.h>

class PlayerClient;
class CharacterHandle;

struct PlayerStaticInfo
{
	std::string nick;

	uint32_t skin = 0u;
};

class Player : public NetObject
{
private:

	struct Info
	{
		Transform transform;
	} info {};

	CharacterHandle* handle = nullptr;

	PlayerStaticInfo static_info {};

	PlayerClient* client = nullptr;

public:

	static constexpr uint32_t TYPE() { return NetObject_Player; }

	uint32_t get_type() const override { return TYPE(); }

#ifdef JC_CLIENT
	Player(PlayerClient* pc, NID nid);

	void verify_exec(const std::function<void(Character*)>& fn);
#else
	Player(PlayerClient* pc);

	void verify_exec(auto fn) {}
#endif
	~Player();

	bool spawn() override;

	const PlayerStaticInfo& get_static_info() const { return static_info; }

	PlayerClient* get_client() const { return client; }

	// info getters/setters

	void set_transform(const Transform& transform);

	Transform get_transform() const;

	// static info getters/setters

	void set_nick(const std::string& v);
	void set_skin(uint32_t v);

	uint32_t get_skin() const;

	const std::string& get_nick() const { return static_info.nick; }
};