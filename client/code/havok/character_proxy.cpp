#include <defs/standard.h>

#include "character_proxy.h"

void hkCharacterProxy::set_mass(float v)
{
	jc::write(v, this, jc::hk::character_proxy::MASS);
}

float hkCharacterProxy::get_mass() const
{
	return jc::read<float>(this, jc::hk::character_proxy::MASS);
}

hkSimpleShapePhantom* hkCharacterProxy::get_shape() const
{
	return jc::read<hkSimpleShapePhantom*>(this, jc::hk::character_proxy::SIMPLE_SHAPE_PHANTOM);
}