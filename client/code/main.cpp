#define ENET_IMPLEMENTATION
#define ENET_FEATURE_ADDRESS_MAPPING

#include <enet.h>

#include <defs/libs.h>
#include <defs/standard.h>

#include <timer/timer.h>

#include <game/sys/all.h>

#include <patches/patches.h>

#include <registry/registry.h>
#include <core/task_system/task_system.h>
#include <core/thread_system/thread_system.h>
#include <core/clean_dbg.h>
#include <core/keycode.h>
#include <core/test_units.h>
#include <core/ui.h>

#include <mp/net.h>
#include <mp/chat/chat.h>

#include <game/object/savegame/savegame.h>
#include <game/object/character/character.h>

HMODULE g_module = nullptr;

bool initialized = false;

std::atomic_bool unload_mod = false;
std::atomic_bool mod_unloaded = false;

DEFINE_HOOK_THISCALL_S(tick, 0x4036F0, bool, void* _this)
{
	if (!initialized)
	{
		g_registry.init();

		char nick[256] = { 0 };

		auto len = DWORD(256);

		GetUserNameA(nick, &len);	// todojc

		// initialize game systems/managers

		log(GREEN, "Initializing game systems...");

		g_game_control->init();
		g_renderer->init();
		g_world->init();
		g_camera->init();
		g_physics->init();
		g_vehicle->init();
		g_ammo->init();
		g_sound->init();
		g_time->init();
		g_particle->init();
		g_day_cycle->init();
		g_weapon->init();
		g_factory->init();
		g_ai->init();
		g_game_status->init();
		g_rsrc_streamer->init();
		g_player_global_info->init();
		g_settings->init();

		// initialize mod systems

		log(GREEN, "Creating mod systems...");

		g_ui = JC_ALLOC(UI);
		g_key = JC_ALLOC(Keycode);
		g_explosion = JC_ALLOC(ExplosionManager);
		g_net = JC_ALLOC(Net);
		g_chat = JC_ALLOC(Chat);
		g_task = JC_ALLOC(TaskSystem);
		g_thread_system = JC_ALLOC(ThreadSystem);

		// initializing MH

		log(GREEN, "Initializing patches...");

		jc::clean_dbg::init();
		jc::patches::apply();
		jc::test_units::init();

		// initialize mod systems

		log(GREEN, "Initializing mod systems...");

		g_key->init();
		g_explosion->init();
		g_task->init();

		// hook present

		log(GREEN, "Hooking...");

		g_renderer->hook_present();
		g_game_status->hook_dispatcher();

		jc::hooks::hook_game_fns();

		// initialize net

		log(GREEN, "Initializing NET...");

#ifdef _DEBUG
		g_net->init("192.168.0.24", nick);
#else
		g_net->init(g_registry.get_string("ip"), nick);
#endif

		jc::hooks::hook_queued();

		log(GREEN, "Loaded from hook");

		initialized = true;
	}
	else if (unload_mod)
	{
		// wait until all tasks are processed before destroying everything
		
		g_task->process();

		if (g_task->can_be_destroyed())
		{
			// check that all tasks have been destroyed just in case

			g_task->destroy();

			// clear timers

			timer::clear_timers();

			// unhook the present since we cleaned the ui system data

			g_game_status->unhook_dispatcher();
			g_renderer->unhook_present();

			jc::hooks::unhook_game_fns();

			// uninitialize MH

			jc::test_units::destroy();
			jc::patches::undo();
			jc::clean_dbg::destroy();

			// unhook all

			jc::hooks::unhook_queued();

			// destroy net after all hooks are done

			g_net->pre_destroy();
			g_net->destroy();

			// destroy the rest of the mod systems

			g_explosion->destroy();
			g_key->destroy();
			g_ui->destroy();

			// destroy game systems

			g_settings->destroy();
			g_player_global_info->destroy();
			g_rsrc_streamer->destroy();
			g_game_status->destroy();
			g_ai->destroy();
			g_factory->destroy();
			g_weapon->destroy();
			g_day_cycle->destroy();
			g_particle->destroy();
			g_time->destroy();
			g_sound->destroy();
			g_ammo->destroy();
			g_vehicle->destroy();
			g_physics->destroy();
			g_camera->destroy();
			g_world->destroy();
			g_renderer->destroy();
			g_game_control->destroy();

			// free mod systems

			JC_FREE(g_thread_system);
			JC_FREE(g_task);
			JC_FREE(g_chat);
			JC_FREE(g_explosion);
			JC_FREE(g_key);
			JC_FREE(g_ui);
			JC_FREE(g_net);

			g_registry.destroy();

			log(GREEN, "Unloaded from hook successfully");

			unload_mod = false;
			mod_unloaded = true;
		}
	}
	else if (g_game_control)
		g_game_control->on_tick();

	return tick_hook.call(_this);
}

// patches the loading screen and the GuiLoadSave objects updating when
// the game has started initializing, not before
//
DEFINE_HOOK_THISCALL_S(init_window_context, 0x403EC0, bool, ptr ctx)
{
	auto ok = init_window_context_hook.call(ctx);

	// apply initial patches before the game window opens

	jc::patches::apply_initial_patches();

	// set our settings (todojc - some of these features will be customizable
	// from the launcher)

	g_settings->set_float(SettingType_FxVolume, 1.f);
	g_settings->set_float(SettingType_MusicVolume, 0.f);
	g_settings->set_float(SettingType_DialogueVolume, 0.f);
	g_settings->set_int(SettingType_MotionBlur, 0);
	g_settings->set_int(SettingType_Subtitles, 0);
	g_settings->set_int(SettingType_ActionCamera, 0);
	g_settings->set_int(SettingType_HeatHaze, 0);
	g_settings->set_int(SettingType_TextureResolution, 2);
	g_settings->set_int(SettingType_SceneComplexity, 2);
	g_settings->set_int(SettingType_WaterQuality, 2);
	g_settings->set_int(SettingType_PostFX, 1);

	return ok;
}

#if FAST_LOAD
// bypasses input and visibility of GuiLoadSave object by setting 
// a few bools to true and the action type to 6
//
DEFINE_HOOK_STDCALL(load_save, 0x66D7F0, void*, ref<void*>* r)
{
	static void* main_load_save = nullptr;

	auto res = load_save_hook.call(r);

	if (!main_load_save && res)
	{
		main_load_save = r->obj;

		jc::write<int>(6, main_load_save, 0x34);
		jc::write(true, main_load_save, 0x285);
		jc::write(true, main_load_save, 0x290);
		jc::write(true, main_load_save, 0x293);
	}

	return res;
}

// intercepts the function to read game files, we will create our own buffer which the game will deserialize after being
// patched so it deserializes minimal stuff, not everything is needed for multiplayer, in fact, there is barely useful stuff
//
DEFINE_HOOK_STDCALL(read_save_games_file, 0x45F680, int, jc::stl::string* filename, uint8_t* buffer, size_t size, size_t offset)
{
	SaveGameBuffer savegame;

	savegame.add(1.f);
	savegame.add(Transform(jc::character::g::DEFAULT_SPAWN_LOCATION));
	savegame.add(0);	// ammo grenades

	for (int i = 0; i < 14; ++i)
		savegame.add(0);

	savegame.add(0);
	savegame.add(0); // weapon count

	savegame.add(0);
	savegame.add(0);
	savegame.add(0);
	savegame.add(0);

	savegame.add(0ui8);

	savegame.add(0ui8);
	savegame.add(0ui8);

	savegame.add(25.f);
	savegame.add(3);
	savegame.add(12.5f);
	savegame.add(25.f);

	for (int i = 0; i < 21; ++i)
		savegame.add(true);

	savegame.copy_to(buffer);

	return 0;
}
#endif

void dll_thread()
{
	jc::prof::init("JC:MP");

	// wait until the shit is unpacked lol

	do SwitchToThread();
	while (jc::read<uint32_t>(0x46C859) != 0x02BD92E8
#ifdef JC_DBG
		&& jc::read<uint32_t>(0x46C859) != 0x90909090	// make sure we skip if we already patched this
#endif
		);

	log(GREEN, "Initializing at {:x}...", ptr(g_module));

	util::init();
	jc::hooks::init();

	init_window_context_hook.hook();

#if FAST_LOAD
	read_save_games_file_hook.hook();
	load_save_hook.hook();
#endif

	tick_hook.hook();

	jc::hooks::hook_queued();

	jc::prof::adjust_console();

	// f8 = exit key (unloads the mod only)

	bool game_exit = false;

	while (!GetAsyncKeyState(VK_F8) && !(game_exit = GetAsyncKeyState(VK_F9)))
		Sleep(50);

	unload_mod = true;

	while (!mod_unloaded)
		Sleep(100);

	tick_hook.unhook();
	
#if FAST_LOAD
	load_save_hook.unhook();
	read_save_games_file_hook.unhook();
#endif

	init_window_context_hook.unhook();

	jc::hooks::unhook_queued();
	jc::hooks::destroy();

	// close console and unload dll

	log(GREEN, "Unloaded successfully");

	jc::prof::close_console(true);

	if (game_exit)
		std::exit(EXIT_FAILURE);

	FreeLibraryAndExitThread(g_module, 0);
}

BOOL APIENTRY DllMain(HMODULE instance, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		g_module = instance;

		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)dll_thread, nullptr, 0, nullptr);
	}

	return TRUE;
}