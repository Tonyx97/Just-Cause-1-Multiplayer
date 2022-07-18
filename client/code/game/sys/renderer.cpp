#include <defs/standard.h>

#include "renderer.h"

#include <core/keycode.h>
#include <core/ui.h>

#include <game/sys/all.h>

#include <core/test_units.h>

DEFINE_HOOK_FASTCALL(game_present, 0x40FB70, int, void* _this)
{
	jc::hooks::HookLock lock {};

	g_renderer->on_present();

	return game_present_hook.call(_this);
}

void Renderer::on_present()
{
	g_ui->dispatch();

	jc::test_units::test_0();
}

void Renderer::init()
{
}

void Renderer::destroy()
{
}

void Renderer::hook_present()
{
	game_present_hook.hook();
}

void Renderer::unhook_present()
{
	game_present_hook.unhook();
}

IDirect3DDevice9* Renderer::get_device() const
{
	return jc::read<IDirect3DDevice9*>(this, jc::renderer::DX9_DEVICE);
}