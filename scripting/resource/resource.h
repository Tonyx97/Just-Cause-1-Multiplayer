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
	struct ScriptCtx
	{
		std::string source;

		ScriptType type;
	};

	std::string path;
	std::string author;
	std::string version;
	std::string description;

	std::unordered_map<std::string, ScriptCtx> scripts;
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

public:

	static constexpr auto ALLOWED_CHARACTERS() { return "0123456789abcdefghijklmnopqrstuvwxyz_"; }
	static constexpr auto META_FILE() { return "meta.json"; }

	Resource(const std::string& name, const ResourceVerificationCtx& ctx);
	~Resource();
	
	ResourceResult start();
	ResourceResult stop();
	ResourceResult restart();
};