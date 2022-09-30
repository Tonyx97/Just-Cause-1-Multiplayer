#include <defs/standard.h>

#include "game_resource.h"

void GameResource::free()
{
	jc::v_call(this, jc::resource::vt::FREE_DATA);
	jc::v_call(this, jc::resource::vt::FREE, 1);
}

bool GameResource::push(const jc::stl::string& file_to_load)
{
	return jc::this_call<bool>(jc::resource::fn::PUSH_TO_QUEUE, this, &file_to_load);
}