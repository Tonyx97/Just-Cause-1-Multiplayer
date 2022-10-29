#include <defs/standard.h>

#include "cam_settings.h"

#include "../character/character.h"

#include <game/sys/camera/cam_control_manager.h>

namespace jc::cam_settings::hook
{
	DEFINE_INLINE_HOOK_IMPL(camera_focus_height, 0x60AD04)
	{
		// fix camera focus on the attached character when crouching
		
		if (const auto character = ihp->read_ebp<Character*>(0x28))
		{
			if (character->is_crouching())
			{
				g_cam_control->set_default_focus_height(1.4f);
				g_cam_control->set_aim_focus_height(1.2f);
			}
			else
			{
				g_cam_control->set_default_focus_height(1.7f);
				g_cam_control->set_aim_focus_height(2.f);
			}
		}
	}

	void enable(bool apply)
	{
		camera_focus_height_hook.hook(apply);
	}
}