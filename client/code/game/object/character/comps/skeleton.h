#pragma once

namespace jc::skeleton
{
	static constexpr uint32_t BONE_LIST		 = 0x260; // Bone*
	static constexpr uint32_t TRANSFORM_LIST = 0x294; // Transform*
}

class Transform;

enum class BoneID : uint8_t
{
	Root		 = 0,
	Head		 = 11,
	Neck		 = 10,
	ClavicleL	 = 12,
	ClavicleR	 = 17,
	ShoulderArmL = 13,
	ShoulderArmR = 18,
	ElbowL		 = 14,
	ElbowR		 = 19,
	HandL		 = 15,
	HandR		 = 20,
	FingersL	 = 16,
	FingersR	 = 21,
	Stomach		 = 9,
	Waist		 = 8,
	Pelvis		 = 1,
	LegL		 = 2,
	LegR		 = 5,
	KneeL		 = 3,
	KneeR		 = 6,
	FootL		 = 4,
	FootR		 = 7
};

class Bone
{
public:
	const char unknown[0x9C];
};

class BoneList
{
public:
	jc::stl::vector<Bone*> bones;

public:
	[[nodiscard]] size_t get_bone_count() const { return bones.size(); }
};

class Skeleton
{
private:
	BoneList* get_bone_list();

public:
	bool get_bone_transform(BoneID index, Transform& out);
};