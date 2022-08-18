#pragma once

namespace jc::character_handle
{
	static constexpr uint32_t CHARACTER				= 0x11C;	// Character*
	static constexpr uint32_t HANDLE_BASE			= 0x14;	// CharacterHandleBase*
	static constexpr uint32_t PENDING_TO_DESTROY	= 0x196;	// bool
	static constexpr uint32_t LOOKING_AT_ANY_NPC	= 0x198;	// bool

	namespace fn
	{
		static constexpr uint32_t GET_FREE_HANDLE  = 0x761210;
		static constexpr uint32_t CREATE_CHARACTER = 0x8ADE60;
	}
}

class Character;
class Transform;

struct CharacterInfo;

class CharacterHandleBase
{

};

class CharacterHandle
{
private:

	void set_pending_destroy();

public:

	static CharacterHandle* GET_FREE_HANDLE();

	void destroy();
	void respawn();

	bool is_looking_at_any_npc() const;

	CharacterHandle* create(CharacterInfo* info, Transform* transform, int weapon_id);

	CharacterHandleBase* get_base() const;

	Character* get_character() const;
	Character* get_character_unsafe() const;
};