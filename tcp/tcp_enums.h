#pragma once

namespace netcp
{
	DEFINE_ENUM(ServerClientType, uint8_t)
	{
		ServerClientType_None,
		ServerClientType_Client,
		ServerClientType_Server,
	};
}