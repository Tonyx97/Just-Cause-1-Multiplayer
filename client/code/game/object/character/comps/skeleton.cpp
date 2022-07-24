#include <defs/standard.h>

#include "skeleton.h"

#include <game/transform/transform.h>

#include "../character.h"

void* SkeletonInstance::get_animator() const
{
	return jc::read<void*>(this, jc::skeleton_instance::ANIMATOR);
}

void Skeleton::set_head_interpolation(float v)
{
	jc::write(v, this, jc::skeleton::HEAD_INTERPOLATION);
}

void Skeleton::set_head_euler_rotation(const vec3& v)
{
	jc::write(v, this, jc::skeleton::HEAD_EULER_ROTATION);
}

bool Skeleton::get_bone_transform(const BoneID index, Transform& out)
{
	if (static_cast<uint8_t>(index) < 0u)
		return false;

	const auto bone_list = get_bone_list();

	if (!bone_list)
		return false;

	if (static_cast<size_t>(index) >= bone_list->get_bone_count())
		return false;

	out = jc::read<Transform*>(this, jc::skeleton::TRANSFORM_LIST)[static_cast<uint8_t>(index)];

	return true;
}

float Skeleton::get_head_interpolation() const
{
	return jc::read<float>(this, jc::skeleton::HEAD_INTERPOLATION);
}

float Skeleton::get_head_interpolation_factor() const
{
	return jc::read<float>(this, jc::skeleton::HEAD_INTERPOLATION_FACTOR);
}

Character* Skeleton::get_character() const
{
	return REFB(Character*, this, jc::character::SKELETON);
}

SkeletonInstance* Skeleton::get_skeleton_0() const
{
    return jc::read<SkeletonInstance*>(this, jc::skeleton::SKELETON_0);
}

SkeletonInstance* Skeleton::get_skeleton_1() const
{
	return jc::read<SkeletonInstance*>(this, jc::skeleton::SKELETON_1);
}

vec3 Skeleton::get_head_euler_rotation() const
{
	return jc::read<vec3>(this, jc::skeleton::HEAD_EULER_ROTATION);
}

quat Skeleton::get_head_rotation() const
{
	return jc::read<quat>(this, jc::skeleton::HEAD_ROTATION);
}

BoneList* Skeleton::get_bone_list()
{
	return jc::read<BoneList*>(this, jc::skeleton::BONE_LIST);
}