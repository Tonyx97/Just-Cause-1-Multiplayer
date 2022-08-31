#include <defs/standard.h>

#include "pfx_base.h"
#include "pfx_collision.h"

#include "../base/base.h"

#include <mp/net.h>

#include <game/sys/world/world.h>

namespace jc::pfx_collision::hook
{
	DEFINE_HOOK_THISCALL(process, 0x65C2A0, void, PfxCollision* col, vec4* a1, vec3* a2)
	{
		if (const auto localplayer = g_net->get_localplayer())
			if (const auto local_char = g_world->get_localplayer_character())
				if (const auto pfx_base = col->get_pfx())
					if (const auto object = pfx_base->get_userdata<ObjectBase>())
						if (const auto net_obj = g_net->get_net_object_by_game_object(object))
						{
							if (!net_obj->is_owned())
								return;

							process_hook(col, a1, a2);

							net_obj->set_pending_velocity(pfx_base->get_velocity());
						}

		return process_hook(col, a1, a2);
	}

	void enable(bool apply)
	{
		process_hook.hook(apply);
	}
}

PfxBase* PfxCollision::get_pfx() const
{
	if (auto unk = jc::read<ptr>(this, jc::pfx_collision::UNK))
		return jc::read<PfxBase*>(unk, jc::pfx_collision::PFX);

	return nullptr;
}