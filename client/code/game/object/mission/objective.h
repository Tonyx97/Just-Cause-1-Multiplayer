#pragma once

#include "../base/base.h"

namespace jc::objective
{
	static constexpr uint32_t TARGET_INDICATOR = 0xA4; // UITargetIndicator*

	namespace vt
	{
	}
}

class UITargetIndicator;

class Objective : public ObjectBase
{
public:

	IMPL_OBJECT_TYPE_ID("CObjective");

	bool setup(const vec3& position, const u8vec4& color);

	UITargetIndicator* get_target_indicator() const;
};