#pragma once

class DebugUI
{
private:

	bool show_overlay_debug = true;
	bool show_admin_panel = false;

#ifdef JC_DBG
	bool admin_panel_available = true;
#else
	bool admin_panel_available = true;
#endif

#ifdef JC_DBG
	bool show_skeleton			= true,
#else
	bool show_skeleton			= false,
#endif
		 show_velocity			= false,
		 show_distance			= false,
		 show_health			= false,
		 show_view_direction	= false,
		 show_muzzle			= false,
		 show_bullets			= true,
		 show_grip				= false,
		 show_last_muzzle		= false,
		 show_last_grip			= false,
		 infinite_ammo			= false,
		 no_clip				= true,
		 godmode				= false,
		 show_top_dbg			= false;

	void overlay_debug();
	void net_debug();

public:

	void render_admin_panel();
	void render_net();
	void render_overlay();
	void toggle_admin_panel();
	void set_admin_panel_available(bool v) { admin_panel_available = v; }
};