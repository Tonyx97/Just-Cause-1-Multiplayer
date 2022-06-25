#include <defs/standard.h>

#include "skeleton.h"

#include <game/transform/transform.h>

void* SkeletonInstance::get_animator() const
{
	return jc::read<void*>(this, jc::skeleton_instance::ANIMATOR);
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

SkeletonInstance* Skeleton::get_skeleton_0() const
{
    return jc::read<SkeletonInstance*>(this, jc::skeleton::SKELETON_0);
}

SkeletonInstance* Skeleton::get_skeleton_1() const
{
	return jc::read<SkeletonInstance*>(this, jc::skeleton::SKELETON_1);
}

BoneList* Skeleton::get_bone_list()
{
	return jc::read<BoneList*>(this, jc::skeleton::BONE_LIST);
}
