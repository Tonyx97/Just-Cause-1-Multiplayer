#include <defs/standard.h>

#include "skeleton.h"

#include <game/object/transform/transform.h>

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

BoneList* Skeleton::get_bone_list()
{
	return jc::read<BoneList*>(this, jc::skeleton::BONE_LIST);
}