#pragma once

#include "../base/base.h"

namespace jc::ui::map_icon
{
	static constexpr uint32_t POSITION = 0x38; // u16vec3
}

class UIMapIcon : public ObjectBase
{
public:

	IMPL_OBJECT_TYPE_ID("CGuiMapIcon");

	bool setup(const vec3& position, uint32_t icon);

	void set_position(const vec3& v);

	vec3 get_position() const;
};