#include <defs/standard.h>

#include "obj_event_manager.h"

bool Event::is_executing() const
{
	return jc::read<bool>(this, jc::_event::EXECUTING);
}

int8_t ObjectEventManager::get_base_offset() const
{
	// some instances have the event base at 0xC and others have it at -0x14 for some reason
	// (probably older msvc compiling shit), we want to know which instance is which so we
	// read the first byte of the first virtual func which will tell us if it's one or another
	// and then we can get the actual base offset of event instance with this

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