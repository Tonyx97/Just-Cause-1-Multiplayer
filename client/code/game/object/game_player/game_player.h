#pragma once

namespace jc::game_player
{
	static constexpr uint32_t CHARACTER = 0x1C; // Character*
	
	static constexpr uint32_t PARACHUTE			= 0x24;		// Parachute
	static constexpr uint32_t RIGHT				= 0xF8;		// float
	static constexpr uint32_t FORWARD			= 0xFC;		// float
	static constexpr uint32_t STATE				= 0x130;	// int
	static constexpr uint32_t INPUT_BLOCKED		= 0x1D8;	// bool
	static constexpr uint32_t WASD_CONTROLLER	= 0x2BC;	// WASDController*

	namespace fn
	{
		static constexpr uint32_t CTOR									= 0x4C03B0;
		static constexpr uint32_t UPDATE								= 0x4CB8C0;
		static constexpr uint32_t RESET_WEAPON_BELT						= 0x4C35E0;
		static constexpr uint32_t TELEPORT_TO_CLOSEST_SAFEHOUSE			= 0x4D0490;
		static constexpr uint32_t SET_CURRENT_WEAPON_SCOPE_FOV			= 0x4D0020;
		static constexpr uint32_t INCREASE_CURRENT_WEAPON_SCOPE_FOV		= 0x4D0000;
		static constexpr uint32_t DECREASE_CURRENT_WEAPON_SCOPE_FOV		= 0x4CFFE0;
		static constexpr uint32_t SWITCH_TO_PREVIOUS_WEAPON				= 0x4CDD80;
		static constexpr uint32_t SWITCH_TO_NEXT_WEAPON					= 0x4CDDF0;
		static constexpr uint32_t LOAD_PARACHUTE_MODEL					= 0x4C3510;
		static constexpr uint32_t DISPATCH_SWIMMING						= 0x4C8470;
		static constexpr uint32_t CROUCH								= 0x4CDEC0;
		static constexpr uint32_t UNCROUCH								= 0x4CDEA0;
		static constexpr uint32_t DRAW_GRAPPLING_HOOK					= 0x4CCC40;
	}

	namespace hook
	{
		void enable(bool apply);
	}
}

class WASDController;
class Character;
class Parachute;

DEFINE_ENUM(GamePlayerState, int32_t)
{
	GamePlayerState_Unk					= 0,
	GamePlayerState_StrafingWithWeapon	= 1,
	GamePlayerState_Normal				= 2,
	GamePlayerState_SkyDiving			= 3,
	GamePlayerState_Paragliding			= 4,
	GamePlayerState_UsingWeaponScope	= 5,
};

class GamePlayer
{
private:

	void load_parachute_model();

public:

	static GamePlayer* CREATE();

	void destroy();
	void update();
	void set_character(Character* character);
	void reset_weapon_belt();
	void teleport_to_closest_safehouse();
	void increase_current_weapon_scope_fov(float factor = 0.5f);
	void decrease_current_weapon_scope_fov(float factor = 0.5f);
	void set_state(int32_t v);
	void set_right(float v);
	void set_forward(float v);
	void crouch(bool enabled, bool sync = true);
	void dispatch_swimming();
	void block_key_input(bool blocked);
	void draw_grappling_hook(const Transform& begin, const vec3& end, bool hooked = false);

	int32_t get_state_id() const;

	float get_right() const;
	float get_forward() const;

	Character* get_character() const;
	Parachute* get_parachute() const;
};