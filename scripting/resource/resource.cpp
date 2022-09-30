#include <defs/standard.h>

#include "resource.h"

Resource::Resource(const std::string& name) : name(name)
{

}

Resource::~Resource()
{

}

ResourceResult Resource::start()
{
	logt_nl(YELLOW, "Starting up '{}'... ", name);

	log(GREEN, "OK");

	return ResourceResult_Ok;
}