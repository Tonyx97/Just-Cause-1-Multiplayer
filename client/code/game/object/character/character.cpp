#include <defs/standard.h>

#include "character.h"

#include "../weapon/weapon_belt.h"

#include <game/transform/transform.h>
#include <game/sys/world.h>
#include <game/sys/physics.h>
#include <game/sys/resource_streamer.h>

#include <havok/character_proxy.h>
#include <havok/motion_state.h>
#include <havok/simple_shape_phantom.h>

#include <mp/net.h>

#include "../exported_entity/exported_entity.h"
#include "../resource/ee_resource.h"
#include "../agent_type/npc_variant.h"

// hooks

#include <core/test_units.h>
#include "comps/stance_controller.h"

namespace jc::character::hook
{
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
		if (const auto local_char = g_world->get_localplayer_character())
		{
			if (character == local_char)
			{
				//log(RED, "{} {} {} {}", angle, right, forward, aiming);

				if (g_test_char)
					dispatch_movement_hook.call(g_test_char, angle, right, forward, aiming);

				g_net->send_reliable(PlayerPID_StanceAndMovement, 0u, angle, right, forward, aiming);
			}
			else if (g_net->get_player_by_character(character))
				return;
		}

		dispatch_movement_hook.call(character, angle, right, forward, aiming);
	}

	DEFINE_HOOK_THISCALL(set_body_stance, 0x625750, void, BodyStanceController* stance, uint32_t id)
	{
		if (const auto local_char = g_world->get_localplayer_character())
		{
			const auto character = stance->get_character();

			if (character == local_char)
			{
				switch (const auto ret_add = ptr(_ReturnAddress()))
				{
				case 0x59F44C: // check jump
				{
					g_net->send_reliable(PlayerPID_StanceAndMovement, 1u);
					break;
				}
				default:
				{
					// if the engine is not setting the stance from a known return address
					// then we need to filter by stance id

					switch (id)
					{
					case 1:
					case 9: break; // ignore
					case 85:
					case 86:
					case 88:
					case 89:
					{
						g_net->send_reliable(PlayerPID_StanceAndMovement, 3u, id);

						break;
					}
					default:
					{
						//log(GREEN, "[DBG] Localplayer stance set to: {} from {:x}", id, ret_add);
					}
					}
				}
				}
			}
			else if (const auto player = g_net->get_player_by_character(character))
			{
				// if we are trying to set the stance from a top-hooked function such as dispatch_movement_t
				// then we want to make sure this player's stance can be changed from this local player,
				// this is useful because we want to ignore all stances that the game sets to remote players
				// like falling and stuff like that, which will be controlled by packets sent by remote players
				// containg such information (summary: to avoid desync)
				
				if (player->must_skip_engine_stances())
					return;
			}
		}

		set_body_stance_hook.call(stance, id);
	}

	DEFINE_HOOK_THISCALL_S(setup_punch, 0x5A4380, Character*, Character* character)
	{
		auto res = setup_punch_hook.call(character);

		if (const auto local_char = g_world->get_localplayer_character())
			if (character == local_char && res == character)
				g_net->send_reliable(PlayerPID_StanceAndMovement, 2u);

		return res;
	}

	void apply()
	{
		can_be_destroyed_hook.hook();
		dispatch_movement_hook.hook();
		set_body_stance_hook.hook();
		setup_punch_hook.hook();
	}

	void undo()
	{
		setup_punch_hook.unhook();
		set_body_stance_hook.unhook();
		dispatch_movement_hook.unhook();
		can_be_destroyed_hook.unhook();
	}
}

// statics

void Character::SET_GLOBAL_PUNCH_DAMAGE(float v, bool ai)
{
	if (ai)
		jc::write(v, jc::character::g::AI_PUNCH_DAMAGE);
	else jc::write(v, jc::character::g::PLAYER_PUNCH_DAMAGE);
}

// character

void Character::rebuild_skeleton()
{
	const auto skeleton = get_skeleton();

	jc::this_call<ptr>(jc::character::fn::DESTROY_SKELETON, skeleton);
	jc::this_call<ptr>(jc::character::fn::CREATE_SKELETON, skeleton);
}

void Character::respawn()
{
	jc::this_call<ptr>(jc::character::fn::RESPAWN, this, 1.f);
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

void Character::set_model(uint32_t id, bool sync)
{
	g_rsrc_streamer->request_exported_entity(id, [&](ExportedEntityResource* eer)
	{
		if (object_base_map* map = nullptr; eer->get_exported_entity()->load_class_properties(map) && map)
		{
			// Avalanche bois forgot to recreate the skeleton when setting the character info
			// so we have to do it ourselves

			rebuild_skeleton();

			// let's try to get the model name from the map
			// we will try to get the normal model first, if it fails
			// the only choice we have is to check for PD model which seems
			// to be some kind of model for special characters/agents

			jc::stl::string model;

			bool use_npc_variant = false;

			if (!(use_npc_variant = map->find_string(ObjectBase::Hash_Model, model)))
				if (!map->find_string(ObjectBase::Hash_PD_Model, model))
					return;

			// get current info and set the new model

			auto new_info = get_info();

			new_info->model = model;

			// make sure we patch/skip the weapon belt recreation, because, for some reason, they added
			// that inside this character info function

			scoped_patch<8> sp(jc::g::patch::AVOID_WEAPON_BELT_RECREATION_WHILE_CHAR_INIT.address, { 0xE9, 0x8C, 0x0, 0x0, 0x0, 0x90, 0x90, 0x90 });

			jc::v_call<ptr>(this, jc::alive_object::vt::INITIALIZE_MODELS, new_info);

			// if we need to use npc variant then create one
			// initialize it using the agent type map
			// and set it to this character

			if (use_npc_variant)
			{
				auto npc_variant = NPCVariant::CREATE();

				npc_variant->init_from_map(map);

				set_npc_variant(*npc_variant);
			}

			if (sync)
			{
				// sync the skin id with other players

				g_net->send_reliable(PlayerPID_DynamicInfo, 3u, id);
			}
		}
	});
}

void Character::set_npc_variant(NPCVariant* v)
{
	jc::this_call<bool>(jc::character::fn::SET_NPC_VARIANT, this, v);
}

void Character::set_flag(uint32_t mask)
{
	jc::write(get_flags() & mask, this, jc::character::FLAGS);
}

void Character::remove_flag(uint32_t mask)
{
	jc::write(get_flags() & ~mask, this, jc::character::FLAGS);
}

void Character::set_roll_clamp_enabled(bool v)
{
	jc::write(v ? 0.f : 1.f, this, jc::character::ROLL_CLAMP);
}

void Character::dispatch_movement(float angle, float right, float forward, bool aiming)
{
	//jc::character::hook::dispatch_movement_hook.call(this, angle, right, forward, aiming);
}

void Character::set_body_stance(uint32_t id)
{
	//jc::character::hook::set_body_stance_hook.call(get_body_stance(), id);
}

void Character::setup_punch()
{
	//jc::character::hook::setup_punch_hook.call(this);
}

bool Character::has_flag(uint32_t mask) const
{
	return get_flags() & mask;
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

vec3 Character::get_velocity()
{
	return *REF(vec3*, this, jc::character::VELOCITY);
}

vec3 Character::get_bone_position(BoneID index) const
{
	const auto skeleton = get_skeleton();

	if (!skeleton)
		return {};

	Transform bone_transform;

	if (!skeleton->get_bone_transform(index, bone_transform))
		return {};

	return get_transform().rotate_point(bone_transform.position());
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