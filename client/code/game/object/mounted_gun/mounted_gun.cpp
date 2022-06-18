#include <defs/standard.h>

#include "mounted_gun.h"

bool MountedGun::is_begin_used() const
{
	return jc::read<bool>(this, jc::mounted_gun::BEGIN_USED);
}