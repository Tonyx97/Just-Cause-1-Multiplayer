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

	CmdExecRes set_last_error(const std::string& v);
	const std::string& get_last_error();
}