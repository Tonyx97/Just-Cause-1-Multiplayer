#pragma once

enum CmdExecRes
{
	Cmd_Ok,
	Cmd_Invalid,
	Cmd_InvalidParams,
	Cmd_LastError,
};

namespace cmd
{
	using cmd_params = const std::vector<std::string>&;
	using cmd_fn_t = CmdExecRes(cmd_params);

	void register_commands();
	void destroy_all_commands();

	CmdExecRes execute_command(const std::string& cmd, cmd_params params);

	CmdExecRes toggle_netstats(cmd_params);
	CmdExecRes toggle_debug(cmd_params);
	CmdExecRes register_user(cmd_params);
	CmdExecRes login(cmd_params);
	CmdExecRes logout(cmd_params);
	CmdExecRes set_flags(cmd_params);
	CmdExecRes name(cmd_params);
	CmdExecRes start_resource(cmd_params);
	CmdExecRes stop_resource(cmd_params);
	CmdExecRes restart_resource(cmd_params);
	CmdExecRes refresh(cmd_params);

	CmdExecRes set_last_error(const std::string& v);
	const std::string& get_last_error();
}