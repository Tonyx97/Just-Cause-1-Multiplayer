#pragma once

namespace jc::hk::simple_shape_phantom
{
	static constexpr uint32_t WORLD		   = 0x8;
	static constexpr uint32_t THIS_SHAPE   = 0x20;
	static constexpr uint32_t MOTION_STATE = 0x80;
}

class hkMotionState;

class hkSimpleShapePhantom
{
private:
public:
	hkMotionState* get_motion_state() const;
};