#pragma once

namespace jc::g
{
	namespace memory
	{
		namespace fn
		{
			static constexpr uint32_t MALLOC = 0x4058E0;
		}
	}

	namespace math
	{
		namespace fn
		{
			static constexpr uint32_t ROTATE_POINT_BY_MATRIX = 0x412E00;
			static constexpr uint32_t FLOAT_TO_INT16 = 0x45C650;
		}
	}

	namespace patch
	{
		inline patch_value AVOID_WEAPON_BELT_RECREATION_WHILE_CHAR_INIT(0x58C515u, 8u);
		inline patch_value CHAR_LOOK_MIN_DISTANCE(0x730243u, 4u);
	}

	static constexpr uint32_t FOCUS_FREEZE_TIME = 0xAEBE98; // int (default 10)
}

namespace jc::game
{
	void* malloc_internal(int size);

	template <typename T = void>
	inline T* malloc(int size)
	{
		return BITCAST(T*, malloc_internal(size));
	}

	inline float decode_float(uint16_t v)
	{
		const auto i = read<int>(0xA1EAA0, 3 * v) << 8;
		return *BITCAST(float*, &i);
	}

	inline uint16_t encode_float(float v)
	{
		uint16_t out;
		return *jc::this_call<uint16_t*>(jc::g::math::fn::FLOAT_TO_INT16, &out, v);
	}

	inline u16vec3 to_u16vec3(const vec3& v)
	{
		return u16vec3(jc::game::encode_float(v.x), jc::game::encode_float(v.y), jc::game::encode_float(v.z));
	}

	inline vec3 to_vec3(const u16vec3& v)
	{
		return vec3(jc::game::decode_float(v.x), jc::game::decode_float(v.y), jc::game::decode_float(v.z));
	}
}