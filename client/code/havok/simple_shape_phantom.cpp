#include <defs/standard.h>

#include "simple_shape_phantom.h"

hkMotionState* hkSimpleShapePhantom::get_motion_state() const
{
	return REF(hkMotionState*, this, jc::hk::simple_shape_phantom::MOTION_STATE);
}