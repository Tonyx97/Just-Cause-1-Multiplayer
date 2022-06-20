#include <defs/standard.h>

#include "nh_player.h"

enet::PacketResult nh::player::dispatch(const enet::PacketR& p)
{
	const auto anim_name = p.get_str();

	/*enet::PacketW out(PlayerPID_SetAnim);

	out.add(message);
	out.send_broadcast();*/

	logt(WHITE, "{}: {}", p.get_player_client()->get_nick(), anim_name);

	return enet::PacketRes_Ok;
}