#include <defs/standard.h>

#include "weapon.h"

#include <mp/net.h>

#include <game/transform/transform.h>

#include <shared_mp/objs/vehicle_net.h>

#include <game/object/character/character.h>
#include <game/object/character/comps/stance_controller.h>
#include <game/object/vehicle/vehicle.h>
#include <game/object/weapon/weapon.h>

#include <game/sys/sound/sound_system.h>
#include <game/sys/world/world.h>

namespace jc::weapon::hook
{
	// patch to properly create synced shots in remote players,
	// since the spread is clientside, the bullets could hit or not in remote players causing
	// a sense of desync, this patch will sync the rand seed across all players and each
	// player will generate the correct set of PRGN so they stay the same
	//
	DEFINE_INLINE_HOOK_IMPL(fire_bullet, 0x61F6AB)
	{
		const auto weapon = ihp->read_ebp<Weapon*>(0x17C);
		const auto final_muzzle_transform = ihp->at_ebp<Transform>(0x9C);

		if (const auto owner = weapon->get_owner())
		{
			if (const auto player = g_net->get_player_by_character(owner))
			{
				if (const auto weapon_info = weapon->get_info())
				{
					// hand guns first

					if (!weapon_info->is_vehicle_weapon())
					{
						// if the player is local then just get our aim target and calculate the
						// proper direction, otherwise, we want to grab the muzzle and aim target
						// from the remote Player instance

						const auto muzzle = player->get_muzzle_position();

						auto direction = player->get_bullet_direction();

						// modify direction if this weapon fires more than 1 bullet at the same time
						// the rand seed used to generate the direction modifier is synced across all players
						// so all bullets should be the same in other's players instances

						if (player->should_use_multiple_rand_seed())
						{
							const bool is_crouching = player->is_crouching();
							const auto spread_factor = is_crouching ? Character::CROUCH_SPREAD_MODIFIER() : Character::STAND_SPREAD_MODIFIER();
							const auto spread = spread_factor * glm::radians(1.f - weapon_info->get_accuracy(false));
							const auto rand_vector = player->generate_bullet_rand_spread() * spread;
							const auto rotation_matrix = glm::yawPitchRoll(rand_vector.x, rand_vector.y, rand_vector.z);

							direction = vec4(direction, 0.f) * rotation_matrix;
						}

						*final_muzzle_transform = Transform::look_at(muzzle, muzzle + direction);

						return;
					}
					else
					{
						// handle vehicle weapons

						if (const auto vehicle_net = player->get_vehicle(); vehicle_net)
						{
							if (const auto seat = owner->get_vehicle_seat(); seat && seat->get_type() == VehicleSeat_Special)
							{
								// if the shooter is in the special seat, they must be using a mounted gun

								if (const auto fire_info = vehicle_net->get_mounted_gun_fire_info())
									*final_muzzle_transform = Transform::look_at(fire_info->muzzle, fire_info->muzzle + fire_info->direction);
							}
							else
							{
								// actual vehicle weapons

								if (const auto fire_info = vehicle_net->get_fire_info_from_weapon(weapon))
									*final_muzzle_transform = Transform::look_at(fire_info->base.muzzle, fire_info->base.muzzle + fire_info->base.direction);
							}
						}

						return;
					}
				}
			}
		}

		const auto muzzle = weapon->get_muzzle_position();

		*final_muzzle_transform = Transform::look_at(muzzle, muzzle + weapon->get_aim_target());
	}

	DEFINE_HOOK_THISCALL_S(play_fire_sound, jc::weapon::fn::PLAY_FIRE_SOUND, void, Weapon* weapon)
	{
		play_fire_sound_hook(weapon);
	}

	void enable(bool apply)
	{
		fire_bullet_hook.hook(apply, 0x61F6C6);
		play_fire_sound_hook.hook(apply);
	}
}

std::tuple<vec2, vec2> WeaponInfo::CALCULATE_ICON_UVS(uint8_t icon)
{
	float grid_x = static_cast<float>(icon % 8),
		  grid_y = static_cast<float>(icon / 8);

	const auto uv0 = vec2(grid_x * 0.125f, grid_y * 0.25f);
	const auto uv1 = vec2(uv0.x + 0.125f, uv0.y + 0.25f);

	return { uv0, uv1 };
}

void WeaponInfo::set_max_mag_ammo(int32_t v)
{
	jc::write(v, this, jc::weapon_info::MAX_MAG_AMMO);
}

void WeaponInfo::set_bullets_to_fire(int32_t v)
{
	jc::write(v, this, jc::weapon_info::BULLETS_TO_FIRE);
}

void WeaponInfo::set_muzzle_offset(const vec3& v)
{
	jc::write(v, this, jc::weapon_info::MUZZLE_OFFSET);
}

void WeaponInfo::set_fire_sound_id(int32_t v)
{
	jc::write(v, this, jc::weapon_info::FIRE_SOUND_ID);
}

void WeaponInfo::set_bullet_force1(float v)
{
	jc::write(v, this, jc::weapon_info::BULLET_FORCE1);
}

void WeaponInfo::set_bullet_force2(float v)
{
	jc::write(v, this, jc::weapon_info::BULLET_FORCE2);
}

void WeaponInfo::set_can_create_shells(bool v)
{
	jc::write(v, this, jc::weapon_info::CAN_CREATE_SHELLS);
}

void WeaponInfo::set_accuracy(bool ai, float v)
{
	if (ai)
		jc::write(v, this, jc::weapon_info::ACCURACY_AI);
	else jc::write(v, this, jc::weapon_info::ACCURACY_PLAYER);
}

void WeaponInfo::set_infinite_ammo(bool v)
{
	jc::write(v, this, jc::weapon_info::INFINITE_AMMO);
}

bool WeaponInfo::can_create_shells() const
{
	return jc::read<bool>(this, jc::weapon_info::CAN_CREATE_SHELLS);
}

bool WeaponInfo::is_vehicle_weapon() const
{
	return get_icon_id() == 0;
}

bool WeaponInfo::has_infinite_ammo() const
{
	return jc::read<bool>(this, jc::weapon_info::INFINITE_AMMO);
}

uint8_t WeaponInfo::get_id() const
{
	return static_cast<uint8_t>(jc::read<int32_t>(this, jc::weapon_info::ID));
}

int32_t WeaponInfo::get_bullet_type() const
{
	return jc::read<int32_t>(this, jc::weapon_info::BULLET_TYPE);
}

int32_t WeaponInfo::get_type_id() const
{
	return jc::read<int32_t>(this, jc::weapon_info::TYPE_ID);
}

int32_t WeaponInfo::get_icon_id() const
{
	return jc::read<int32_t>(this, jc::weapon_info::ICON_ID);
}

int32_t WeaponInfo::get_max_mag_ammo()
{
	return jc::read<int32_t>(this, jc::weapon_info::MAX_MAG_AMMO);
}

int32_t WeaponInfo::get_bullets_to_fire()
{
	return jc::read<int32_t>(this, jc::weapon_info::BULLETS_TO_FIRE);
}

int32_t WeaponInfo::get_fire_sound_id()
{
	return jc::read<int32_t>(this, jc::weapon_info::FIRE_SOUND_ID);
}

float WeaponInfo::get_bullet_force1() const
{
	return jc::read<float>(this, jc::weapon_info::BULLET_FORCE1);
}

float WeaponInfo::get_bullet_force2() const
{
	return jc::read<float>(this, jc::weapon_info::BULLET_FORCE2);
}

float WeaponInfo::get_accuracy(bool ai) const
{
	return ai ? jc::read<float>(this, jc::weapon_info::ACCURACY_AI) :
				jc::read<float>(this, jc::weapon_info::ACCURACY_PLAYER);
}

std::tuple<vec2, vec2> WeaponInfo::get_icon_uvs() const
{
	const auto icon = get_icon_id();

	return CALCULATE_ICON_UVS(icon);
}

vec3 WeaponInfo::get_muzzle_offset()
{
	return jc::read<vec3>(this, jc::weapon_info::MUZZLE_OFFSET);
}

jc::stl::string* WeaponInfo::get_type_name()
{
	return REF(jc::stl::string*, this, jc::weapon_info::TYPE_NAME);
}

jc::stl::string* WeaponInfo::get_name()
{
	return REF(jc::stl::string*, this, jc::weapon_info::NAME);
}

void Weapon::set_ammo(int32_t v)
{
	jc::write(v, this, jc::weapon::AMMO);
}

void Weapon::set_last_shot_time(float v)
{
	jc::write(v, this, jc::weapon::LAST_SHOT_TIME);
}

void Weapon::set_muzzle_position(const vec3& v)
{
	jc::write(v, this, jc::weapon::MUZZLE_POSITION);
}

void Weapon::set_last_muzzle_position(const vec3& v)
{
	jc::write(v, this, jc::weapon::LAST_MUZZLE_POSITION);
}

void Weapon::set_last_muzzle_transform(const Transform& v)
{
	jc::write(v, this, jc::weapon::LAST_MUZZLE_TRANSFORM);
}

void Weapon::set_aim_target(const vec3& v)
{
	jc::write(v, this, jc::weapon::AIM_TARGET);
}

void Weapon::force_fire()
{
	jc::write(true, this, jc::weapon::CAN_FIRE);
}

void Weapon::set_enabled(bool v)
{
	jc::v_call(this, jc::weapon::vt::SET_ENABLED, v);
}

bool Weapon::is_reloading() const
{
	return jc::read<bool>(this, jc::weapon::RELOADING);
}

bool Weapon::can_fire() const
{
	return jc::read<bool>(this, jc::weapon::CAN_FIRE);
}

bool Weapon::is_trigger_pulled() const
{
	return jc::read<bool>(this, jc::weapon::TRIGGER_PULLED);
}

uint8_t Weapon::get_id() const
{
    return get_info()->get_id();
}

int32_t Weapon::get_ammo()
{
	return jc::read<int32_t>(this, jc::weapon::AMMO);
}

int32_t Weapon::get_bullets_fired()
{
	return jc::read<int32_t>(this, jc::weapon::TOTAL_BULLETS_FIRED);
}

int32_t Weapon::get_bullets_fired_now()
{
	return jc::read<int32_t>(this, jc::weapon::TOTAL_BULLETS_FIRED_NOW);
}

float Weapon::get_last_shot_time()
{
	return jc::read<float>(this, jc::weapon::LAST_SHOT_TIME);
}

WeaponInfo* Weapon::get_info() const
{
	return jc::read<WeaponInfo*>(this, jc::weapon::WEAPON_INFO);
}

Character* Weapon::get_owner() const
{
	return jc::read<Character*>(this, jc::weapon::CHARACTER_OWNER);
}

vec3 Weapon::get_muzzle_position()
{
	return get_muzzle_transform()->get_position();
}

vec3 Weapon::get_aim_target()
{
	return jc::read<vec3>(this, jc::weapon::AIM_TARGET);
}

Transform* Weapon::get_grip_transform()
{
	return REF(Transform*, this, jc::weapon::GRIP_TRANSFORM);
}

Transform* Weapon::get_last_muzzle_transform()
{
	return REF(Transform*, this, jc::weapon::LAST_MUZZLE_TRANSFORM);
}

Transform* Weapon::get_muzzle_transform()
{
	return REF(Transform*, this, jc::weapon::MUZZLE_TRANSFORM);
}

Transform* Weapon::get_last_ejector_transform()
{
	return REF(Transform*, this, jc::weapon::LAST_EJECTOR_TRANSFORM);
}