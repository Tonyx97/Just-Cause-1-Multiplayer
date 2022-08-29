#include <defs/standard.h>

#include "renderer.h"

#include <core/keycode.h>
#include <core/ui.h>

#include <game/sys/all.h>

#include <core/test_units.h>

// dbg

#include <timer/timer.h>

#include <mp/net.h>

#include <game/object/character/character.h>
#include <game/object/localplayer/localplayer.h>
#include <game/object/character_handle/character_handle.h>

DEFINE_HOOK_FASTCALL(game_present, 0x40FB70, int, void* _this)
{
	g_renderer->on_present();
	g_key->clear_states();

	return game_present_hook(_this);
}

void Renderer::on_present()
{
	g_ui->dispatch();
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