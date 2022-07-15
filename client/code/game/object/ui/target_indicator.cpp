#include <defs/standard.h>

#include "target_indicator.h"

void UITargetIndicator::set_color(const u8vec4& v)
{
	jc::write(v, this, jc::ui::target_indicator::COLOR);
}

u8vec4 UITargetIndicator::get_color() const
{
	return jc::read<u8vec4>(this, jc::ui::target_indicator::COLOR);
}