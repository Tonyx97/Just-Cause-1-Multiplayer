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

struct ResourceVerificationCtx
{
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

	std::unordered_map<std::string, Script*> scripts;

	ResourceStatus status = ResourceStatus_Stopped;

public:

	static constexpr auto ALLOWED_CHARACTERS() { return "0123456789abcdefghijklmnopqrstuvwxyz_"; }
	static constexpr auto META_FILE() { return "meta.json"; }

	Resource(const std::string& name, const ResourceVerificationCtx& ctx);
	~Resource();

	bool is_stopped() const { return get_status() == ResourceStatus_Stopped; }
	bool is_running() const { return get_status() == ResourceStatus_Running; }

	ResourceStatus get_status() const { return status; }
	
	ResourceResult start();
	ResourceResult stop();
	ResourceResult restart();
};