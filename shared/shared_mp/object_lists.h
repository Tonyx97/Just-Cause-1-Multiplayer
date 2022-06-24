#pragma once

#ifndef JC_LOADER
#include <net/interface.h>

#include <shared_mp/player_client/player_client.h>

class ObjectLists
{
private:
	std::unordered_map<NID, PlayerClient*> player_clients;
	std::unordered_map<NID, Player*> players;
	std::unordered_map<NID, NetObject*> net_objects;

	NetObject* get_net_object_by_nid_impl(NID nid);

public:

#ifdef JC_CLIENT
	PlayerClient* add_player_client(NID nid);

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_reliable(uint32_t id, const A&... args)
	{
		vec<uint8_t> data;
		
		enet::serialize(data, id);
		enet::serialize_params(data, args...);
		enet::send_packet(enet::GET_CLIENT_PEER(), data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE, channel);
	}
#else
	PlayerClient* add_player_client(ENetPeer* peer);

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_broadcast_reliable(PlayerClient* ignore_pc, uint32_t id, const A&... args)
	{
		enet::PacketW p(id);

		p.add(args...);
		p.ready();

		for_each_player_client([&](NID, PlayerClient* pc)
		{
			if (pc != ignore_pc)
				pc->send<channel>(p);
		});
	}

	template <uint8_t channel = ChannelID_Generic, typename... A>
	inline void send_broadcast_reliable(uint32_t id, const A&... args)
	{
		send_broadcast_reliable<channel>(nullptr, id, args...);
	}
#endif

	bool remove_player_client(PlayerClient* pc);

	template <typename T = NetObject>
	T* get_net_object_by_nid(NID nid)
	{
		const auto net_obj = get_net_object_by_nid_impl(nid);

		if constexpr (std::is_same_v<T, NetObject>)
			return net_obj;
		else return net_obj->cast<T>();
	}

	template <typename Fn>
	void for_each_net_object(const Fn& fn)
	{
		for (const auto& [nid, obj] : net_objects)
			fn(nid, obj);
	}

	template <typename Fn>
	void for_each_player_client(const Fn& fn)
	{
		for (const auto& [nid, pc] : player_clients)
			fn(nid, pc);
	}

	size_t get_player_clients_count() const { return player_clients.size(); }
	size_t get_net_objects_count() const { return net_objects.size(); }

	NetObject* add_net_object(NetObject* net_obj);

	bool remove_net_object(NetObject* net_obj);

	Player* get_player_by_nid(NID nid);
	PlayerClient* get_player_client_by_nid(NID nid);
};
#endif