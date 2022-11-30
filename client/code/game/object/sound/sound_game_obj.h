#pragma once

#include "../base/base.h"

namespace jc::sound_game_obj
{
	static constexpr uint32_t SOB_NAME = 0x1C;
	static constexpr uint32_t SAB_NAME = 0x38;
	static constexpr uint32_t MAX_DISTANCE = 0x5C;
	static constexpr uint32_t TRANSFORM = 0x60;

	namespace vt
	{
	}
}

class SoundGameObject : public ObjectBase
{
public:

	enum Hash : uint32_t
	{
		Hash_SAB = 0x425d6659,
		Hash_SOB = 0xeb9f5c85,
		Hash_SoundID = 0x402e727a,
		Hash_MaxDistance = 0x695d2cf2,
		Hash_Event0 = 0xe9b2e8b7,
		Hash_Event1 = 0xe6d85c43,
		Hash_Event2 = 0xb22eaffd,
		Hash_Event3 = 0x60470680,
		Hash_Event4 = 0xbebbdce3,
		Hash_Event5 = 0xe022d451,
		Hash_Event6 = 0x5a489a2d,
	};

	IMPL_OBJECT_TYPE_ID("CSoundGameObject");

	bool setup(const vec3& position, const std::string& bank_name, uint32_t sound_id);
};