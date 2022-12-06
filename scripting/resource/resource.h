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
	ResourceVerification_Max,
};

DEFINE_ENUM(ResourceResult, uint8_t)
{
	ResourceResult_Fail = ResourceVerification_Max,
	ResourceResult_Ok,
	ResourceResult_NotExists,
	ResourceResult_AlreadyStarted,
	ResourceResult_AlreadyStopped,
	ResourceResult_ScriptError,
	ResourceResult_NotAllowed,
};

DEFINE_ENUM(ResourceStatus, uint8_t)
{
	ResourceStatus_Stopped,
	ResourceStatus_Running,
};

DEFINE_ENUM(ResourceAction, uint8_t)
{
	ResourceAction_Stop,
	ResourceAction_Start,
	ResourceAction_Restart,
};

struct ResourceFileInfo
{
	std::string filename;

	uint64_t lwt; // last write time

	size_t size;

	ScriptType script_type;
};

struct FileCtx
{
	std::string source;

	uint64_t lwt; // last write time

	ScriptType script_type;
};

struct ScriptCtx : public FileCtx
{
	ScriptType type;
};

struct FileList
{
	std::unordered_map<std::string, FileCtx> files;
	std::unordered_map<std::string, ScriptCtx> scripts;

	void clear()
	{
		files.clear();
		scripts.clear();
	}

	size_t size() const { return files.size() + scripts.size(); }
};

struct ResourceVerificationCtx
{
	std::string path;
	std::string author;
	std::string version;
	std::string description;

#ifdef JC_SERVER
	FileList client,
			 shared,
			 server;
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

	size_t client_files_total_size = 0u;
	
	std::unordered_map<std::string, Script*> scripts;

	ResourceStatus status = ResourceStatus_Stopped;

#ifdef JC_CLIENT
	int files_to_download = 0,
		files_downloaded = 0;

	bool up_to_date = false;
#else
	FileList client_files;
	FileList shared_files;
	FileList server_files;

	void destroy_file_lists();
#endif

	void clear();

public:

	static constexpr auto ALLOWED_CHARACTERS() { return "0123456789abcdefghijklmnopqrstuvwxyz_"; }
	static constexpr auto META_FILE() { return "meta.json"; }

	static constexpr std::string RESULT_TO_STRING(ResourceResult v)
	{
		switch (v)
		{
		case ResourceVerification_Ok:
		case ResourceResult_Ok:							return "Ok";
		case ResourceVerification_AlreadyExists:		return "Resource already exists";
		case ResourceVerification_InvalidName:			return "Invalid name";
		case ResourceVerification_InvalidMeta:			return "Invalid meta file";
		case ResourceVerification_InvalidScriptList:	return "Invalid script list";
		case ResourceVerification_InvalidScriptSource:	return "Invalid script source";
		case ResourceVerification_InvalidScriptType:	return "Invalid script type";
		case ResourceVerification_ScriptNotExists:		return "A script does not exist";
		case ResourceResult_AlreadyStarted:				return "Resource already running";
		case ResourceResult_AlreadyStopped:				return "Resource already stopped";
		case ResourceResult_NotExists:					return "Resource does not exist";
		case ResourceResult_NotAllowed:					return "Not allowed";
		default:										return FORMATV("Unknown error: {}", v);
		}
	}

#ifdef JC_CLIENT
	Resource(const std::string& path, const std::string& name);

	void set_up_to_date(bool v) { up_to_date = v; }
	void set_files_downloaded(int v) { files_downloaded = v; }
	void set_files_to_download(int v) { files_to_download = v; }
	void inc_files_downloaded() { ++files_downloaded; }
	void inc_files_to_download() { ++files_to_download; }

	bool is_up_to_date() const { return up_to_date; }

	int get_files_downloaded() const { return files_downloaded; }
	int get_files_to_download() const { return files_to_download; }
#else
	Resource(const std::string& name, const ResourceVerificationCtx& ctx);

	template <typename Fn>
	void for_each_server_file(const Fn& fn)
	{
		for (const auto& [filename, ctx] : server_files.files) fn(filename, &ctx);
		for (const auto& [filename, ctx] : server_files.scripts) fn(filename, &ctx);
	}

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

	template <typename Fn>
	void for_each_file(const Fn& fn)
	{
		for_each_client_file(fn);
		for_each_server_file(fn);
	}

	size_t calculate_total_client_file_size();
	size_t get_total_client_file_size();
	size_t get_total_client_files() { return client_files.size() + shared_files.size(); }

	/*
	* builds the resource information using a new verification context
	*/
	void build_with_verification_ctx(const ResourceVerificationCtx& ctx);
#endif

	~Resource();

	void destroy_scripts();
	void create_script(const ScriptCtx& script_ctx, const std::string& script_name);

	bool is_stopped() const { return get_status() == ResourceStatus_Stopped; }
	bool is_running() const { return get_status() == ResourceStatus_Running; }

	ResourceStatus get_status() const { return status; }
	
	ResourceResult start();
	ResourceResult stop();

	const std::string& get_path() const { return path; }
	const std::string& get_name() const { return name; }
	const std::string& get_author() const { return author; }
	const std::string& get_version() const { return version; }
	const std::string& get_description() const { return description; }
};