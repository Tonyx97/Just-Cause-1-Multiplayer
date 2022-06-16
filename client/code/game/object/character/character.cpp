#include <defs/standard.h>

#include "character.h"

#include "../transform/transform.h"
#include "../weapon/weapon_belt.h"

#include <game/sys/physics.h>

#include <havok/character_proxy.h>
#include <havok/motion_state.h>
#include <havok/simple_shape_phantom.h>

void Character::SET_GLOBAL_PUNCH_DAMAGE(float v, bool ai)
{
	if (ai)
		jc::write(v, jc::character::g::AI_PUNCH_DAMAGE);
	else jc::write(v, jc::character::g::PLAYER_PUNCH_DAMAGE);
}

void Character::rebuild_skeleton()
{
	const auto skeleton = get_skeleton();

	jc::this_call<ptr>(jc::character::fn::DESTROY_SKELETON, skeleton);
	jc::this_call<ptr>(jc::character::fn::CREATE_SKELETON, skeleton);
}

void Character::set_grenades_ammo(int32_t v)
{
	jc::write(v, this, jc::character::GRENADES_AMMO);
}

void Character::set_animation(const std::string& name, float speed, bool unk0, bool unk1)
{
	jc::this_call<bool>(jc::character::fn::SET_ANIMATION, this, &name, unk0, speed, unk1);
}

void Character::set_grenade_timeout(float v)
{
	jc::write(v, this, jc::character::GRENADE_TIMEOUT);
}

void Character::set_model(const std::string& name)
{
	// Avalanche bois forgot to recreate the skeleton when setting the character info
	// so we have to do it ourselves

	rebuild_skeleton();

	// get current info and set the new model

	auto new_info = *get_info();

	// available models for characters:
	// female1, female2, male1, male2, male3, male4 and male5

	new_info.model = "Characters\\KC_022\\KC_022.lod";
	// new_info.model = "characters\\paperdolls\\paperdoll_" + name + ".lod";
	new_info.character_as = "settings\\animationsets\\female_characterai.as";

	// make sure we patch the weapon belt recreation, because, for some reason, they added
	// that inside this character info function

	scoped_patch<8> sp(jc::g::patch::AVOID_WEAPON_BELT_RECREATION_WHILE_CHAR_INIT.address, { 0xE9, 0x8C, 0x0, 0x0, 0x0, 0x90, 0x90, 0x90 });

	jc::v_call<ptr>(this, jc::alive_object::vt::INITIALIZE_MODELS, &new_info);
}

void Character::set_flag(uint32_t mask)
{
	jc::write(get_flags() & mask, this, jc::character::FLAGS);
}

void Character::remove_flag(uint32_t mask)
{
	jc::write(get_flags() & ~mask, this, jc::character::FLAGS);
}

bool Character::has_flag(uint32_t mask)
{
	return get_flags() & mask;
}

int32_t Character::get_grenades_ammo()
{
	return jc::read<int32_t>(this, jc::character::GRENADES_AMMO);
}

uint32_t Character::get_flags()
{
	return jc::read<uint32_t>(this, jc::character::FLAGS);
}

float Character::get_grenade_timeout()
{
	return jc::read<float>(this, jc::character::GRENADE_TIMEOUT);
}

float Character::get_grenade_time()
{
	return jc::read<float>(this, jc::character::GRENADE_TIME);
}

float Character::get_death_time()
{
	return jc::read<float>(this, jc::character::DEATH_TIME);
}

WeaponBelt* Character::get_weapon_belt() const
{
	return jc::read<WeaponBelt*>(this, jc::character::WEAPON_BELT);
}

VehicleController* Character::get_vehicle_controller()
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

StanceController* Character::get_stance_controller() const
{
	return REF(StanceController*, this, jc::character::STANCE_CONTROLLER);
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

	return get_transform().rotate_point(*bone_transform.position());
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