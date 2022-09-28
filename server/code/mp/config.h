#pragma once

#include <defs/json.h>

namespace netcp { class tcp_client; }

class Config
{
private:

	struct ServerInfo
	{
		std::string ip,
					name,
					password,
					gamemode,
					discord,
					community;

		int refresh_rate;
	} server_info;

	json server_config;

	netcp::tcp_client* ms_conn = nullptr;

public:

	static constexpr auto CONFIG_FILE() { return "settings.json"; }

	bool init();

	void destroy();
	void process();

	template <typename T>
	std::pair<T, bool> get_field(const std::string& name) const
	{
		try { return { server_config.at(name), true }; }
		catch (...) { return { {}, false }; }
	}

	const ServerInfo& get_info() const { return server_info; }
};