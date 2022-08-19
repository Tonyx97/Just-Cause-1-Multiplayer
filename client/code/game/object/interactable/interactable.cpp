#include <defs/standard.h>

#include "interactable.h"

#include <mp/net.h>

#include <game/object/base/base.h>
#include <game/object/vehicle/car.h>

namespace jc::interactable::hook
{
	DEFINE_HOOK_THISCALL(interact_with, 0x78AF10, void, Interactable* interactable, Character* character)
	{
		if (const auto localplayer = g_net->get_localplayer())
			if (const auto local_char = localplayer->get_character(); character == local_char)
				if (const auto target = interactable->get_target())
					if (const auto vehicle_net = g_net->get_net_object_by_game_object(target))
						g_net->send_reliable(PlayerPID_EnterExitVehicle, vehicle_net, true);

		return interact_with_hook.call(interactable, character);
	}

	void apply()
	{
		interact_with_hook.hook();
	}

	void undo()
	{
		interact_with_hook.unhook();
	}
}

void Interactable::interact_with(Character* character)
{
	jc::interactable::hook::interact_with_hook.call(this, character);
}

ObjectBase* Interactable::get_target() const
{
	return jc::read<ObjectBase*>(this, jc::interactable::TARGET);
}