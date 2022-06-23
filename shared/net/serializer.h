#pragma once

#ifndef JC_LOADER

#include <enet.h>

#include <shared_mp/objs/all.h>

namespace enet
{
	// puts data into the buffer
	//
	inline void serialize_general_data(vec<uint8_t>& buffer, const void* data, size_t size)
	{
		auto ptr = BITCAST(uint8_t*, data);

		buffer.insert(buffer.end(), ptr, ptr + size);
	}

	// puts string data into the buffer
	//
	inline void serialize_string(vec<uint8_t>& buffer, const void* data, size_t size)
	{
		auto ptr = BITCAST(uint8_t*, data);
		auto len_ptr = BITCAST(uint8_t*, &size);

		buffer.insert(buffer.end(), len_ptr, len_ptr + sizeof(size));
		buffer.insert(buffer.end(), ptr, ptr + size);
	}

	// puts net object
	//
	void serialize_net_object(vec<uint8_t>& buffer, NetObject* v);

	// serialize dummy
	//
	template <typename... A>
	inline void serialize(vec<uint8_t>& buffer) {}

	// serialize integral types
	//
	template <typename T, typename... A, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
	inline void serialize(vec<uint8_t>& buffer, const T& v, const A&... args)
	{
		serialize_general_data(buffer, &v, sizeof(v));

		serialize(buffer, args...);
	}

	// serialize floating point types
	//
	template <typename T, typename... A, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline void serialize(vec<uint8_t>& buffer, const T& v, const A&... args)
	{
		serialize_general_data(buffer, &v, sizeof(v));

		serialize(buffer, args...);
	}

	// serialize std::string
	//
	template <typename... A>
	inline void serialize(vec<uint8_t>& buffer, const std::string& v, const A&... args)
	{
		serialize_string(buffer, v.data(), v.length());

		serialize(buffer, args...);
	}

	// serialize std::wstring
	//
	template <typename... A>
	inline void serialize(vec<uint8_t>& buffer, const std::wstring& v, const A&... args)
	{
		serialize_string(buffer, v.data(), v.length() * 2u);

		serialize(buffer, args...);
	}

	// serialize net objects
	//
	template <typename T, typename... A, std::enable_if_t<std::derived_from<std::remove_pointer_t<std::remove_reference_t<T>>, NetObject>>* = nullptr>
	inline void serialize(vec<uint8_t>& buffer, const T& v, const A&... args)
	{
		serialize_net_object(buffer, v);

		serialize(buffer, args...);
	}

	// main serialization method
	//
	template <typename... A>
	inline void serialize_params(vec<uint8_t>& buffer, const A&... args)
	{
		serialize(buffer, args...);
	}
}
#endif