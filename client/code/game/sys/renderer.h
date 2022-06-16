#pragma once

namespace jc::renderer
{
	static constexpr uint32_t SINGLETON = 0xAF23A4; // Renderer*

	static constexpr uint32_t DX9_DEVICE = 0x448; // IDirect3DDevice9*
}

class Renderer
{
public:
	void init();
	void destroy();
	void on_present();
	void on_reset();

	IDirect3DDevice9* get_device() const;

	// hooks

	static int __fastcall hk_game_present(void* _this);
	static int __stdcall hk_reset(void* _this, D3DPRESENT_PARAMETERS* params);
};

inline Renderer* g_renderer = nullptr;