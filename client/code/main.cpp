#include <defs/libs.h>
#include <defs/standard.h>

#include <game/sys/all.h>

#include <patches/patches.h>

#include <core/clean_dbg.h>
#include <core/keycode.h>
#include <core/test_units.h>
#include <core/ui.h>

#include <mp/net.h>
#include <mp/chat/chat.h>
#include <mp/sync_hooks/sync_hooks.h>

HMODULE g_module = nullptr;

void dll_thread()
{
	jc::prof::init("JC:MP");

	log(GREEN, "Initializing...");

	// patch multi instance shit

	/*while (*(uint16_t*)0x402DDD != 0x15FF)
		SwitchToThread();

	for (int i = 0; i < 11; ++i)
		*(uint8_t*)(0x402DE3 + i) = 0x90;*/

	do
	{
		if (auto renderer = jc::read<Renderer*>(jc::renderer::SINGLETON))
			if (renderer->get_device())
				if (auto world = jc::read<World*>(jc::world::SINGLETON))
					if (auto physics = jc::read<World*>(jc::physics::SINGLETON))
						if (auto spawn_system = jc::read<SpawnSystem*>(jc::spawn_system::SINGLETON))
							break;

		SwitchToThread();
	} while (true);

	// initialize mod systems

	log(GREEN, "Initializing mod systems...");

	g_ui		= JC_ALLOC(UI);
	g_key		= JC_ALLOC(Keycode);
	g_explosion = JC_ALLOC(ExplosionManager);
	g_net		= JC_ALLOC(Net);
	g_chat		= JC_ALLOC(Chat);

	g_key->init();
	g_explosion->init();

	// get game systems

	log(GREEN, "Initializing game systems...");

	g_game_control	= jc::read<GameControl*>(jc::game_control::SINGLETON);
	g_renderer		= jc::read<Renderer*>(jc::renderer::SINGLETON);
	g_world			= jc::read<World*>(jc::world::SINGLETON);
	g_camera		= jc::read<CameraManager*>(jc::camera_manager::SINGLETON);
	g_physics		= jc::read<Physics*>(jc::physics::SINGLETON);
	g_vehicle		= jc::read<VehicleManager*>(jc::vehicle_manager::SINGLETON);
	g_ammo			= jc::read<AmmoManager*>(jc::ammo_manager::SINGLETON);
	g_sound			= jc::read<SoundSystem*>(jc::sound_system::SINGLETON);
	g_time			= jc::read<TimeSystem*>(jc::time_system::SINGLETON);
	g_particle		= jc::read<ParticleSystem*>(jc::particle_system::SINGLETON);
	g_day_cycle		= jc::read<DayCycle*>(jc::day_cycle::SINGLETON);
	g_weapon		= jc::read<WeaponSystem*>(jc::weapon_system::SINGLETON);
	g_spawn			= jc::read<SpawnSystem*>(jc::spawn_system::SINGLETON);
	g_ai			= jc::read<AiCore*>(jc::ai_core::SINGLETON);
	g_game_status	= jc::read<GameStatus*>(jc::game_status::SINGLETON);
	g_rsrc_streamer = jc::read<ResourceStreamer*>(jc::resource_streamer::SINGLETON);

	// initialize net

#ifdef _DEBUG
	char nick[256] = { 0 };

	auto len = DWORD(256);

	GetUserNameA(nick, &len);

	g_net->init("192.168.0.22", nick);
#else
	g_net->init("192.168.0.22", "test_user");	// todojc
#endif

	// initializing MH

	log(GREEN, "Initializing MH and patches...");

	jc::hooks::init();
	jc::clean_dbg::init();
	jc::patches::apply();
	jc::sync_hooks::apply();
	jc::test_units::init();

	// initialize game systems/managers

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
	g_spawn->init();
	g_ai->init();
	g_game_status->init();
	g_rsrc_streamer->init();

	// f8 = exit key (unloads the mod only)

	bool game_exit = false;

	while (!GetAsyncKeyState(VK_F8) && !(game_exit = GetAsyncKeyState(VK_F9)))
		Sleep(50);

	// destroy mod systems

	g_net->destroy();
	g_explosion->destroy();
	g_key->destroy();
	g_ui->wait_until_destruction();

	// destroy game systems

	g_rsrc_streamer->destroy();
	g_game_status->destroy();
	g_ai->destroy();
	g_spawn->destroy();
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

	// uninitialize MH

	jc::test_units::destroy();
	jc::sync_hooks::undo();
	jc::patches::undo();
	jc::clean_dbg::destroy();
	jc::hooks::destroy();

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