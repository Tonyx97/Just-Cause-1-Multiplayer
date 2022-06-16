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

	inline float int16_to_float(int16_t v)
	{
		const auto i = read<int>(0xA1EAA0, 3 * v) << 8;
		return *BITCAST(float*, &i);
	}
}