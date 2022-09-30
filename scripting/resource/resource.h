#pragma once

DEFINE_ENUM(ResourceResult, uint8_t)
{
	ResourceResult_Ok,
	ResourceResult_Fail,
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

class Resource
{
private:

public:
	
};