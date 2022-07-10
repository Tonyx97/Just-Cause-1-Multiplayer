#include <defs/standard.h>

#include "obj_event_manager.h"

bool Event::is_executing() const
{
	return jc::read<bool>(this, jc::_event::EXECUTING);
}

void ObjectEventManager::setup_event_and_subscribe(ptr offset, const std::string& name, const std::string& userdata_type)
{
	// todojc - some instances don't have the instance at 0xC (be careful)

	jc::stl::string _name(name.c_str());
	jc::stl::string _userdata_type(userdata_type.c_str());

	jc::this_call(jc::obj_event_manager::fn::SETUP_EVENT_AND_SUBSCRIBE, this, REF(void*, this, offset - 0xC), &_name, &_userdata_type);
}

void ObjectEventManager::call_event(ptr offset, void* userdata)
{
	jc::this_call(jc::obj_event_manager::fn::CALL_EVENT, REF(ptr, this, offset - 0xC), userdata); // not tested
}

void ObjectEventManager::call_event_ex(Event* event_instance, void* userdata)
{
	jc::v_call(this, jc::obj_event_manager::vt::CALL, event_instance, userdata);
}

void ObjectEventManager::call_event_ex(ptr offset, void* userdata)
{
	if (const auto event_instance = get_event(offset))
		call_event_ex(event_instance, userdata);
}

Event* ObjectEventManager::get_event(ptr offset)
{
	// todojc - this only access the first entry of the event list

	return jc::read<Event*>(jc::read<ptr>(jc::read<ptr>(this, offset - 0xC), 0x4));
}