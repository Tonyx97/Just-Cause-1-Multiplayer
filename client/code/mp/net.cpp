#include <defs/standard.h>

#include <ports.h>

#include "net.h"
#include "logic.h"

#include <shared_mp/player_client/player_client.h>

#include <tcp_client.h>

#include <core/ui.h>

#include <game/sys/game/game_status.h>

#include <resource_sys/resource_system.h>

bool Net::init(const std::string& ip, const std::string& pw, const std::string& nick)
{
	this->nick = nick;

	// initialize tpc connection first

	tcp = JC_ALLOC(netcp::tcp_client, [](netcp::client_interface* ci, const netcp::packet_header& header, serialization_ctx& data)
	{
		g_net->on_tcp_message(ci, &header, data);
	});

	log(YELLOW, "Connecting TCP...");

	check(tcp, "Could not initialize TCP connection");
	check(tcp->connect(ip, netcp::CLIENT_TO_SERVER_TCP_PORT), "Could not connect to server via TCP");

	log(GREEN, "TCP Connected");
	
	tcp->send_packet(ClientToMsPacket_Password, pw);

	log(YELLOW, "Waiting for password ack...");

	// wait until we received the password ack

	check(tcp->wait_for(32s, [&]() -> bool { return tcp_ctx.password_ack && tcp_ctx.password_valid; }), "Invalid password");

	log(YELLOW, "Waiting for default server files ack...");

	// wait until we received all default server files

	check(tcp->wait_for(1024s, [&]() -> bool { return tcp_ctx.default_server_files_received; }), "Could not receive default server files");

	// initialize enet after the important tcp communication

	enet::init();

	logb(GREEN, "enet initialized");

	if (!(client = enet_host_create(nullptr, 1, 2, 0, 0)))
		return logb(RED, "Could not create enet client");

	// setup channels

	enet::setup_channels();

	// establish connection

	ENetAddress address = { 0 };
	ENetEvent	e		= {};

	enet_address_set_host(&address, ip.c_str());
	address.port = netcp::CLIENT_TO_SERVER_GAME_PORT;

	if (!(peer = enet_host_connect(client, &address, 255, 0)))
		return logb(RED, "No peers available for initiating enet");

	enet_peer_timeout(peer, 0, 0, enet::PEER_TIMEOUT);

#ifdef JC_DBG
	while (!connected && enet_host_service(client, &e, 1000) >= 0)
#else
	while (!connected && enet_host_service(client, &e, 10000) >= 0)
#endif
		if (e.type == ENET_EVENT_TYPE_CONNECT)
			connected = true;

	check(connected, "Error while connecting to the server");

	enet_peer_ping_interval(peer, 2000);

	log(GREEN, "Connected");

	// packet to initialize the client in the server

	send(Packet(PlayerClientPID_Init, ChannelID_PlayerClient, nick));

	// make sure the net system is initialized before continuing

	while (!is_initialized())
		tick();

	// request complete resource sync (all resources)
	// and also send our NID for the first time through TCP connection
	// so the server can associate the connection with the corresponding
	// PlayerClient instance
	
	tcp->send_packet(ClientToMsPacket_StartupSync, local->get_nid());

	return connected;
}

void Net::pre_destroy()
{
	// destroy the local's player client before clearing the object list

	if (const auto old_local = std::exchange(local, nullptr))
		remove_player_client(old_local);

	// destroy and clear object list

	clear_object_list();

	// close tcp connection

	JC_FREE(tcp);

	tcp = nullptr;
}

void Net::disconnect()
{
	if (!peer || !connected)
		return;

	if (const auto old_local = std::exchange(local, nullptr))
		remove_player_client(old_local);

	enet_peer_disconnect(peer, 0);

	bool disconnected = false;

	enet::dispatch_packets([&](ENetEvent& e)
	{
		switch (e.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy(e.packet);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			disconnected = true;
			break;
		}
	}, 500);

	if (peer)
	{
		enet_peer_reset(peer);

		log(YELLOW, "Disconnected");
	}

	if (timed_out)
		log(RED, "Time out");

	peer	  = nullptr;
	connected = false;
}

void Net::destroy()
{
	disconnect();

	enet_host_destroy(client);
	enet_deinitialize();

	client = nullptr;
}

void Net::add_local(NID nid)
{
	check(!local, "Localplayer must not exists");

	local = add_player_client(nid);
	local->set_nick(nick);
	local->get_player()->set_local();
}

void Net::set_initialized(bool v)
{
	initialized = v;
}

void Net::set_joined(bool v)
{
	if (!is_initialized())
		return;

	// set joined

	joined = v;

	if (joined)
	{
		const auto localplayer = local->get_player();
		const auto local_char = localplayer->get_character();

		// make sure we mark our localplayer as spawned

		localplayer->spawn();

		// let everyone know that we entered the world and spawned

		send(Packet(PlayerClientPID_Join, ChannelID_PlayerClient));
	}
}

void Net::set_game_ready_to_load()
{
	game_ready_to_load = true;
}

void Net::tick()
{
	// check tcp connection

	check(is_tcp_connected(), "TCP connection lost");

	if (!connected || !client)
		return;

	// if the game is ready to load and we downloaded all startup resources
	// then finally get into the game

	if (is_game_ready_to_load() && tcp_ctx.startup_all_resources_synced && !game_loaded)
	{
		game_loaded = true;

		g_game_status->load_game();
	}

	// do the net logic now

	jc::mp::logic::on_tick();

	// send queued packets and dispatch the incoming ones

	enet::dispatch_packets([this](ENetEvent& e)
	{
		switch (e.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
		{
			enet::call_channel_dispatcher(e);
			break;
		}
		case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
		{
			timed_out = true;
			[[fallthrough]];
		}
		case ENET_EVENT_TYPE_DISCONNECT:
		{
			disconnect();
			break;
		}
		}
	});
}

void Net::update_objects()
{
	jc::mp::logic::on_update_objects();
}

bool Net::is_tcp_connected() const
{
	return tcp->is_connected();
}

Player* Net::get_localplayer() const
{
	return local ? local->get_player() : nullptr;
}

void Net::on_tcp_message(netcp::client_interface* ci, const netcp::packet_header* header, serialization_ctx& data)
{
	using namespace netcp;

	auto reset_counters_and_disable_download_bar = [&]()
	{
		tcp_ctx.rsrc.resources_info_count = 0u;
		tcp_ctx.rsrc.downloaded_bytes = 0u;
		tcp_ctx.rsrc.up_to_date_resources = 0u;
		tcp_ctx.rsrc.total_size = 0u;
		tcp_ctx.rsrc.total_resources = 0u;
		tcp_ctx.startup_all_resources_synced = true;

		g_ui->set_download_bar_enabled(false);
	};

	switch (header->id)
	{
	case ClientToMsPacket_Password:
	{
		tcp_ctx.password_ack = true;
		tcp_ctx.password_valid = _deserialize<bool>(data);

		tcp->cancel_sleep();

		break;
	}
	case ClientToMsPacket_SyncDefaultFiles:
	{
		util::fs::create_directory(DEFAULT_SERVER_FILES_PATH());

		const auto default_files_count = _deserialize<size_t>(data);

		for (size_t i = 0u; i < default_files_count; ++i)
		{
			const auto target_path = _deserialize<std::string>(data);
			const auto data_size = _deserialize<size_t>(data);

			std::vector<uint8_t> file_data(data_size);

			data.read(file_data.data(), file_data.size());

			util::fs::create_bin_file(DEFAULT_SERVER_FILES_PATH() + target_path, file_data);
		}

		tcp_ctx.default_server_files_received = true;

		tcp->cancel_sleep();

		break;
	}
	case ClientToMsPacket_SyncResource:
	{
		// it's important to note that the server must send this packet at least once
		// so we can continue, all servers will have several resources so it's fine,
		// we don't want to have a server with no resources anyways...

		const auto rsrcs_count = _deserialize<size_t>(data);
		const auto rsrc_name = _deserialize<std::string>(data);
		const auto rsrc_size = _deserialize<size_t>(data);
		const auto files_info = _deserialize<std::vector<ResourceFileInfo>>(data);

		const auto rsrc_path = ResourceSystem::RESOURCES_FOLDER() + rsrc_name + '\\';
		const bool do_complete_sync = !util::fs::is_directory(rsrc_path) || util::fs::is_empty(rsrc_path);

		// start creating the request packet
		
		serialization_ctx out;

		_serialize(out, rsrc_name);
		_serialize(out, do_complete_sync);

		// if the resource folder doesn't exist then clearly we need
		// to do a complete sync of the resource

		bool is_up_to_date = false;
		
		if (!do_complete_sync)
		{
			// list of outdated/new files we have to request to
			// the server
			//
			std::vector<std::string> files_to_request;

			// get info about the current files in the resource
			// folder and save it

			std::vector<std::string> useless_files;
			std::vector<std::string> outdated_files;
			std::vector<std::string> new_files;

			// get useless and outdated files first

			std::unordered_set<std::string> files_in_directory;

			util::fs::for_each_file_in_directory(rsrc_path, [&](const std::filesystem::directory_entry& p)
			{
				auto filename = p.path().string();

				// it's mandatory that we find the resource path...

				if (const auto i = filename.find(rsrc_path); i != std::string::npos)
				{
					filename = filename.substr(i + rsrc_path.length());

					// save the existing file so we can use it to detect
					// new files as well

					files_in_directory.insert(filename);

					const auto full_filename = rsrc_path + filename;
					const auto it = std::find_if(files_info.begin(), files_info.end(), [&](const ResourceFileInfo& rfi) { return rfi.filename == filename; });

					if (it != files_info.end())
					{
						// check if the file is outdated, if it's up to date then
						// we will not request it

						if (it->lwt != util::fs::get_last_write_time(full_filename))
						{
							files_to_request.push_back(filename);

							tcp_ctx.rsrc.total_size += it->size;
						}
					}
					else
					{
						// this file is not needed anymore so let's remove it

						useless_files.push_back(full_filename);
					}
				}
			});

			// finally get the new files (if any)

			for (const auto& file_info : files_info)
				if (!files_in_directory.contains(file_info.filename))
				{
					files_to_request.push_back(file_info.filename);

					tcp_ctx.rsrc.total_size += file_info.size;
				}

			// remove all useless files

			for (const auto& f : useless_files)
				util::fs::remove(f);

			_serialize(out, files_to_request);

			is_up_to_date = files_to_request.empty();
		}
		else tcp_ctx.rsrc.total_size += rsrc_size;

		// if resources count is 0 then we will get this batch's count

		if (tcp_ctx.rsrc.total_resources == 0u)
			tcp_ctx.rsrc.total_resources = rsrcs_count;
		else check(tcp_ctx.rsrc.total_resources == rsrcs_count, "Out of order TCP packet received, design mistake...");

		// if it's up to date, increase the up to date resources count
		
		if (is_up_to_date)
			++tcp_ctx.rsrc.up_to_date_resources;
		else
		{
			// increase the total size of resources files we have to download

			g_ui->set_download_bar_enabled(true);
			g_ui->set_download_bar_target(static_cast<float>(tcp_ctx.rsrc.total_size));

			// make sure we don't keep empty folders

			util::fs::remove_empty_directories_in_directory(rsrc_path);

			// request the needed files to the server

			ci->send_packet(ClientToMsPacket_ResourceFile, out);
		}

		// if total size is 0 it means there is no resource we need to sync
		// so we will check if we can load now.
		// if the up to date resources count is the same as the amount of resources
		// we received from the server to sync then just reset counters, disable download
		// bar and load game
		
		if (++tcp_ctx.rsrc.resources_info_count == tcp_ctx.rsrc.total_resources &&
			tcp_ctx.rsrc.up_to_date_resources == tcp_ctx.rsrc.total_resources &&
			tcp_ctx.rsrc.total_size == 0u)
		{
			reset_counters_and_disable_download_bar();
		}

		break;
	}
	case ClientToMsPacket_ResourceFile:
	{
		const auto rsrc_name = _deserialize<std::string>(data);
		const auto rsrc_path = ResourceSystem::RESOURCES_FOLDER() + rsrc_name + '\\';
		const auto file = _deserialize<std::string>(data);
		const auto filename = rsrc_path + file;
		const auto data_size = _deserialize<size_t>(data);
		const auto lwt = _deserialize<uint64_t>(data);

		// update the download bar current file display

		g_ui->set_download_bar_file(file);

		// create resource folder if it's not already created

		util::fs::create_directory(rsrc_path);

		// create/replace all the resource files

		const auto parent_path = std::filesystem::path(filename).parent_path().string();

		// create directories needed to place this file

		if (!parent_path.empty())
			util::fs::create_directories(parent_path);

		// create the binary file

		std::vector<uint8_t> file_data(data_size);

		data.read(file_data.data(), file_data.size());

		util::fs::create_bin_file(filename, file_data);

		// update the last write time so it won't send the request for this
		// file to the server if it's not changed

		util::fs::set_last_write_time(filename, lwt);

		// increase downloaded bytes to keep track
		
		tcp_ctx.rsrc.downloaded_bytes += data_size;

		g_ui->set_download_bar_progress(static_cast<float>(tcp_ctx.rsrc.downloaded_bytes));

		// if we downloaded everything we had to then reset the counters
		// and set all resources synced to true

		if (tcp_ctx.rsrc.downloaded_bytes == tcp_ctx.rsrc.total_size)
		{
			reset_counters_and_disable_download_bar();
		}

		break;
	}
	default: logt(RED, "[{}] Unknown packet id: {}", CURR_FN, header->id);
	}
}