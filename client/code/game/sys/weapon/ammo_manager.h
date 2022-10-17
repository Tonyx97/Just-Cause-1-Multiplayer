#pragma once

namespace jc::ammo_manager
{
	static constexpr uint32_t SINGLETON = 0xD84EC4; // AmmoManager*

	static constexpr uint32_t BULLET_LIST	 = 0x70; // vector<Bullet*>
	static constexpr uint32_t MAX_AMMO_SLOTA = 0x84; // int32_t
	static constexpr uint32_t MAX_AMMO_SLOTB = 0x88; // int32_t
	static constexpr uint32_t MAX_AMMO_SLOTC = 0x8C; // int32_t
	static constexpr uint32_t MAX_AMMO_SLOTD = 0x90; // int32_t
	static constexpr uint32_t MAX_AMMO_SLOTE = 0x94; // int32_t
	static constexpr uint32_t MAX_AMMO_SLOTF = 0x98; // int32_t
	static constexpr uint32_t MAX_AMMO_SLOTG = 0x9C; // int32_t
	static constexpr uint32_t MAX_AMMO_SLOTH = 0xA0; // int32_t
}

using bullet_iteration_t = std::function<void(int, class Bullet*)>;

class AmmoManager
{
public:
	void init();
	void destroy();
	void set_max_ammo_for_slot(uint8_t slot, int32_t v);

	int for_each_bullet(const bullet_iteration_t& fn);

	int32_t get_max_ammo_for_slot(uint8_t slot);
};

inline Singleton<AmmoManager, jc::ammo_manager::SINGLETON> g_ammo;