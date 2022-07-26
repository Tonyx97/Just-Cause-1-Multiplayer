#pragma once

enum eKeyID
{
	KEY_UNKNOWN = -1,
	KEY_LMOUSE = VK_LBUTTON,
	KEY_RMOUSE = VK_RBUTTON,
	KEY_MMOUSE = VK_MBUTTON,
	KEY_MOUSEX4 = VK_XBUTTON1,
	KEY_MOUSEX5 = VK_XBUTTON2,
	KEY_SPACE = VK_SPACE,
	KEY_COMMA = VK_OEM_COMMA,
	KEY_MINUS = VK_OEM_MINUS,
	KEY_PERIOD = VK_OEM_PERIOD,
	KEY_SLASH = VK_OEM_102,
	KEY_SEMICOLON = VK_OEM_1,
	KEY_0 = 0x30,
	KEY_1 = 0x31,
	KEY_2 = 0x32,
	KEY_3 = 0x33,
	KEY_4 = 0x34,
	KEY_5 = 0x35,
	KEY_6 = 0x36,
	KEY_7 = 0x37,
	KEY_8 = 0x38,
	KEY_9 = 0x39,
	KEY_A = 0x41,
	KEY_B = 0x42,
	KEY_C = 0x43,
	KEY_D = 0x44,
	KEY_E = 0x45,
	KEY_F = 0x46,
	KEY_G = 0x47,
	KEY_H = 0x48,
	KEY_I = 0x49,
	KEY_J = 0x4A,
	KEY_K = 0x4B,
	KEY_L = 0x4C,
	KEY_M = 0x4D,
	KEY_N = 0x4E,
	KEY_O = 0x4F,
	KEY_P = 0x50,
	KEY_Q = 0x51,
	KEY_R = 0x52,
	KEY_S = 0x53,
	KEY_T = 0x54,
	KEY_U = 0x55,
	KEY_V = 0x56,
	KEY_W = 0x57,
	KEY_X = 0x58,
	KEY_Y = 0x59,
	KEY_Z = 0x5A,
	KEY_NUM_0 = VK_NUMPAD0,
	KEY_NUM_1 = VK_NUMPAD1,
	KEY_NUM_2 = VK_NUMPAD2,
	KEY_NUM_3 = VK_NUMPAD3,
	KEY_NUM_4 = VK_NUMPAD4,
	KEY_NUM_5 = VK_NUMPAD5,
	KEY_NUM_6 = VK_NUMPAD6,
	KEY_NUM_7 = VK_NUMPAD7,
	KEY_NUM_8 = VK_NUMPAD8,
	KEY_NUM_9 = VK_NUMPAD9,
	KEY_NUM_ADD = VK_ADD,
	KEY_NUM_DECIMAL = VK_DECIMAL,
	KEY_NUM_DIVIDE = VK_DIVIDE,
	KEY_NUM_MULTIPLY = VK_MULTIPLY,
	KEY_NUM_SUBTRACT = VK_SUBTRACT,
	KEY_CAPSLOCK = VK_CAPITAL,
	KEY_DELETE = VK_DELETE,
	KEY_DOWN = VK_DOWN,
	KEY_END = VK_END,
	KEY_ENTER = VK_RETURN,
	KEY_ESCAPE = VK_ESCAPE,
	KEY_F1 = VK_F1,
	KEY_F2 = VK_F2,
	KEY_F3 = VK_F3,
	KEY_F4 = VK_F4,
	KEY_F5 = VK_F5,
	KEY_F6 = VK_F6,
	KEY_F7 = VK_F7,
	KEY_F8 = VK_F8,
	KEY_F9 = VK_F9,
	KEY_F10 = VK_F10,
	KEY_F11 = VK_F11,
	KEY_F12 = VK_F12,
	KEY_HOME = VK_HOME,
	KEY_INSERT = VK_INSERT,
	KEY_LEFT = VK_LEFT,
	KEY_ALT = VK_MENU,
	KEY_SHIFT = VK_SHIFT,
	KEY_CONTROL = VK_CONTROL,
	KEY_LEFT_ALT = VK_LMENU,
	KEY_LEFT_CONTROL = VK_LCONTROL,
	KEY_LEFT_SHIFT = VK_LSHIFT,
	KEY_LEFT_SUPER = VK_LWIN,
	KEY_MENU = VK_MENU,
	KEY_NUM_LOCK = VK_NUMLOCK,
	KEY_PAGE_DOWN = VK_NEXT,
	KEY_PAGE_UP = VK_PRIOR,
	KEY_PAUSE = VK_PAUSE,
	KEY_PRINT_SCREEN = VK_PRINT,
	KEY_RIGHT = VK_RIGHT,
	KEY_RIGHT_ALT = VK_RMENU,
	KEY_RIGHT_CONTROL = VK_RCONTROL,
	KEY_RIGHT_SHIFT = VK_RSHIFT,
	KEY_RIGHT_SUPER = VK_RWIN,
	KEY_SCROLL_LOCK = VK_SCROLL,
	KEY_TAB = VK_TAB,
	KEY_UP = VK_UP,
	KEY_MAX = 512,
};

class Keycode
{
private:
	struct input_state
	{
		bool down	  = false,
			 pressed  = false,
			 released = false;
	} keys_input[512] {};

	std::vector<bool*> pressed_keys,
					   released_keys;

	WNDPROC wnd_proc = nullptr;

	float mouse_wheel_x = 0.f,
		  mouse_wheel_y = 0.f;

public:
	void init();
	void destroy();

	void clear_states();
	void set_key_input(uint32_t key, bool state);
	void set_wnd_proc(HWND wnd);
	void reset_wnd_proc(HWND wnd);
	void set_mouse_wheel_value(const std::tuple<float, float>& val);
	void block_input(bool block);

	bool is_key_down(uint32_t key) const;
	bool is_key_pressed(uint32_t key) const;
	bool is_key_released(uint32_t key) const;

	std::tuple<float, float> get_mouse_wheel_value() const;
};

inline Keycode* g_key = nullptr;