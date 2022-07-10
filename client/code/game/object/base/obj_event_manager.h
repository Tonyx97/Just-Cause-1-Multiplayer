#pragma once

namespace jc::obj_event_manager
{
	namespace fn
	{
		static constexpr uint32_t CALL_EVENT				= 0x407FD0;
		static constexpr uint32_t SETUP_EVENT_AND_SUBSCRIBE	= 0x409840;
	}

	namespace vt
	{
		static constexpr uint32_t DESTROY	= 0;
		static constexpr uint32_t CALL		= 1;
	}
}

namespace jc::_event
{
	static constexpr uint32_t EXECUTING = 0x1A;

	namespace fn
	{
	}

	namespace vt
	{
	}
}

struct Event
{
	bool is_executing() const;
};

class ObjectEventManager
{
protected:

	int8_t get_base_offset() const;

	ObjectEventManager* get_event_manager() const;

public:

	void call_event(ptr offset, void* userdata = nullptr);
	void setup_event_and_subscribe(ptr offset, const std::string& name, const std::string& userdata_type = "");
};