#pragma once

#include <script/script.h>

DEFINE_ENUM(ResourceVerification, uint8_t)
{
	ResourceVerification_Ok,
	ResourceVerification_AlreadyExists,
	ResourceVerification_InvalidName,
	ResourceVerification_InvalidMeta,
	ResourceVerification_InvalidScriptList,
	ResourceVerification_InvalidScriptSource,
	ResourceVerification_InvalidScriptType,
	ResourceVerification_ScriptNotExists,
};

DEFINE_ENUM(ResourceResult, uint8_t)
{
	ResourceResult_Ok,
	ResourceResult_Fail,
	ResourceResult_NotExists,
};

DEFINE_ENUM(ResourceStatus, uint8_t)
{
	ResourceStatus_Stopped,
	ResourceStatus_Running,
};

DEFINE_ENUM(ResourceAction, uint8_t)
{
	ResourceResult_Stop,
	ResourceResult_Start,
	ResourceResult_Restart,
};

struct ResourceFileInfo
{
	std::string filename;

	uint64_t lwt; // last write time

	size_t size;
};

struct FileCtx
{
	std::string source;
};

struct ScriptCtx : public FileCtx
{
	ScriptType type;
};

struct FileList
{
	std::unordered_map<std::string, FileCtx> files;
	std::unordered_map<std::string, ScriptCtx> scripts;
};

struct ResourceVerificationCtx
{
	std::string path;
	std::string author;
	std::string version;
	std::string description;

	// server needs all the lists
	// but client only needs client and shared lists

	FileList client,
			 shared;

#ifdef JC_SERVER
	FileList server;
#endif
};

class Resource
{
private:
	
	std::string path;
	std::string name;
	std::string author;
	std::string version;
	std::string description;

	FileList client_files;
	FileList shared_files;

	size_t client_files_total_size = 0u;

#ifdef JC_SERVER
	FileList server_files;
#endif
	
	std::unordered_map<std::string, Script*> scripts;

	ResourceStatus status = ResourceStatus_Stopped;

public:

	static constexpr auto ALLOWED_CHARACTERS() { return "0123456789abcdefghijklmnopqrstuvwxyz_"; }
	static constexpr auto META_FILE() { return "meta.json"; }

	Resource(const std::string& name, const ResourceVerificationCtx& ctx);
	~Resource();

	template <typename Fn>
	void for_each_client_file(const Fn& fn)
	{
		// client files

		for (const auto& [filename, ctx] : client_files.files) fn(filename, &ctx);
		for (const auto& [filename, ctx] : client_files.scripts) fn(filename, &ctx);

		// shared files

		for (const auto& [filename, ctx] : shared_files.files) fn(filename, &ctx);
		for (const auto& [filename, ctx] : shared_files.scripts) fn(filename, &ctx);
	}

#ifdef JC_SERVER
	template <typename Fn>
	void for_each_server_file(const Fn& fn)
	{
		for (const auto& [filename, ctx] : server_files.files) fn(filename, &ctx);
		for (const auto& [filename, ctx] : server_files.scripts) fn(filename, &ctx);
	}

	size_t calculate_total_client_file_size();
	size_t get_total_client_file_size();
#endif

	template <typename Fn>
	void for_each_file(const Fn& fn)
	{
		for_each_client_file(fn);

#ifdef JC_SERVER
		for_each_server_file(fn);
#endif
	}

	bool is_stopped() const { return get_status() == ResourceStatus_Stopped; }
	bool is_running() const { return get_status() == ResourceStatus_Running; }

	ResourceStatus get_status() const { return status; }
	
	ResourceResult start();
	ResourceResult stop();
	ResourceResult restart();

	const std::string& get_name() const { return name; }
	const std::string& get_path() const { return path; }
	const std::string& get_author() const { return author; }
	const std::string& get_version() const { return version; }
	const std::string& get_description() const { return description; }
};