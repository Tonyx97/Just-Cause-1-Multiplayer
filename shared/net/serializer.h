#pragma once

#include <enet.h>

#include <net/packets.h>

#include <shared_mp/objs/all.h>

namespace enet
{
	// puts data into the buffer
	//
	inline void serialize_general_data(vec<uint8_t>& buffer, size_t at, const void* data, size_t size)
	{
		auto ptr = BITCAST(uint8_t*, data);

		if (at == 1u)
			buffer.insert(buffer.end(), ptr, ptr + size);
		else buffer.insert(buffer.begin() + sizeof(PacketID), ptr, ptr + size);
	}

	// puts string data into the buffer
	//
	inline void serialize_string(vec<uint8_t>& buffer, size_t at, const void* data, size_t size)
	{
		auto ptr = BITCAST(uint8_t*, data);
		auto len_ptr = BITCAST(uint8_t*, &size);

		if (at == 1u)
		{
			buffer.insert(buffer.end(), len_ptr, len_ptr + sizeof(size));
			buffer.insert(buffer.end(), ptr, ptr + size);
		}
		else
		{
			buffer.insert(buffer.begin() + sizeof(PacketID), len_ptr, len_ptr + sizeof(size));
			buffer.insert(buffer.begin() + sizeof(PacketID), ptr, ptr + size);
		}
	}

	// puts net object
	//
	void serialize_net_object(vec<uint8_t>& buffer, size_t at, NetObject* v);

	// serialize dummy
	//
	template <typename... A>
	inline void serialize(vec<uint8_t>& buffer, size_t at) {}

	// serialize integral types
	//
	template <typename T, typename... A, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
	inline void serialize(vec<uint8_t>& buffer, size_t at, const T& v, const A&... args)
	{
		serialize_general_data(buffer, at, &v, sizeof(v));

		serialize(buffer, at, args...);
	}

	// serialize floating point types
	//
	template <typename T, typename... A, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
	inline void serialize(vec<uint8_t>& buffer, size_t at, const T& v, const A&... args)
	{
		serialize_general_data(buffer, at, &v, sizeof(v));

		serialize(buffer, at, args...);
	}

	// serialize std::string
	//
	template <typename... A>
	inline void serialize(vec<uint8_t>& buffer, size_t at, const std::string& v, const A&... args)
	{
		serialize_string(buffer, at, v.data(), v.length());

		serialize(buffer, at, args...);
	}

	// serialize std::wstring
	//
	template <typename... A>
	inline void serialize(vec<uint8_t>& buffer, size_t at, const std::wstring& v, const A&... args)
	{
		serialize_string(buffer, at, v.data(), v.length() * 2u);

		serialize(buffer, at, args...);
	}
	
	// serialize packet structs
	//
	template <typename T, typename... A, std::enable_if_t<std::derived_from<std::remove_pointer_t<std::remove_reference_t<T>>, PacketBase>>* = nullptr>
	inline void serialize(vec<uint8_t>& buffer, size_t at, const T& v, const A&... args)
	{
		serialize_general_data(buffer, at, &v, sizeof(v));

		serialize(buffer, at, args...);
	}

	// serialize net objects
	//
	template <typename T, typename... A, std::enable_if_t<std::derived_from<std::remove_pointer_t<std::remove_reference_t<T>>, NetObject>>* = nullptr>
	inline void serialize(vec<uint8_t>& buffer, size_t at, const T& v, const A&... args)
	{
		serialize_net_object(buffer, at, v);

		serialize(buffer, at, args...);
	}

	// main serialization method
	// 0 -> insert at the beginning
	// 1 -> insert at the end
	//
	template <typename... A>
	inline void serialize_params(vec<uint8_t>& buffer, size_t at, const A&... args)
	{
		serialize(buffer, at, args...);
	}
}