#include <defs/standard.h>

#include "obj_event_manager.h"

bool Event::is_executing() const
{
	return jc::read<bool>(this, jc::_event::EXECUTING);
}

int8_t ObjectEventManager::get_base_offset() const
{
	const auto vfn0 = jc::get_vtable_fn(this, 0);
	const auto offset = jc::read<uint8_t>(vfn0) == 0x83 ? -jc::read<int8_t>(vfn0, 0x2) : 0xC;

	check(offset == 0xC || offset == -0x14, "Unknown base offset found: {:x}", offset);

	return offset;
}

ObjectEventManager* ObjectEventManager::get_event_manager() const
{
	return REF(ObjectEventManager*, this, get_base_offset());
}

void ObjectEventManager::setup_event_and_subscribe(ptr offset, const std::string& name, const std::string& userdata_type)
{
	jc::stl::string _name(name.c_str());
	jc::stl::string _userdata_type(userdata_type.c_str());

	jc::this_call(jc::obj_event_manager::fn::SETUP_EVENT_AND_SUBSCRIBE, get_event_manager(), REF(Event**, this, offset), &_name, &_userdata_type);
}

void ObjectEventManager::call_event(ptr offset, void* userdata)
{
	jc::this_call(jc::obj_event_manager::fn::CALL_EVENT, REF(Event**, this, offset), userdata); // not tested
}

void ObjectEventManager::call_event_ex(Event* event_instance, void* userdata)
{
	jc::v_call(get_event_manager(), jc::obj_event_manager::vt::CALL, event_instance, userdata);
}

void ObjectEventManager::call_event_ex(ptr offset, void* userdata)
{
	if (const auto event_instance = get_event(offset))
		call_event_ex(event_instance, userdata);
}

Event* ObjectEventManager::get_event(ptr offset)
{
	// todojc - this only access the first entry of the event list

	return jc::read<Event*>(jc::read<ptr>(jc::read<ptr>(this, offset), 0x4));
}