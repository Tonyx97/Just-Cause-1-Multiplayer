#include <defs/standard.h>

#include "keycode.h"
#include "ui.h"

#include <imgui/imgui_impl_win32.h>

#include <mp/chat/chat.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI wnd_proc_callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	bool was_key = false,
		 key_pressed = false;

	switch (msg)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_KEYDOWN:
	{
		was_key = key_pressed = true;
		g_key->set_key_input(wParam, true);
		break;
	}
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_KEYUP:
	{
		was_key = true;
		key_pressed = false;
		g_key->set_key_input(wParam, false);
		break;
	}
	case WM_CHAR:
	{
		switch (const auto c = static_cast<wchar_t>(wParam))
		{
		case 8:   g_chat->remove_char(); break;
		case 127: g_chat->remove_word(); break;
		case 22:  g_chat->paste_text();  break;
		default:
		{
			if (c > 31)
				g_chat->add_char(c);
		}
		}

		break;
	}
	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:
		g_key->set_mouse_wheel_value({ 0.f, static_cast<float>(SHORT(HIWORD(wParam)) / float(WHEEL_DELTA)) });
		break;
	}

	if (was_key)
	{
		// system key input processing

		g_chat->key_input(wParam, key_pressed);
	}

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

void Keycode::init()
{
	pressed_keys.reserve(sizeof(keys_input));
	released_keys.reserve(sizeof(keys_input));
}

void Keycode::destroy()
{
}

void Keycode::clear_states()
{
	for (auto pressed_key_state : pressed_keys)
		*pressed_key_state = false;

	for (auto released_key_state : released_keys)
		*released_key_state = false;

	pressed_keys.clear();
	released_keys.clear();
}

void Keycode::set_key_input(uint32_t key, bool state)
{
	auto& key_input = keys_input[key];

	if (!key_input.down && state)
	{
		key_input.pressed = true;
		pressed_keys.push_back(&key_input.pressed);
	}
	else if (key_input.down && !state)
	{
		key_input.released = true;
		released_keys.push_back(&key_input.released);
	}

	key_input.down = state;
}

void Keycode::set_wnd_proc(HWND wnd)
{
	wnd_proc = BITCAST(WNDPROC, SetWindowLongPtr(wnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wnd_proc_callback)));
}

void Keycode::reset_wnd_proc(HWND wnd)
{
	SetWindowLongPtr(wnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wnd_proc));
}

void Keycode::set_mouse_wheel_value(const std::tuple<float, float>& val)
{
	std::tie(mouse_wheel_x, mouse_wheel_y) = val;
}

void Keycode::hijack_engine_io(bool hijack)
{
	// Engine sets the mouse position to the center of the working screen every tick.
	// Since SetCursorPos can be legitimately use, we will disable engine call to the
	// function rather than hooking exported function.
	//

	// Engine input worker.
	//
	constexpr auto input_stub = static_cast<uintptr_t>(0x00403494);

	// Backup original state.
	//
	static uint8_t original_code[0x15] = {};
	if (original_code[0] == 0x0)
	{
		memcpy_s(&original_code, sizeof original_code, reinterpret_cast<const void*>(input_stub), sizeof original_code);
	}

	const auto is_nop = *reinterpret_cast<uint8_t*>(input_stub) == 0x90;
	if (hijack && !is_nop)
	{
		*(uint8_t*)0x00403494 = 0x90;
		*(uint8_t*)0x004034A2 = 0x90;

		*(uint8_t*)0x004034A3 = 0x90;
		*(uint8_t*)0x004034A4 = 0x90;
		*(uint8_t*)0x004034A5 = 0x90;
		*(uint8_t*)0x004034A6 = 0x90;
		*(uint8_t*)0x004034A7 = 0x90;
		*(uint8_t*)0x004034A8 = 0x90;
	}
	else if (!hijack && is_nop)
	{
		memcpy_s(reinterpret_cast<void*>(input_stub), sizeof original_code, &original_code, sizeof original_code);
	}
	else
	{
		// No need for extra work :).
		//
	}

	// TODO: block character rotation.
	//
}

bool Keycode::is_key_down(uint32_t key) const
{
	return keys_input[key].down;
}

bool Keycode::is_key_pressed(uint32_t key) const
{
	return keys_input[key].pressed;
}

bool Keycode::is_key_released(uint32_t key) const
{
	return keys_input[key].released;
}

std::tuple<float, float> Keycode::get_mouse_wheel_value() const
{
	return { mouse_wheel_x, mouse_wheel_y };
}