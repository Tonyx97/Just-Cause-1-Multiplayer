#include <defs/standard.h>

#include "renderer.h"

#include <core/keycode.h>
#include <core/ui.h>

#include <game/sys/all.h>

#include <core/test_units.h>

#include <mp/net.h>

int __fastcall Renderer::hk_game_present(void* _this)
{
	jc::hooks::HookLock lock {};

	g_net->tick();
	g_renderer->on_present();
	g_key->clear_states();

	return jc::hooks::call<jc::proto::game_present>(_this);
}

int __stdcall Renderer::hk_reset(void* _this, D3DPRESENT_PARAMETERS* params)
{
	jc::hooks::HookLock lock {};

	g_renderer->on_reset();

	return jc::hooks::call<jc::proto::reset>(_this, params);
}

void Renderer::on_present()
{
	g_ui->dispatch();

	jc::test_units::test_0();
}

void Renderer::on_reset()
{
	// check(false, "Well well, look who decided to be called :o");
}

void Renderer::init()
{
	jc::hooks::hook<jc::proto::game_present>(&hk_game_present);
	jc::hooks::vhook<jc::proto::reset>(get_device(), &hk_reset);
}

void Renderer::destroy()
{
	jc::hooks::unhook<jc::proto::reset>();
	jc::hooks::unhook<jc::proto::game_present>();
}

IDirect3DDevice9* Renderer::get_device() const
{
	return jc::read<IDirect3DDevice9*>(this, jc::renderer::DX9_DEVICE);
}