#pragma once

#include "../base/base.h"

namespace jc::traffic_light
{
	static constexpr uint32_t INSTANCE_SIZE = 0x110;

	namespace fn
	{
		static constexpr uint32_t SETUP			= 0x7F9C80;
		static constexpr uint32_t SET_LIGHT		= 0x7FA750;
	}
}

enum TrafficLightStatus : uint32_t
{
	TrafficLight_Green,
	TrafficLight_Orange,
	TrafficLight_Red,
};

class TrafficLight : public ObjectBase
{
public:

	IMPL_OBJECT_TYPE_ID("CTrafficLight");

	bool setup(const vec3& position);

	void set_light(uint32_t v);
};