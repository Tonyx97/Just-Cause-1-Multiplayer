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
	void hook_present();
	void unhook_present();

	IDirect3DDevice9* get_device() const;
};

inline Singleton<Renderer, jc::renderer::SINGLETON> g_renderer;