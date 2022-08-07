#include <defs/standard.h>

#include "character.h"

#include "../weapon/weapon.h"
#include "../weapon/weapon_belt.h"
#include "../character_handle/character_handle.h"

#include <game/transform/transform.h>
#include <game/sys/world.h>
#include <game/sys/physics.h>
#include <game/sys/ai_core.h>
#include <game/sys/resource_streamer.h>
#include <game/sys/player_global_info.h>
#include <game/sys/weapon_system.h>
#include <game/sys/particle_system.h>

#include <havok/character_proxy.h>
#include <havok/motion_state.h>
#include <havok/simple_shape_phantom.h>

#include <mp/net.h>

#include "../vars/anims.h"
#include "../exported_entity/exported_entity.h"
#include "../resource/ee_resource.h"

// hooks

#include "comps/stance_controller.h"

namespace jc::character::hook
{
	static void* test = nullptr;

	DEFINE_HOOK_THISCALL(update, 0x58FC80, void, Character* character, float delta)
	{
		/*const auto position = character->get_stomach_bone_position();

		if (character->get_flags() & (1 << 31))
		{
			if (!test)
			{
				test = g_particle->spawn_fx("firesmall_static", position, {}, {}, true, false);

				log(RED, "fire: {:x}", ptr(test));
			}

			character->set_hp(character->get_real_hp() - delta * 500.f);
		}
		else test = nullptr;

		if (test)
			jc::write(position, test, 0x4 + 0x30);*/

		return update_hook.call(character, delta);
	}

	DEFINE_HOOK_THISCALL(set_body_stance, 0x625750, void, BodyStanceController* stance, uint32_t id)
	{
		if (const auto local_char = g_world->get_localplayer_character())
		{
			const auto character = stance->get_character();

			if (character == local_char)
			{
				switch (RET_ADDRESS)
				{
				case 0x59F44C: // check jump
				{
					g_net->send_reliable(PlayerPID_StanceAndMovement, PlayerStanceID_Jump);
					break;
				}
				default:
				{
					// if the engine is not setting the stance from a known return address
					// then we need to filter by stance id

					switch (id)
					{
					case 1:
					case 3:
					case 4:
					case 6:
					case 14:
					case 86:
					case 87:
					case 89: break; // ignore
					case 85:
					{
						g_net->send_reliable(PlayerPID_StanceAndMovement, PlayerStanceID_BodyStance, id);

						break;
					}
					default:
					{
						//log(GREEN, "[DBG] Localplayer body stance set to: {} from {:x}", id, ret_add);
					}
					}
				}
				}
			}
			else if (const auto player = g_net->get_player_by_character(character))
			{
				// if we are trying to set the stance from a top-hooked function such as dispatch_movement
				// then we want to make sure this player's stance can be changed from this local player,
				// this is useful because we want to ignore all stances that the game sets to remote players
				// like falling and stuff like that, which will be controlled by packets sent by remote players
				// containg such information (summary: to avoid desync)

				switch (id)
				{
				case 1:
				case 3:
				case 6:
				case 9:
				case 24:
				case 30:
				case 88: break; // allow these to be set to remote players by the engine itself
				default:
				{
					if (!player->is_dispatching_movement())
						return;
				}
				}

			}
		}

		set_body_stance_hook.call(stance, id);
	}

	DEFINE_HOOK_THISCALL(set_arms_stance, 0x744230, void, ArmsStanceController* stance, uint32_t id)
	{
		if (const auto local_char = g_world->get_localplayer_character())
		{
			const auto character = stance->get_character();

			if (character == local_char)
			{
				/*switch (const auto ret_add = ptr(_ReturnAddress()))
				{
				default:
				{
					switch (id)
					{
					}
				}
				}*/
			}
		}

		set_arms_stance_hook.call(stance, id);
	}

	DEFINE_HOOK_THISCALL_S(can_be_destroyed, 0x595F10, bool, Character* character)
	{
		// if our local player is being checked here, it means PlayerSettings is trying to
		// get us to the game continue menu, we will return false all the time so we can implement our
		// own spawing and also we avoid the stupid menu that serves no purpose, same for remote players
		// obviously...

		if (g_net->get_player_by_character(character))
			return false;

		// we implement our own logic of this function, we can modify the amount of time
		// a character can stay death before its destruction or we can just skip it in the future
		// and so on, this will vary in future development

		const auto stance_controller = character->get_body_stance();
		const auto movement_id = stance_controller->get_movement_id();

		return character->get_max_hp() <= std::numeric_limits<float>::max() &&
			(movement_id == 0x34 || movement_id == 0x62) &&
			character->get_death_time() > 10.f;
	}

	DEFINE_HOOK_THISCALL(dispatch_movement, 0x5A45D0, void, Character* character, float angle, float right, float forward, bool aiming)
	{
		if (const auto localplayer = g_net->get_localplayer())
		{
			if (const auto local_char = localplayer->get_character(); character == local_char)
			{
				const auto& move_info = localplayer->get_movement_info();

				const bool was_moving = move_info.right != 0.f || move_info.forward != 0.f || move_info.aiming;
				const bool is_moving = right != 0.f || forward != 0.f || aiming;
				const bool is_diff = right != move_info.right || forward != move_info.forward || aiming != move_info.aiming;

				const bool is_move_diff = is_diff || (was_moving && !is_moving || !was_moving && is_moving);
				const bool is_angle_diff = angle != move_info.angle;

				if (is_move_diff || is_angle_diff)
				{
					localplayer->set_movement_info(angle, right, forward, aiming);

					if (is_move_diff)
					{
						const auto packed_angle = util::pack::pack_pi_angle(angle);
						const auto packed_right = util::pack::pack_norm(right);
						const auto packed_forward = util::pack::pack_norm(forward);

						g_net->send_reliable(PlayerPID_StanceAndMovement, PlayerStanceID_Movement, packed_angle, packed_right, packed_forward, aiming);
					}
					else if (is_moving && is_angle_diff)
						localplayer->set_movement_angle(angle, true);
				}
			}
			else if (auto player = g_net->get_player_by_character(character))
				return;
		}

		dispatch_movement_hook.call(character, angle, right, forward, aiming);
	}

	DEFINE_HOOK_THISCALL_S(setup_punch, 0x5A4380, Character*, Character* character)
	{
		auto res = setup_punch_hook.call(character);

		if (const auto local_char = g_world->get_localplayer_character())
			if (character == local_char && res == character)
				g_net->send_reliable(PlayerPID_StanceAndMovement, PlayerStanceID_Punch);

		return res;
	}

	DEFINE_HOOK_THISCALL_S(update_mid, 0x591AD0, void, Character* character)
	{
		if (const auto localplayer = g_net->get_localplayer())
			if (const auto local_char = localplayer->get_character(); character == local_char)
			{
				const auto was_hip_aim = localplayer->is_hip_aiming(),
						   was_full_aim = localplayer->is_full_aiming(),
						   is_hip_aim = local_char->has_flag(CharacterFlag_HipAiming),
						   is_full_aim = local_char->has_flag(CharacterFlag_FullAiming);

				const auto aim_target = local_char->get_aim_target();

				if (was_hip_aim != is_hip_aim || was_full_aim != is_full_aim)
					g_net->send_reliable(PlayerPID_StanceAndMovement, PlayerStanceID_Aiming, is_hip_aim, is_full_aim, aim_target);

				localplayer->set_aim_info(is_hip_aim, is_full_aim, aim_target);
			}

		update_mid_hook.call(character);
	}

	DEFINE_HOOK_THISCALL(fire_weapon, 0x59FD20, bool, Character* character, bool a2)
	{
		const auto res = fire_weapon_hook.call(character, a2);

		if (res)
			if (const auto localplayer = g_net->get_localplayer())
				if (const auto local_char = localplayer->get_character(); character == local_char)
					if (auto weapon = local_char->get_weapon_belt()->get_current_weapon())
						g_net->send_reliable(
							PlayerPID_StanceAndMovement,
							PlayerStanceID_Fire,
							weapon->get_id(),
							weapon->get_grip_transform()->get_position(),
							weapon->get_aim_target());
		
		return res;
	}

	// hook the localplayer's key dispatch to reload weapon so we know if we should send
	// the reload msg or not
	//
	DEFINE_HOOK_THISCALL_S(reload_current_weapon, 0x5A0220, void, Character* character)
	{
		if (const auto lp = g_net->get_localplayer())
			if (const auto local_char = lp->get_character())
				if (const auto weapon_belt = local_char->get_weapon_belt())
					if (auto current_weapon = weapon_belt->get_current_weapon())
					{
						const auto was_reloading = current_weapon->is_reloading();

						reload_current_weapon_hook.call(character);

						const auto is_reloading = current_weapon->is_reloading();

						if (is_reloading && was_reloading != is_reloading)
							g_net->send_reliable(PlayerPID_StanceAndMovement, PlayerStanceID_Reload);

						return;
					}

		reload_current_weapon_hook.call(character);
	}

	DEFINE_HOOK_THISCALL(force_launch, 0x5A34A0, void, Character* character, vec3* dir, float f1, float f2)
	{
		if (const auto lp = g_net->get_localplayer())
			if (const auto local_char = lp->get_character())
			{
				//log(RED, "damn, {} {} {} {} {}", dir->x, dir->y, dir->z, f1, f2);

				if (local_char == character)
					g_net->send_reliable(PlayerPID_StanceAndMovement, PlayerStanceID_ForceLaunch, character->get_added_velocity(), *dir, f1, f2);
				else if (g_net->get_player_by_character(character))
					return;
			}

		force_launch_hook.call(character, dir, f1, f2);
	}

	DEFINE_HOOK_THISCALL(character_proxy_add_velocity, 0x744B20, void, hkCharacterProxy* proxy, const vec3* velocity, vec4* rotation)
	{
		// check which parts of the engine code we want to let modify a remote player's velocity

		switch (const auto ret = RET_ADDRESS)
		{
		case 0x5A3EAB: break;	// Character::CheckForceLaunch, modifying Y velocity for a better parabolic movement
		default:
		{
			//log(RED, "[{}] {}", CURR_FN, ret);

			// block the localplayer from modifying a remote player's velocity
			
			if (const auto lp = g_net->get_localplayer())
				if (const auto local_char = lp->get_character())
					if (const auto character = proxy->get_character())
						if (character != local_char && g_net->get_player_by_character(character))
							return;
		}
		}

		character_proxy_add_velocity_hook.call(proxy, velocity, rotation);
	}

	void apply()
	{
		update_hook.hook();
		set_body_stance_hook.hook();
		set_arms_stance_hook.hook();
		can_be_destroyed_hook.hook();
		dispatch_movement_hook.hook();
		setup_punch_hook.hook();
		update_mid_hook.hook();
		fire_weapon_hook.hook();
		reload_current_weapon_hook.hook();
		force_launch_hook.hook();
		character_proxy_add_velocity_hook.hook();
	}

	void undo()
	{
		character_proxy_add_velocity_hook.unhook();
		force_launch_hook.unhook();
		reload_current_weapon_hook.unhook();
		fire_weapon_hook.unhook();
		update_mid_hook.unhook();
		setup_punch_hook.unhook();
		dispatch_movement_hook.unhook();
		can_be_destroyed_hook.unhook();
		set_arms_stance_hook.unhook();
		set_body_stance_hook.unhook();
		update_hook.unhook();
	}
}

// statics

void Character::SET_GLOBAL_PUNCH_DAMAGE(float v, bool ai)
{
	if (ai)
		jc::write(v, jc::character::g::AI_PUNCH_DAMAGE);
	else jc::write(v, jc::character::g::PLAYER_PUNCH_DAMAGE);
}

void Character::SET_FLYING_Y_MODIFIER(float v)
{
	jc::write(v, jc::character::g::FLYING_Y_MODIFIER);
}

// character

void Character::rebuild_skeleton()
{
	const auto skeleton = get_skeleton();

	jc::this_call(jc::character::fn::DESTROY_SKELETON, skeleton);
	jc::this_call(jc::character::fn::CREATE_SKELETON, skeleton);
}

void Character::init_model()
{
	// make sure we patch/skip the weapon belt recreation, because, for some reason, they added
	// that inside this character info function

	scoped_patch<8> sp(jc::g::patch::AVOID_WEAPON_BELT_RECREATION_WHILE_CHAR_INIT.address, { 0xE9, 0x8C, 0x0, 0x0, 0x0, 0x90, 0x90, 0x90 });

	jc::v_call<ptr>(this, jc::alive_object::vt::INITIALIZE_MODELS, get_info());
}

void Character::respawn()
{
	remove_flag(1 << 31); // dbg

	jc::this_call(jc::character::fn::RESPAWN, this, 1.f);
}

void Character::set_proxy_velocity(const vec3& v)
{
	if (auto physical = get_proxy())
		if (auto p1 = jc::read<ptr>(physical, 0xB0))
			if (auto p2 = jc::read<ptr>(p1, 0x4))
				jc::write(vec4(v.x, v.y, v.z, 0.f), p2, 0x10);
}

void Character::set_added_velocity(const vec3& v)
{
	if (auto physical = get_proxy())
	{
		vec4 rotation = get_transform()[3];

		jc::character::hook::character_proxy_add_velocity_hook.call(physical, &v, &rotation);
	}
}

void Character::set_grenades_ammo(int32_t v)
{
	jc::write(v, this, jc::character::GRENADES_AMMO);
}

void Character::set_animation(const jc::stl::string& name, float speed, bool unk0, bool unk1)
{
	jc::this_call<bool>(jc::character::fn::SET_ANIMATION, this, &name, unk0, speed, unk1);
}

void Character::set_grenade_timeout(float v)
{
	jc::write(v, this, jc::character::GRENADE_TIMEOUT);
}

void Character::set_walking_anim_set(int32_t walking_anim_id, int32_t skin_id, bool sync)
{
	auto it = jc::vars::anim_sets.find(walking_anim_id);
	if (it == jc::vars::anim_sets.end())
		return;

	auto new_info = get_info();

	new_info->character_as = it->second;

	set_skin(skin_id, false);

	if (sync)
	{
		// sync the skin id with other players

		g_net->send_reliable(PlayerPID_DynamicInfo, PlayerDynInfo_WalkingSetAndSkin, walking_anim_id, skin_id);
	}
}

void Character::set_skin(int32_t id, bool sync)
{
	set_skin(id, -1, -1, -1, {}, sync);
}

void Character::set_skin(int32_t id, int32_t cloth_skin, int32_t head_skin, int32_t cloth_color, const std::vector<VariantPropInfo>& props, bool sync)
{
	g_rsrc_streamer->request_exported_entity(id, [=](ExportedEntityResource* eer)
	{
		if (object_base_map* map = nullptr; eer->get_exported_entity()->load_class_properties(map) && map)
		{
			// let's try to get the model name from the map
			// we will try to get the normal model first, if it fails
			// the only choice we have is to check for PD model which seems
			// to be some kind of model for special characters/agents

			jc::stl::string model;

			bool use_npc_variant = false;

			if (!(use_npc_variant = map->find_string(ObjectBase::Hash_Model, model)))
				if (!map->find_string(ObjectBase::Hash_PD_Model, model))
					return;

			// Avalanche bois forgot to recreate the skeleton when setting the character info
			// so we have to do it ourselves

			rebuild_skeleton();

			// get current info and set the new model

			auto new_info = get_info();

			new_info->model = model;
			
			// update the model etc

			init_model();

			// if we specified a new npc variant through the parameters then
			// let's use it instead of the default one

			const bool use_custom_npc_variant = (cloth_skin != -1 || head_skin != -1 || cloth_color != -1 || props.size() > 0);

			if (use_custom_npc_variant)
			{
				// npc variant through set_skin is never synced

				set_npc_variant(cloth_skin, head_skin, cloth_color, props, false);
			}
			else if (use_npc_variant)
			{
				// if we need to use npc variant then create one
				// initialize it using the agent type map
				// and set it to this character

				auto npc_variant = NPCVariant::CREATE();

				npc_variant->init_from_map(map);

				set_npc_variant(*npc_variant);
			}

			if (sync)
			{
				// sync the skin id with other players

				g_net->send_reliable(PlayerPID_DynamicInfo, PlayerDynInfo_Skin, id);
			}
		}
	});
}

void Character::set_npc_variant(NPCVariant* v)
{
	jc::this_call<bool>(jc::character::fn::SET_NPC_VARIANT, this, v);
}

void Character::set_npc_variant(int32_t cloth_skin, int32_t head_skin, int32_t cloth_color, const std::vector<VariantPropInfo>& props, bool sync)
{
	auto get_hashes = [](int index) -> std::tuple<uint32_t, uint32_t, uint32_t>
	{
		switch (index)
		{
		case 0: return { NPCVariant::Hash_Accessory1_Slot1, NPCVariant::Hash_Accessory1_Loc, NPCVariant::Hash_ForceAccessory1 };
		case 1: return { NPCVariant::Hash_Accessory2_Slot1, NPCVariant::Hash_Accessory2_Loc, NPCVariant::Hash_ForceAccessory2 };
		case 2: return { NPCVariant::Hash_Accessory3_Slot1, NPCVariant::Hash_Accessory3_Loc, NPCVariant::Hash_ForceAccessory3 };
		case 3: return { NPCVariant::Hash_Accessory4_Slot1, NPCVariant::Hash_Accessory4_Loc, NPCVariant::Hash_ForceAccessory4 };
		case 4: return { NPCVariant::Hash_Accessory5_Slot1, NPCVariant::Hash_Accessory5_Loc, NPCVariant::Hash_ForceAccessory5 };
		}

		return { 0u, 0u, 0u };
	};

	object_base_map map {};

	if (cloth_skin != -1)
		map.insert<object_base_map::String>(NPCVariant::Hash_ClothSkinSlot1, jc::vars::npc_variants::cloth_skins[cloth_skin]);

	if (head_skin != -1)
		map.insert<object_base_map::String>(NPCVariant::Hash_HeadSkinSlot1, jc::vars::npc_variants::head_skins[head_skin]);
	
	if (cloth_color != -1)
		map.insert<object_base_map::String>(NPCVariant::Hash_ClothColor, jc::vars::npc_variants::cloth_color[cloth_color]);

	for (int i = 0; const auto& prop : props)
	{
		const auto hashes = get_hashes(i++);

		if (prop.prop != -1)
		{
			map.insert<object_base_map::String>(std::get<0>(hashes), jc::vars::npc_variants::props[prop.prop]);
			map.insert<object_base_map::String>(std::get<1>(hashes), jc::vars::npc_variants::prop_locs[prop.loc]);
			map.insert<object_base_map::Int>(std::get<2>(hashes), 1);
		}
	}
	
	map.insert<object_base_map::Int>(0x937af6a, 1);

	auto npc_variant = NPCVariant::CREATE();

	npc_variant->init_from_map(&map);

	set_npc_variant(*npc_variant);

	if (sync)
	{
		// sync the variant info with other players

		g_net->send_reliable(PlayerPID_DynamicInfo, PlayerDynInfo_NPCVariant, cloth_skin, head_skin, cloth_color, props);
	}
}

void Character::set_flag(uint32_t mask)
{
	set_flags(get_flags() | mask);
}

void Character::set_flags(uint32_t mask)
{
	jc::write(mask, this, jc::character::FLAGS);
}

void Character::remove_flag(uint32_t mask)
{
	jc::write(get_flags() & ~mask, this, jc::character::FLAGS);
}

void Character::play_idle_stance()
{
	jc::write(-1.f, this, jc::character::IDLE_STANCE_TIME);
}

void Character::set_roll_clamp_enabled(bool v)
{
	jc::write(v ? 0.f : 1.f, this, jc::character::ROLL_CLAMP);
}

void Character::dispatch_movement(float angle, float right, float forward, bool aiming)
{
	jc::character::hook::dispatch_movement_hook.call(this, angle, right, forward, aiming);
}

void Character::set_body_stance(uint32_t id)
{
	jc::character::hook::set_body_stance_hook.call(get_body_stance(), id);
}

void Character::set_arms_stance(uint32_t id)
{
	jc::character::hook::set_arms_stance_hook.call(get_arms_stance(), id);
}

void Character::setup_punch()
{
	jc::character::hook::setup_punch_hook.call(this);
}

void Character::force_launch(const vec3& vel, const vec3& dir, float f1, float f2)
{
	set_added_velocity(vel);

	jc::character::hook::force_launch_hook.call(this, BITCAST(vec3*, &dir), f1, f2);
}

void Character::clear_weapon_belt()
{
	get_weapon_belt()->clear();
}

void Character::set_weapon(uint8_t id, bool is_remote_player)
{
	if (id == 0u)
		return save_current_weapon();

	if (const auto weapon_belt = get_weapon_belt())
	{
		const auto curr_weapon = weapon_belt->get_current_weapon();

		// if we have it and we are already holding it then 
		// do nothing

		if (curr_weapon->get_id() == id)
			return;

		const auto target_weapon_type = g_weapon->get_weapon_type(id);

		// simply draw the gun if we already have it but not
		// in hands

		for (int slot = 0; slot < WeaponBelt::MAX_SLOTS(); ++slot)
			if (auto rf = weapon_belt->get_weapon_from_slot(slot))
			{
				if (rf->get_id() == id)
				{
					set_draw_weapon(rf);

					return apply_weapon_switch();
				}
				else if (slot == weapon_belt->get_weapon_slot(target_weapon_type))
				{
					// clear our hands

					save_current_weapon();

					// remove the weapon before adding the new one to avoid weapon drop and bugs
					
					weapon_belt->remove_weapon(slot);

					break;
				}
			}

		// this character does not have the weapon we want to set so
		// we create and add it to the weapon belt

		auto rf = weapon_belt->add_weapon(id);

		// do a few adjustments to the weapon if it's owned by a remote player

		if (is_remote_player)
		{
			if (const auto weapon_info = rf->get_info())
			{
				weapon_info->set_infinite_ammo(true);
				weapon_info->set_accuracy(true, weapon_info->get_accuracy(false));
			}
		}

		// draw the weapon

		set_draw_weapon(rf);
		apply_weapon_switch();
	}
}

void Character::set_draw_weapon(int32_t slot)
{
	jc::this_call(jc::character::fn::SET_DRAW_WEAPON, this, slot);
}

void Character::set_draw_weapon(ref<Weapon>& weapon)
{
	if (const auto weapon_belt = get_weapon_belt())
	{
		const auto slot = weapon_belt->get_weapon_slot(*weapon);
		const auto is_local = this == g_world->get_localplayer_character();

		set_draw_weapon(slot);
	}
}

void Character::save_current_weapon()
{
	set_draw_weapon(1);
	apply_weapon_switch();
}

void Character::apply_weapon_switch()
{
	jc::this_call(jc::character::fn::DRAW_WEAPON_NOW, this);
}

void Character::set_aim_target(const vec3& v)
{
	jc::write(v, this, jc::character::AIM_TARGET);
}

void Character::fire_current_weapon(int32_t weapon_id, const vec3& muzzle, const vec3& aim_target)
{
	// let's make sure the weapon id we want to fire is the actual current weapon

	if (const auto weapon_belt = get_weapon_belt())
		if (auto weapon = weapon_belt->get_current_weapon())
			if (const auto weapon_info = weapon->get_info(); weapon_info && weapon_info->get_id() == weapon_id)
			{
				weapon->set_last_shot_time(jc::nums::MAXF);
				weapon->set_muzzle_position(muzzle);
				weapon->set_aim_target(aim_target);
				weapon->force_fire();
			}
}

void Character::reload_current_weapon()
{
	jc::this_call<bool>(jc::character::fn::RELOAD_CURRENT_WEAPON, this);
}

bool Character::has_flag(uint32_t mask) const
{
	return get_flags() & mask;
}

bool Character::is_on_ground() const
{
	return get_air_time() <= 0.f;
}

int32_t Character::get_grenades_ammo() const
{
	return jc::read<int32_t>(this, jc::character::GRENADES_AMMO);
}

uint32_t Character::get_flags() const
{
	return jc::read<uint32_t>(this, jc::character::FLAGS);
}

float Character::get_grenade_timeout() const
{
	return jc::read<float>(this, jc::character::GRENADE_TIMEOUT);
}

float Character::get_grenade_time() const
{
	return jc::read<float>(this, jc::character::GRENADE_TIME);
}

float Character::get_death_time() const
{
	return jc::read<float>(this, jc::character::DEATH_TIME);
}

float Character::get_roll_clamp() const
{
	return jc::read<float>(this, jc::character::ROLL_CLAMP);
}

float Character::get_air_time() const
{
	return jc::read<float>(this, jc::character::AIR_TIME);
}

CharacterHandleBase* Character::get_handle_base() const
{
	CharacterHandleBase* handle_base = nullptr;

	if (this == g_world->get_localplayer_character())
		handle_base = g_player_global_info->get_localplayer_handle_base();

	if (!handle_base)
		if (const auto handle_entry = REF(CharacterHandleEntry*, this, jc::character::HANDLE_ENTRY))
			if (const auto handle = g_ai->get_character_handle_from_entry(handle_entry))
				handle_base = handle->get_base();

	check(handle_base, "Invalid handle base");

	return handle_base;
}

Character* Character::get_facing_object() const
{
	return jc::this_call<Character*>(jc::character::fn::GET_FACING_OBJECT, this, nullptr);
}

WeaponBelt* Character::get_weapon_belt() const
{
	return jc::read<WeaponBelt*>(this, jc::character::WEAPON_BELT);
}

VehicleController* Character::get_vehicle_controller() const
{
	return jc::read<VehicleController*>(this, jc::character::VEHICLE_CONTROLLER);
}

Skeleton* Character::get_skeleton() const
{
	return REF(Skeleton*, this, jc::character::SKELETON);
}

CharacterInfo* Character::get_info() const
{
	return REF(CharacterInfo*, this, jc::character::INFO);
}

BodyStanceController* Character::get_body_stance() const
{
	return REF(BodyStanceController*, this, jc::character::BODY_STANCE_CONTROLLER);
}

ArmsStanceController* Character::get_arms_stance() const
{
	return REF(ArmsStanceController*, this, jc::character::ARMS_STANCE_CONTROLLER);
}

hkCharacterProxy* Character::get_proxy() const
{
	return jc::read<hkCharacterProxy*>(this, jc::character::PHYSICAL);
}

vec3 Character::get_velocity()
{
	return *REF(vec3*, this, jc::character::VELOCITY);
}

vec3 Character::get_added_velocity() const
{
	if (auto physical = get_proxy())
		return jc::read<vec3>(physical, 0x40);

	return {};
}

vec3 Character::get_aim_target() const
{
	return jc::read<vec3>(this, jc::character::AIM_TARGET);
}

vec3 Character::get_bone_position(BoneID index) const
{
	const auto skeleton = get_skeleton();

	if (!skeleton)
		return {};

	Transform bone_transform;

	if (!skeleton->get_bone_transform(index, bone_transform))
		return {};

	return get_transform().rotate_point(bone_transform.get_position());
}

vec3 Character::get_head_bone_position()
{
	vec3 out;

	return *jc::this_call<vec3*>(jc::character::fn::GET_HEAD_BONE_POSITION, this, &out);
}

vec3 Character::get_pelvis_bone_position()
{
	vec3 out;

	return *jc::this_call<vec3*>(jc::character::fn::GET_PELVIS_BONE_POSITION, this, &out);
}

vec3 Character::get_stomach_bone_position()
{
	vec3 out;

	return *jc::this_call<vec3*>(jc::character::fn::GET_STOMACH_BONE_POSITION, this, &out);
}

vec3 Character::get_random_bone_position()
{
	vec3 out;

	return *jc::this_call<vec3*>(jc::character::fn::GET_RANDOM_BONE_POSITION, this, &out);
}

Transform Character::get_head_bone_transform()
{
	Transform out;

	return *jc::this_call<Transform*>(jc::character::fn::GET_HEAD_BONE_TRANSFORM, this, &out);
}

Transform Character::get_pelvis_bone_transform()
{
	Transform out;

	return *jc::this_call<Transform*>(jc::character::fn::GET_PELVIS_BONE_TRANSFORM, this, &out);
}