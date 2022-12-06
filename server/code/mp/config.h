#pragma once

namespace netcp { class tcp_client; }

class Config
{
public:

	struct DefaultFile
	{
		std::vector<uint8_t> data;	// file data

		std::string src_path,	// location of the file in the server fs
					dst_path;	// location the file will take in the client fs
	};

private:

	struct ServerInfo
	{
		std::vector<std::string> startup_rsrcs;

		std::string masterserver_ip,
					ip,
					name,
					password,
					gamemode,
					discord,
					community,
					auth_key;

		int refresh_rate;
		int max_players;
	} server_info;

	std::vector<DefaultFile> default_files;

	json j_server_config,
		 j_server_files;

	netcp::tcp_client* ms_conn = nullptr;

public:

	static constexpr auto CONFIG_FILE() { return "settings.json"; }
	static constexpr auto DEFAULT_FILES_CONFIG_FILE() { return "default_files.json"; }

	bool init();
	bool check_ms_conn(bool force = false);

	void destroy();
	void update(bool force_ms_info = false);

	const ServerInfo& get_info() const { return server_info; }

	size_t get_default_files_count() const { return default_files.size(); }
	
	template <typename Fn>
	void for_each_default_file(const Fn& fn) const
	{
		for (const auto& entry : default_files)
			fn(entry);
	}
};