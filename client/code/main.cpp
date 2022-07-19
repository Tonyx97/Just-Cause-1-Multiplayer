#define ENET_IMPLEMENTATION
#define ENET_FEATURE_ADDRESS_MAPPING

#include <enet.h>

#include <defs/libs.h>
#include <defs/standard.h>

#include <timer/timer.h>

#include <game/sys/all.h>

#include <patches/patches.h>

#include <core/clean_dbg.h>
#include <core/keycode.h>
#include <core/test_units.h>
#include <core/ui.h>

#include <mp/net.h>
#include <mp/chat/chat.h>

HMODULE g_module = nullptr;

void dll_thread()
{
	jc::prof::init("JC:MP");

	log(GREEN, "Initializing...");

	util::rand::init_seed();

	// patch multi instance shit

	/*while (*(uint16_t*)0x402DDD != 0x15FF)
		SwitchToThread();

	for (int i = 0; i < 11; ++i)
		*(uint8_t*)(0x402DE3 + i) = 0x90;*/

	do
	{
		if (g_renderer && g_renderer->get_device() &&
			g_world &&
			g_game_control &&
			g_physics &&
			g_game_status &&
			g_day_cycle &&
			g_ai) break;

		SwitchToThread();
	} while (true);

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

	// initialize mod systems

	log(GREEN, "Creating mod systems...");

	g_ui = JC_ALLOC(UI);
	g_key = JC_ALLOC(Keycode);
	g_explosion = JC_ALLOC(ExplosionManager);
	g_net = JC_ALLOC(Net);
	g_chat = JC_ALLOC(Chat);

	// initializing MH

	log(GREEN, "Initializing patches...");

	jc::hooks::init();
	jc::clean_dbg::init();
	jc::patches::apply();
	jc::test_units::init();

	// initialize mod systems

	log(GREEN, "Initializing mod systems...");

	g_key->init();
	g_explosion->init();
	
	// hook present

	log(GREEN, "Hooking...");

	g_renderer->hook_present();
	g_game_control->hook_tick();
	g_game_status->hook_dispatcher();

	// initialize net

	log(GREEN, "Initializing NET...");

#ifdef _DEBUG
	g_net->init("192.168.0.22", nick);
#else
	g_net->init("217.182.174.42", nick);
#endif

	jc::hooks::hook_all();

	log(GREEN, "Mod initialized");

	// f8 = exit key (unloads the mod only)

	bool game_exit = false;

	while (!GetAsyncKeyState(VK_F8) && !(game_exit = GetAsyncKeyState(VK_F9)))
		Sleep(50);

	// clear timers

	g_net->pre_destroy();
	//timer::clear_timers();

	// wait until we disconnected and the ui is cleaned

	g_ui->wait_until_destruction();

	// unhook the present since we cleaned the ui system data

	g_game_status->unhook_dispatcher();
	g_game_control->unhook_tick();
	g_renderer->unhook_present();

	// uninitialize MH

	jc::test_units::destroy();
	jc::patches::undo();
	jc::clean_dbg::destroy();

	// destroy net after all hooks are done

	g_net->destroy();

	// unhook all

	jc::hooks::unhook_all();
	jc::hooks::destroy();

	// destroy the rest of the mod systems

	g_explosion->destroy();
	g_key->destroy();

	// destroy game systems

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

	JC_FREE(g_chat);
	JC_FREE(g_explosion);
	JC_FREE(g_key);
	JC_FREE(g_ui);
	JC_FREE(g_net);

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