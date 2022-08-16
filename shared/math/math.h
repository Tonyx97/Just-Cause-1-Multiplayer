#pragma once

#include <defs/glm.h>

using _128 = __m128;

// load & stores

#define sse_loadu(base, offset) _mm_loadu_ps((base) + (offset))
#define sse_storeu(value, base, offset) _mm_storeu_ps((base) + (offset), value)

// math functions

#define sse_add(v0, v1) _mm_add_ps((v0), (v1))
#define sse_mul(v0, v1) _mm_mul_ps((v0), (v1))
#define sse_mul_add(v0, v1, v2) _mm_add_ps(_mm_mul_ps((v0), (v1)), (v2))

// shuffling / permuting / splatting / merging

#define sse_shuffle(v0, i) _mm_shuffle_ps((v0), (v0), _MM_SHUFFLE(i, i, i, i))

#define RESTRICT(x) x* __restrict

using mat3 = glm::mat3;
using quat = glm::quat;
using vec4 = glm::vec4;
using vec3 = glm::vec3;
using vec2 = glm::vec2;
using vec1 = glm::vec1;
using ivec4 = glm::i32vec4;
using u8vec4 = glm::u8vec4;
using i16vec4 = glm::i16vec4;
using ivec3 = glm::i32vec3;
using i16vec3 = glm::i16vec3;
using u16vec3 = glm::u16vec3;
using ivec2 = glm::i32vec2;
using uvec2 = glm::u32vec2;
using ivec1 = glm::i32vec1;

/**
* Matrix 4x4 overload from GLM to use better SSE
* operations such as multiplication.
*/
struct mat4 : public glm::mat4
{
	using glm::mat4::mat4;

	mat4(const glm::mat4& v);

	vec4 operator * (const glm::vec4& v) const;

	mat4& operator = (const glm::mat4& v);
	mat4 operator * (const mat4& m) const;
};

namespace jc::prefetch
{
	inline void _read(const void* p) { _mm_prefetch((const char*)p, _MM_HINT_T0); }
	inline void _read_sized(const void* p, size_t size) { _read(p); }
	inline void _prefetch(const void* p) { _read(p); }
	inline void _prefetch(const void* p, size_t size) { _read_sized(p, size); }

	template <typename T, typename... A>
	inline void prefetch(const T* p, const A... args)
	{
		_prefetch(p);

		if constexpr (sizeof...(args) > 0)
			prefetch(args...);
	}
}

namespace jc::vec
{
	static constexpr auto UP = vec3(0.f, 1.f, 0.f);
	static constexpr auto DOWN = vec3(0.f, -1.f, 0.f);
	static constexpr auto RIGHT = vec3(1.f, 0.f, 0.f);
	static constexpr auto LEFT = vec3(-1.f, 0.f, 0.f);
	static constexpr auto FORWARD = vec3(0.f, 0.f, 1.f);
	static constexpr auto BACK = vec3(0.f, 0.f, -1.f);
	static constexpr auto ZERO = vec3(0.f);
	static constexpr auto IDENTITY = vec4(1.f, 0.f, 0.f, 0.f);
}

namespace jc::qua
{
	static constexpr auto IDENTITY = quat(1.f, 0.f, 0.f, 0.f);
}

namespace jc::nums
{
	static constexpr auto EPSILON = std::numeric_limits<float>::epsilon();
	static constexpr auto LOWF = std::numeric_limits<float>::lowest();
	static constexpr auto MINF = std::numeric_limits<float>::min();
	static constexpr auto MAXF = std::numeric_limits<float>::max();
	static constexpr auto MINI = std::numeric_limits<int32_t>::min();
	static constexpr auto MAXI = std::numeric_limits<int32_t>::max();
	static constexpr auto PI = std::numbers::pi_v<float>;
	static constexpr auto PI_DBL = std::numbers::pi_v<double>;
	static constexpr auto DOUBLE_PI = static_cast<float>(PI_DBL * 2.0);
	static constexpr auto HALF_PI = static_cast<float>(PI_DBL * 0.5);
	static constexpr auto QUARTER_PI = static_cast<float>(PI_DBL * 0.25);
	static constexpr auto ALMOST_DOUBLE_PI = DOUBLE_PI - EPSILON;
	static constexpr auto ALMOST_DOUBLE_PI_N = -DOUBLE_PI + EPSILON;
	static constexpr auto ALMOST_PI = PI - EPSILON;
	static constexpr auto ALMOST_HALF_PI = HALF_PI - EPSILON;
}

namespace jc::math
{
	inline i16vec4 pack_quat(const quat& q)
	{
		const auto _q = glm::normalize(q);

		return i16vec4(
			util::pack::pack_float(q.x, 32767.f),
			util::pack::pack_float(q.y, 32767.f),
			util::pack::pack_float(q.z, 32767.f),
			util::pack::pack_float(q.w, 32767.f));
	}

	inline quat unpack_quat(const i16vec4& q)
	{
		return quat(
			util::pack::unpack_float(q.w, 32767.f),
			util::pack::unpack_float(q.x, 32767.f),
			util::pack::unpack_float(q.y, 32767.f),
			util::pack::unpack_float(q.z, 32767.f));
	}

	inline i16vec3 pack_vec3(const vec3& q, float factor = 50.f)
	{
		const auto _factor = (static_cast<float>(1 << 15) - 1.f) / factor;

		return i16vec3(
			util::pack::pack_float(q.x, _factor),
			util::pack::pack_float(q.y, _factor),
			util::pack::pack_float(q.z, _factor));
	}

	inline vec3 unpack_vec3(const i16vec3& v, float factor = 50.f)
	{
		const auto _factor = (static_cast<float>(1 << 15) - 1.f) / factor;

		return vec3(
			util::pack::unpack_float(v.x, _factor),
			util::pack::unpack_float(v.y, _factor),
			util::pack::unpack_float(v.z, _factor));
	}

	inline vec3 lerp(const vec3& v, const vec3& t, float f)
	{
		return
		{
			std::lerp(v.x, t.x, f),
			std::lerp(v.y, t.y, f),
			std::lerp(v.z, t.z, f)
		};
	}

	inline float quat_diff(const quat& a, const quat& b)
	{
		return std::asinf(glm::length(glm::normalize(glm::conjugate(a) * b))) * 2.f;
	}

	inline float quat_cos_theta(const quat& a, const quat& b)
	{
		return glm::dot(glm::quat(a), glm::quat(b));
	}

	inline void mat4_mul_internal(
		const _128& m10, const _128& m11,
		const _128& m12, const _128& m13,
		const _128& m20, const _128& m21,
		const _128& m22, const _128& m23,
		_128& rm0, _128& rm1, _128& rm2, _128& rm3)
	{
		const _128 m20_X = sse_shuffle(m20, 0);
		const _128 m21_X = sse_shuffle(m21, 0);
		const _128 m22_X = sse_shuffle(m22, 0);
		const _128 m23_X = sse_shuffle(m23, 0);
		const _128 rm0_0 = sse_mul(m20_X, m10);
		const _128 rm1_0 = sse_mul(m21_X, m10);
		const _128 rm2_0 = sse_mul(m22_X, m10);
		const _128 rm3_0 = sse_mul(m23_X, m10);
		const _128 m20_Y = sse_shuffle(m20, 1);
		const _128 m21_Y = sse_shuffle(m21, 1);
		const _128 m22_Y = sse_shuffle(m22, 1);
		const _128 m23_Y = sse_shuffle(m23, 1);
		const _128 rm0_1 = sse_mul_add(m20_Y, m11, rm0_0);
		const _128 rm1_1 = sse_mul_add(m21_Y, m11, rm1_0);
		const _128 rm2_1 = sse_mul_add(m22_Y, m11, rm2_0);
		const _128 rm3_1 = sse_mul_add(m23_Y, m11, rm3_0);
		const _128 m20_Z = sse_shuffle(m20, 2);
		const _128 m21_Z = sse_shuffle(m21, 2);
		const _128 m22_Z = sse_shuffle(m22, 2);
		const _128 m23_Z = sse_shuffle(m23, 2);
		const _128 rm0_2 = sse_mul_add(m20_Z, m12, rm0_1);
		const _128 rm1_2 = sse_mul_add(m21_Z, m12, rm1_1);
		const _128 rm2_2 = sse_mul_add(m22_Z, m12, rm2_1);
		const _128 rm3_2 = sse_mul_add(m23_Z, m12, rm3_1);
		const _128 m20_W = sse_shuffle(m20, 3);
		const _128 m21_W = sse_shuffle(m21, 3);
		const _128 m22_W = sse_shuffle(m22, 3);
		const _128 m23_W = sse_shuffle(m23, 3);

		rm0 = sse_mul_add(m20_W, m13, rm0_2);
		rm1 = sse_mul_add(m21_W, m13, rm1_2);
		rm2 = sse_mul_add(m22_W, m13, rm2_2);
		rm3 = sse_mul_add(m23_W, m13, rm3_2);
	}

	template <typename T>
	inline void mat4_copy(RESTRICT(const T) lhs, RESTRICT(mat4) res)
	{
		static_assert(std::is_same_v<T, glm::mat4> || std::is_same_v<T, mat4>, "Invalid instance type");

		auto m1 = (float*)lhs,
			m2 = (float*)res;

		sse_storeu(sse_loadu(m1, 0x0), m2, 0x0);
		sse_storeu(sse_loadu(m1, 0x4), m2, 0x4);
		sse_storeu(sse_loadu(m1, 0x8), m2, 0x8);
		sse_storeu(sse_loadu(m1, 0xC), m2, 0xC);
	}

	inline void mat4_mul(RESTRICT(const mat4) lhs, RESTRICT(const mat4) rhs, RESTRICT(mat4) res)
	{
		auto m1 = (float*)lhs,
			 m2 = (float*)rhs;

		jc::prefetch::prefetch(m1, m2);

		_128 rm0, rm1, rm2, rm3;

		mat4_mul_internal(
			sse_loadu(m1, 0x0),
			sse_loadu(m1, 0x4),
			sse_loadu(m1, 0x8),
			sse_loadu(m1, 0xC),
			sse_loadu(m2, 0x0),
			sse_loadu(m2, 0x4),
			sse_loadu(m2, 0x8),
			sse_loadu(m2, 0xC),
			rm0, rm1, rm2, rm3);

		float* m = (float*)res;

		sse_storeu(rm0, m, 0x0);
		sse_storeu(rm1, m, 0x4);
		sse_storeu(rm2, m, 0x8);
		sse_storeu(rm3, m, 0xC);
	}

	inline void mat4_vec4_mul(RESTRICT(const mat4) lhs, RESTRICT(const vec4) rhs, RESTRICT(vec4) res)
	{
		auto m = (float*)lhs,
			 v = (float*)rhs;

		jc::prefetch::prefetch(m, v);

		auto vl = sse_loadu(v, 0);

		auto m0 = sse_loadu(m, 0),
			 m1 = sse_loadu(m, 4),
			 m2 = sse_loadu(m, 8),
			 m3 = sse_loadu(m, 12);

		auto result = sse_mul(sse_shuffle(vl, 0), m0);

		result = sse_mul_add(sse_shuffle(vl, 1), m1, result);
		result = sse_mul_add(sse_shuffle(vl, 2), m2, result);
		result = sse_mul_add(sse_shuffle(vl, 3), m3, result);

		sse_storeu(result, (float*)res, 0x0);
	}
}

using namespace jc::math;

inline mat4::mat4(const glm::mat4& v)
{
	*this = v;
}

inline vec4 mat4::operator * (const vec4& v) const
{
	vec4 out;

	jc::math::mat4_vec4_mul(this, &v, &out);

	return out;
}

inline mat4& mat4::operator=(const glm::mat4& v)
{
	jc::math::mat4_copy(&v, this);

	return *this;
}

inline mat4 mat4::operator * (const mat4& m) const
{
	auto out = *this;

	jc::math::mat4_mul(this, &m, &out);

	return out;
}