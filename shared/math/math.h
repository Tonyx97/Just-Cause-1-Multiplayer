#pragma once

#include <defs/glm.h>

using _128 = __m128;

// load & stores

#define V4LoadUnaligned(base, offset) _mm_loadu_ps((base) + (offset))
#define V4StoreUnaligned(value, base, offset) _mm_storeu_ps((base) + (offset), value)

// math functions

#define V4Add(v0, v1) _mm_add_ps((v0), (v1))
#define V4Mul(v0, v1) _mm_mul_ps((v0), (v1))
#define V4MulAdd(v0, v1, v2) _mm_add_ps(_mm_mul_ps((v0), (v1)), (v2))

// shuffling / permuting / splatting / merging

#define V4Splat(v0, i) _mm_shuffle_ps((v0), (v0), _MM_SHUFFLE(i, i, i, i))

#define RESTRICT(x) x* __restrict

using mat3 = glm::mat3;
using quat = glm::quat;
using vec4 = glm::vec4;
using vec3 = glm::vec3;
using vec2 = glm::vec2;
using vec1 = glm::vec1;
using ivec4 = glm::i32vec4;
using ivec3 = glm::i32vec3;
using ivec2 = glm::i32vec2;
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

namespace tvg::prefetch
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
};

namespace jc::math
{
	inline void mat4_mul_internal(
		const _128& m10, const _128& m11,
		const _128& m12, const _128& m13,
		const _128& m20, const _128& m21,
		const _128& m22, const _128& m23,
		_128& rm0, _128& rm1, _128& rm2, _128& rm3)
	{
		const _128 m20_X = V4Splat(m20, 0);
		const _128 m21_X = V4Splat(m21, 0);
		const _128 m22_X = V4Splat(m22, 0);
		const _128 m23_X = V4Splat(m23, 0);
		const _128 rm0_0 = V4Mul(m20_X, m10);
		const _128 rm1_0 = V4Mul(m21_X, m10);
		const _128 rm2_0 = V4Mul(m22_X, m10);
		const _128 rm3_0 = V4Mul(m23_X, m10);
		const _128 m20_Y = V4Splat(m20, 1);
		const _128 m21_Y = V4Splat(m21, 1);
		const _128 m22_Y = V4Splat(m22, 1);
		const _128 m23_Y = V4Splat(m23, 1);
		const _128 rm0_1 = V4MulAdd(m20_Y, m11, rm0_0);
		const _128 rm1_1 = V4MulAdd(m21_Y, m11, rm1_0);
		const _128 rm2_1 = V4MulAdd(m22_Y, m11, rm2_0);
		const _128 rm3_1 = V4MulAdd(m23_Y, m11, rm3_0);
		const _128 m20_Z = V4Splat(m20, 2);
		const _128 m21_Z = V4Splat(m21, 2);
		const _128 m22_Z = V4Splat(m22, 2);
		const _128 m23_Z = V4Splat(m23, 2);
		const _128 rm0_2 = V4MulAdd(m20_Z, m12, rm0_1);
		const _128 rm1_2 = V4MulAdd(m21_Z, m12, rm1_1);
		const _128 rm2_2 = V4MulAdd(m22_Z, m12, rm2_1);
		const _128 rm3_2 = V4MulAdd(m23_Z, m12, rm3_1);
		const _128 m20_W = V4Splat(m20, 3);
		const _128 m21_W = V4Splat(m21, 3);
		const _128 m22_W = V4Splat(m22, 3);
		const _128 m23_W = V4Splat(m23, 3);

		rm0 = V4MulAdd(m20_W, m13, rm0_2);
		rm1 = V4MulAdd(m21_W, m13, rm1_2);
		rm2 = V4MulAdd(m22_W, m13, rm2_2);
		rm3 = V4MulAdd(m23_W, m13, rm3_2);
	}

	template <typename T>
	inline void mat4_copy(RESTRICT(const T) lhs, RESTRICT(mat4) res)
	{
		static_assert(std::is_same_v<T, glm::mat4> || std::is_same_v<T, mat4>, "Invalid instance type");

		auto m1 = (float*)lhs,
			m2 = (float*)res;

		V4StoreUnaligned(V4LoadUnaligned(m1, 0x0), m2, 0x0);
		V4StoreUnaligned(V4LoadUnaligned(m1, 0x4), m2, 0x4);
		V4StoreUnaligned(V4LoadUnaligned(m1, 0x8), m2, 0x8);
		V4StoreUnaligned(V4LoadUnaligned(m1, 0xC), m2, 0xC);
	}

	inline void mat4_mul(RESTRICT(const mat4) lhs, RESTRICT(const mat4) rhs, RESTRICT(mat4) res)
	{
		auto m1 = (float*)lhs,
			 m2 = (float*)rhs;

		tvg::prefetch::prefetch(m1, m2);

		_128 rm0, rm1, rm2, rm3;

		mat4_mul_internal(
			V4LoadUnaligned(m1, 0x0),
			V4LoadUnaligned(m1, 0x4),
			V4LoadUnaligned(m1, 0x8),
			V4LoadUnaligned(m1, 0xC),
			V4LoadUnaligned(m2, 0x0),
			V4LoadUnaligned(m2, 0x4),
			V4LoadUnaligned(m2, 0x8),
			V4LoadUnaligned(m2, 0xC),
			rm0, rm1, rm2, rm3);

		float* m = (float*)res;

		V4StoreUnaligned(rm0, m, 0x0);
		V4StoreUnaligned(rm1, m, 0x4);
		V4StoreUnaligned(rm2, m, 0x8);
		V4StoreUnaligned(rm3, m, 0xC);
	}

	inline void mat4_vec4_mul(RESTRICT(const mat4) lhs, RESTRICT(const vec4) rhs, RESTRICT(vec4) res)
	{
		auto m = (float*)lhs,
			 v = (float*)rhs;

		tvg::prefetch::prefetch(m, v);

		auto vl = V4LoadUnaligned(v, 0);

		auto m0 = V4LoadUnaligned(m, 0),
			 m1 = V4LoadUnaligned(m, 4),
			 m2 = V4LoadUnaligned(m, 8),
			 m3 = V4LoadUnaligned(m, 12);

		auto result = V4Mul(V4Splat(vl, 0), m0);

		result = V4MulAdd(V4Splat(vl, 1), m1, result);
		result = V4MulAdd(V4Splat(vl, 2), m2, result);
		result = V4MulAdd(V4Splat(vl, 3), m3, result);

		V4StoreUnaligned(result, (float*)res, 0x0);
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