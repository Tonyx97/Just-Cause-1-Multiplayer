#pragma once

template <typename T>
constexpr bool is_net_object_v = std::derived_from<std::remove_pointer_t<std::remove_reference_t<T>>, NetObject>;

NetObject* get_net_object_from_lists(NID nid, NetObjectType type);

template <typename T, typename... A>
void _serialize(serialization_ctx&, const T&, A&&...) requires(is_net_object_v<T>);

template <typename T>
T _deserialize(serialization_ctx&) requires(is_net_object_v<T>);

template <typename T, typename... A>
void _serialize(serialization_ctx& ctx, const T& v, A&&... args) requires(is_net_object_v<T>)
{
	const auto nid = v ? v->get_nid() : INVALID_NID;

	if (nid != INVALID_NID)
	{
		_serialize(ctx, nid);
		_serialize(ctx, v->get_type());
	}
	else _serialize(ctx, INVALID_NID);

	if constexpr (sizeof...(args) > 0)
		_serialize(ctx, args...);
}

template <typename T>
T _deserialize(serialization_ctx& ctx) requires(is_net_object_v<T>)
{
	const auto nid = _deserialize<NID>(ctx);

	if (nid != INVALID_NID)
	{
		const auto type = _deserialize<NetObjectType>(ctx);

		return get_net_object_from_lists(nid, type);
	}

	return nullptr;
}