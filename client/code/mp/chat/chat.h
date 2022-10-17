#pragma once

class Chat
{
private:

	std::vector<std::string> cmd_history;

	std::vector<std::string> chat_list;

	std::string curr_msg;

	std::mutex chat_list_mtx;

	float text_size = 18.f,
		  curr = 0.f,
		  max_sx = 390.f,
		  interaction_time = 0.f;

	int scroll_dir = 0,
		cmd_history_index = 0;

	bool typing = false,
		 scrolling = false,
		 typing_blocked = true,
		 enabled = true;

public:

	Chat();

	void key_input(uint32_t key, bool pressed);
	void add_char(wchar_t c);
	void remove_char();
	void remove_word();
	void paste_text();
	void begin_typing();
	void end_typing(bool send = true);
	void add_chat_msg(const std::string& msg);
	void scroll_up();
	void scroll_down();

	void update();
	void enable()									{ enabled = true; }
	void disable()									{ enabled = false; }
	
	bool is_typing() const							{ return typing; }

	int msgs_count() const							{ return static_cast<int>(chat_list.size()); }
};

inline Chat* g_chat = nullptr;