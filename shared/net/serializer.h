#pragma once

#define ENET_FEATURE_ADDRESS_MAPPING

#include <enet.h>

#include <net/packets.h>

#include <shared_mp/objs/all.h>

namespace enet
{
	NetObject* deserialize_net_object(std::vector<uint8_t>& data);

	inline bool deserialize_bool(std::vector<uint8_t>& data)
	{
		const auto value = *BITCAST(bool*, data.data());

		data.erase(data.begin(), data.begin() + sizeof(value));

		return value;
	}

	template <typename T>
	inline T deserialize_general_data(std::vector<uint8_t>& data)
	{
		const auto value = *BITCAST(T*, data.data());

		data.erase(data.begin(), data.begin() + sizeof(T));

		return value;
	}

	template <typename T = int>
	inline T deserialize_int(std::vector<uint8_t>& data)
	{
		static_assert(std::is_integral_v<T>, "Not integral type");

		const auto value = *BITCAST(T*, data.data());

		data.erase(data.begin(), data.begin() + sizeof(T));

		return value;
	}

	template <typename T = float>
	inline T deserialize_float(std::vector<uint8_t>& data)
	{
		static_assert(std::is_floating_point_v<T>, "Not floating type");

		const auto value = *BITCAST(T*, data.data());

		data.erase(data.begin(), data.begin() + sizeof(T));

		return value;
	}

	inline std::string deserialize_string(std::vector<uint8_t>& data)
	{
		size_t size = 0u;

		const auto len = deserialize_int(data);

		std::string out;

		out.resize(len);

		memcpy(out.data(), data.data(), len);

		data.erase(data.begin(), data.begin() + len);

		return out;
	}

	template <typename T>
	inline std::vector<T> deserialize_vector(std::vector<uint8_t>& data)
	{
		const auto size = deserialize_int<uint32_t>(data);

		std::vector<T> vec;

		for (size_t i = 0u; i < size; ++i)
		{
			if constexpr (std::is_same_v<T, std::string>)
				vec.push_back(deserialize_string(data));
			else if constexpr (std::is_integral_v<T>)
				vec.push_back(deserialize_int(data));
			else if constexpr (std::is_floating_point_v<T>)
				vec.push_back(deserialize_float(data));
			else if constexpr (std::derived_from<std::remove_pointer_t<std::remove_reference_t<T>>, NetObject>)
				vec.push_back(deserialize_net_object(data));
			else vec.push_back(deserialize_general_data<T>(data));
		}

		return vec;
	}

	// puts data into the buffer
	//
	inline void serialize_general_data(std::vector<uint8_t>& buffer, const void* data, size_t size)
	{
		auto ptr = BITCAST(uint8_t*, data);

		buffer.insert(buffer.end(), ptr, ptr + size);
	}

	// puts string data into the buffer
	//
	inline void serialize_string(std::vector<uint8_t>& buffer, const void* data, size_t size)
	{
		auto ptr = BITCAST(uint8_t*, data);
		auto len_ptr = BITCAST(uint8_t*, &size);

		buffer.insert(buffer.end(), len_ptr, len_ptr + sizeof(size));
		buffer.insert(buffer.end(), ptr, ptr + size);
	}

	// serialize std::string
	//
	inline void serialize_string(std::vector<uint8_t>& buffer, const std::string& v)
	{
		serialize_string(buffer, v.data(), v.length());
	}

	// serialize std::wstring
	//
	inline void serialize_wstring(std::vector<uint8_t>& buffer, const std::wstring& v)
	{
		serialize_string(buffer, v.data(), v.length() * 2u);
	}

	// serialize floating point types
	//
	inline void serialize_float(std::vector<uint8_t>& buffer, float v)
	{
		serialize_general_data(buffer, &v, sizeof(v));
	}

	// serialize integral types
	//
	template <typename T>
	inline void serialize_int(std::vector<uint8_t>& buffer, T v)
	{
		serialize_general_data(buffer, &v, sizeof(v));
	}

	// puts net object
	//
	inline void serialize_net_object(std::vector<uint8_t>& buffer, NetObject* v)
	{
		serialize_int(buffer, v->get_nid());
		serialize_int(buffer, v->get_type());
	}

	// serialize a group of parameters at the same time
	//
	inline void serialize_params(std::vector<uint8_t>&) {}

	template <typename T, typename... A>
	inline void serialize_params(std::vector<uint8_t>& data, const T& value, A... args)
	{
		if constexpr (std::is_same_v<T, std::string>)
			serialize_string(data, value);
		else if constexpr (std::is_integral_v<T>)
			serialize_int(data, value);
		else if constexpr (std::is_floating_point_v<T>)
			serialize_float(data, value);
		else if constexpr (util::stl::is_vector<T>::value)
		{
			serialize_int(data, value.size());

			for (const auto& e : value)
				serialize_params(data, e);
		}
		else if constexpr (std::derived_from<std::remove_pointer_t<std::remove_reference_t<T>>, NetObject>)
			serialize_net_object(data, value);
		else serialize_general_data(data, &value, sizeof(value));

		serialize_params(data, args...);
	}
}

#define DESERIALIZE_NID_AND_TYPE(p)			const auto nid = enet::deserialize_int(p); \
											const auto type = enet::deserialize_int<NetObjectType>(p)