#pragma once

namespace jc::skeleton
{
	static constexpr uint32_t BONE_LIST					= 0x260; // Bone*
	static constexpr uint32_t TRANSFORM_LIST			= 0x294; // Transform*
	static constexpr uint32_t SKELETON_0				= 0x2AC; // SkeletonInstance*
	static constexpr uint32_t SKELETON_1				= 0x2B4; // SkeletonInstance*
	static constexpr uint32_t HEAD_EULER_ROTATION		= 0x3D8; // vec3
	static constexpr uint32_t HEAD_INTERPOLATION		= 0x3E4; // float
	static constexpr uint32_t HEAD_INTERPOLATION_FACTOR	= 0x3E8; // float
	static constexpr uint32_t HEAD_ROTATION				= 0x3F0; // quat
}

namespace jc::skeleton_instance
{
	static constexpr uint32_t ANIMATOR = 0x38; // void*
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

class SkeletonInstance
{
public:

	void* get_animator() const;
};

class Skeleton
{
private:
	BoneList* get_bone_list();

public:

	void set_head_interpolation(float v);
	void set_head_euler_rotation(const vec3& v);

	bool get_bone_transform(BoneID index, Transform& out);

	float get_head_interpolation() const;
	float get_head_interpolation_factor() const;

	Character* get_character() const;

	SkeletonInstance* get_skeleton_0() const;
	SkeletonInstance* get_skeleton_1() const;

	vec3 get_head_euler_rotation() const;

	quat get_head_rotation() const;
};