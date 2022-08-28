#include <defs/standard.h>

#include "keycode.h"
#include "ui.h"

#include <imgui/imgui_impl_win32.h>

#include <mp/chat/chat.h>

#include <game/object/vehicle/vehicle.h>
#include <game/sys/world/player_global_info.h>

#define DEBUG_JOYSTICK 0
#define DEBUG_KEY_PRESS 0
#define DEBUG_KEY_DOWN 0

DEFINE_HOOK_THISCALL(get_joystick_value, 0x48C8B0, float, CharacterController* _this, int control)
{
	auto res = get_joystick_value_hook.call(_this, control);

	switch (control)
	{
	case 3:
	case 4:
	case 5:
	case 6: break;
	default:
	{
		// helicopter input sync requires more work than any other vehicle so
		// we need to sync the joystick values from other players

		if (jc::vehicle::getting_helicopter_input())
			jc::vehicle::dispatch_helicopter_input(control, &res);

#if DEBUG_JOYSTICK
		switch (key)
		{
		case 0x2E:
		case 0x2F:
		case 0x30:
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		{
			if (res != 0.f)
				log(RED, "[JOYSTICK KEY] 0x{:x} -> {}, {:x}", key, res, RET_ADDRESS);
		}
		}
#endif
	}
	}
		
	return res;
}

DEFINE_HOOK_THISCALL(is_key_pressed, 0x48C850, bool, CharacterController* _this, int control)
{
	auto res = is_key_pressed_hook.call(_this, control);

#if DEBUG_KEY_PRESS
	if (res)
		log(RED, "[KEY PRESS] 0x{:x} -> {}, {:x}", control, res, RET_ADDRESS);
#endif

	return res;
}

DEFINE_HOOK_THISCALL(is_key_down, 0x48C800, bool, CharacterController* _this, int control)
{
	auto res = is_key_down_hook.call(_this, control);

#if DEBUG_KEY_DOWN
	if (res)
		log(RED, "[KEY DOWN] 0x{:x} -> {}, {:x}", control, res, RET_ADDRESS);
#endif

	return res;
}

// avoids the cursor centering (this patch is permanent, do not restore)
//
patch reset_cursor_patch(0x40346E);

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

void Keycode::hook_key_input()
{
	get_joystick_value_hook.hook();
	is_key_pressed_hook.hook();
	is_key_down_hook.hook();
}

void Keycode::unhook_key_input()
{
	is_key_down_hook.unhook();
	is_key_pressed_hook.unhook();
	get_joystick_value_hook.unhook();
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

void Keycode::block_input(bool block)
{
	// Engine sets the mouse position to the center of the working screen every tick.
	// Since SetCursorPos can be legitimately use, we will disable engine call to the
	// function rather than hooking exported function.
	//

	if (block)
		reset_cursor_patch._do({ 0xEB, 0x49 });
	else reset_cursor_patch._undo();

	// block keys and mouse input
	
	jc::write(!block, 0xAEC00C);
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

bool Keycode::game_is_key_down(int32_t control) const
{
	return is_key_down_hook.call(g_player_global_info->get_local_controller(), control);
}

bool Keycode::game_is_key_pressed(int32_t control) const
{
	return is_key_pressed_hook.call(g_player_global_info->get_local_controller(), control);
}

float Keycode::game_get_joystick_value(int32_t control) const
{
	return get_joystick_value_hook.call(g_player_global_info->get_local_controller(), control);
}

std::tuple<float, float> Keycode::get_mouse_wheel_value() const
{
	return { mouse_wheel_x, mouse_wheel_y };
}