#pragma once

DEFINE_ENUM(ScriptType, uint32_t)
{
	ScriptType_Client = util::hash::JENKINS("client"),
	ScriptType_Server = util::hash::JENKINS("server"),
	ScriptType_Shared = util::hash::JENKINS("shared"),
};