#include <defs/standard.h>

#include <mp/net.h>

#include "cmd.h"

#include <resource_sys/resource_system.h>

namespace cmd
{
	std::unordered_map<std::string, std::function<cmd_fn_t>> g_cmds;

	std::string last_error;

	CmdExecRes execute_command(const std::string& cmd, cmd_params params)
	{
		if (auto it = g_cmds.find(cmd); it != g_cmds.end())
			return it->second(params);

		return Cmd_Invalid;
		//return (g_resource->call_command(cmd, params) ? CMD_EXEC_OK : CMD_EXEC_INVALID_CMD);
	}

	CmdExecRes toggle_netstats(cmd_params params)
	{
		if (params.size() != 1)
			return Cmd_InvalidParams;

		if (int level = std::stoi(params[0]); level >= 0 && level <= 3)
			g_net->set_net_stats(level);
		else return Cmd_InvalidParams;

		return Cmd_Ok;
	}

	CmdExecRes toggle_debug(cmd_params)
	{
		//g_debugger->toggle();

		return Cmd_Ok;
	}

	CmdExecRes register_user(cmd_params params)
	{
		if (params.size() != 2)
			return Cmd_InvalidParams;

		g_net->send(Packet(PlayerClientPID_RegisterUser, ChannelID_PlayerClient, params[0], params[1]));

		return Cmd_Ok;
	}

	CmdExecRes login(cmd_params params)
	{
		if (params.size() != 2)
			return Cmd_InvalidParams;

		g_net->send(Packet(PlayerClientPID_LoginUser, ChannelID_PlayerClient, params[0], params[1]));

		return Cmd_Ok;
	}

	CmdExecRes logout(cmd_params params)
	{
		if (params.size() != 0)
			return Cmd_InvalidParams;

		g_net->send(Packet(PlayerClientPID_LogoutUser, ChannelID_PlayerClient));

		return Cmd_Ok;
	}

	CmdExecRes set_flags(cmd_params params)
	{
		if (params.size() != 2)
			return Cmd_InvalidParams;

		/*if (uint64_t flags = std::stoi(params[1]); flags != 0)
		{
			gns::net_player::net_flags info {};

			info.user = params[0].c_str();
			info.flags = flags;

			if (!g_client->send_packet(ID_NET_PLAYER_NET_FLAGS, info))
				return set_last_error("logout failed");
		}*/

		return Cmd_Ok;
	}

	CmdExecRes name(cmd_params params)
	{
		if (params.size() != 1)
			return Cmd_InvalidParams;

		/*if (!g_client->set_name(params[0]))
			return set_last_error("Name is too short or too big");*/

		return Cmd_Ok;
	}

	CmdExecRes start_resource(cmd_params params)
	{
		if (params.size() != 1)
			return Cmd_InvalidParams;

		g_net->send(Packet(PlayerClientPID_ResourceAction, ChannelID_PlayerClient, params[0], ResourceResult_Start));

		return Cmd_Ok;
	}

	CmdExecRes stop_resource(cmd_params params)
	{
		if (params.size() != 1)
			return Cmd_InvalidParams;

		g_net->send(Packet(PlayerClientPID_ResourceAction, ChannelID_PlayerClient, params[0], ResourceResult_Stop));

		return Cmd_Ok;
	}

	CmdExecRes restart_resource(cmd_params params)
	{
		if (params.size() != 1)
			return Cmd_InvalidParams;

		g_net->send(Packet(PlayerClientPID_ResourceAction, ChannelID_PlayerClient, params[0], ResourceResult_Restart));

		return Cmd_Ok;
	}

	CmdExecRes refresh(cmd_params params)
	{
		if (params.size() != 0)
			return Cmd_InvalidParams;

		/*if (!g_client->send_packet(ID_RESOURCE_REFRESH_ALL))
			return set_last_error("Refresh failed");*/

		return Cmd_Ok;
	}

	CmdExecRes set_last_error(const std::string& v)
	{
		last_error = v;
		return Cmd_LastError;
	}

	const std::string& get_last_error()
	{
		return last_error;
	}

	void register_commands()
	{
		// debug commands

		g_cmds.insert({ "debug", toggle_debug });
		g_cmds.insert({ "netstat", toggle_netstats });

		// player commands

		g_cmds.insert({ "register", register_user });
		g_cmds.insert({ "login", login });
		g_cmds.insert({ "logout", logout });
		g_cmds.insert({ "name", name });
		g_cmds.insert({ "set_flags", set_flags });

		// resource commands

		g_cmds.insert({ "start", start_resource });
		g_cmds.insert({ "stop", stop_resource });
		g_cmds.insert({ "restart", restart_resource });
		g_cmds.insert({ "refresh", refresh });
	}

	void destroy_all_commands()
	{
		g_cmds.clear();
	}
}