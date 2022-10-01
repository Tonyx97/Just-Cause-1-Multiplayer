#pragma once

DEFINE_ENUM(ScriptType, uint32_t)
{
	ScriptType_Invalid	= 0u,
	ScriptType_Client	= util::hash::JENKINS("client"),
	ScriptType_Server	= util::hash::JENKINS("server"),
	ScriptType_Shared	= util::hash::JENKINS("shared"),
};

namespace luas { class ctx; }

class Script
{
private:

	std::string path;
	std::string name;

	luas::ctx* vm = nullptr;

	ScriptType type = ScriptType_Invalid;

public:

	Script(const std::string& path, const std::string& name, ScriptType type);
	~Script();

	void start();
	void stop();
	void restart();
};