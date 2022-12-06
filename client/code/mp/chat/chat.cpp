#include <defs/standard.h>

#include <core/keycode.h>
#include <core/ui.h>

#include <mp/net.h>
#include <mp/cmd/cmd.h>

#include <game/sys/game/game_status.h>
#include <game/sys/time/time_system.h>

#include "chat.h"

Chat::Chat()
{
	cmd::register_commands();
}

void Chat::key_input(uint32_t key, bool pressed)
{
	if (g_key->is_key_down(KEY_PAGE_UP))
		g_chat->scroll_up();
	else if (g_key->is_key_down(KEY_PAGE_DOWN))
		g_chat->scroll_down();

	if (!enabled || !typing)
	{
		if (g_key->is_key_pressed(KEY_T))
			g_chat->begin_typing();

		/*if (g_chat->is_typing())
			input = 0;
		else if (!lara.frozen)
			input = get_general_input();
		else input = 0;*/

		return;
	}

	if (pressed)
	{
		if (bool escaped = g_key->is_key_pressed(KEY_ESCAPE); escaped || g_key->is_key_pressed(KEY_ENTER))
			g_chat->end_typing(!escaped);
		
		if (!cmd_history.empty())
		{
			auto set_new_message = [&](int index)
			{
				curr_msg = cmd_history[cmd_history_index = std::clamp(cmd_history_index, 0, static_cast<int>(cmd_history.size() - 1))];
			};

			if (key == KEY_UP)		  set_new_message(--cmd_history_index);
			else if (key == KEY_DOWN) set_new_message(++cmd_history_index);
		}
	}
}

void Chat::add_char(wchar_t c)
{
	if (!enabled || !typing)
		return;

	if (std::exchange(typing_blocked, false))
		return;

	if (curr_msg.length() < 256)
		curr_msg += static_cast<char>(c);
}

void Chat::remove_char()
{
	if (!enabled)
		return;

	if (!curr_msg.empty())
		curr_msg.pop_back();
}

void Chat::remove_word()
{
	if (!enabled)
		return;

	if (auto last_space = curr_msg.find_last_of(" _."); last_space == -1)
	{
		if (curr_msg.empty())
			return;
		else curr_msg = "";
	}
	else
	{
		if (!curr_msg.empty() && *(curr_msg.end() - 1) == ' ')
			curr_msg.pop_back();

		curr_msg = curr_msg.substr(0, last_space + 1);
	}
}

void Chat::paste_text()
{
	if (!enabled)
		return;

	//curr_msg += util::win::get_clipboard_text();

	if (curr_msg.length() >= 256)
		curr_msg = curr_msg.substr(0, 256 - 1);
}

void Chat::begin_typing()
{
	if (!enabled || typing)
		return;

	g_key->block_input(true);

	curr_msg = "";

	typing_blocked = true;
	typing = true;
}

void Chat::end_typing(bool send)
{
	if (!typing || !enabled)
		return;

	g_key->block_input(false);

	std::string msg_to_send = curr_msg;

	curr_msg.clear();

	typing = false;

	if (!send)
		return;

	if (msg_to_send.empty())
		return;

	if (*msg_to_send.begin() == L'/')
	{
		cmd_history.push_back(msg_to_send);

		cmd_history_index = static_cast<int>(cmd_history.size());

		std::string cmd,
					params,
					cmd_without_slash = msg_to_send.substr(1);

		if (!util::string::split_left(cmd_without_slash, cmd, params, ' '))
			cmd = cmd_without_slash;

		if (!cmd.compare("clear"))
			chat_list.clear();
		else
		{
			switch (cmd::execute_command(cmd, util::string::split(params, ' ')))
			{
			case Cmd_Invalid:
			{
				add_chat_msg("##ff0000ffInvalid command");
				break;
			}
			case Cmd_InvalidParams:
			{
				add_chat_msg("##ff0000ffInvalid parameters");
				break;
			}
			case Cmd_LastError:
			{
				add_chat_msg("##ff0000ff" + cmd::get_last_error());
				break;
			}
			}
		}
	}
	else g_net->send(Packet(ChatPID_Msg, ChannelID_Chat, msg_to_send));
}

void Chat::add_chat_msg(const std::string& msg)
{
	if (!enabled)
		return;

	std::lock_guard lock(chat_list_mtx);

	chat_list.push_back(msg);

	interaction_time = 0.f;
}

void Chat::scroll_up()
{
	if (!enabled)
		return;

	scrolling = true;
	scroll_dir = -1;
}

void Chat::scroll_down()
{
	if (!enabled)
		return;

	scrolling = true;
	scroll_dir = 1;
}

void Chat::update()
{
	if (!enabled)
		return;

	if (!g_game_status->is_in_game())
		return;

	if (typing)
		interaction_time = 0.f;

	if (interaction_time >= 7.5f)
		return;

	g_ui->begin_window("##chat_wnd", { 15.f, 250.f }, { max_sx + 5.f, 350.f }, { 0.f, 0.f, 0.f, 0.f });

	ImGui::PushFont(g_ui->get_chat_font());

	for (const auto& msg : chat_list)
	{
		const auto curr_pos = ImGui::GetCursorPos();

		ImGui::PushTextWrapPos(max_sx + 4.f);
		ImGui::SetCursorPos({ curr_pos.x + 2.f, curr_pos.y + 2.f });
		ImGui::TextColored(ImVec4(0.f, 0.f, 0.f, 1.f), msg.c_str());
		ImGui::SetCursorPos(curr_pos);
		ImGui::PopTextWrapPos();

		ImGui::PushTextWrapPos(max_sx);
		ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), msg.c_str());
		ImGui::PopTextWrapPos();
	}

	ImGui::PopFont();

	if (scrolling)
	{
		curr = std::lerp(curr, 30.f * float(scroll_dir), 0.1f);

		if (scroll_dir == 0 && std::abs(curr) < 1.f)
			curr = 0.f;

		float new_scroll = ImGui::GetScrollY() + curr;

		ImGui::SetScrollY(new_scroll);

		if (new_scroll >= ImGui::GetScrollMaxY())
		{
			curr = 0.f;
			scrolling = false;
		}
		else scroll_dir = 0;
	}
	else ImGui::SetScrollY(std::ceil(std::lerp(ImGui::GetScrollY(), ImGui::GetScrollMaxY(), 0.25f)));

	g_ui->end_window();

	if (typing)
	{
		auto sy = g_ui->calc_text_size(!curr_msg.empty() ? curr_msg.c_str() : " ", text_size, max_sx).y;

		g_ui->draw_filled_rect(vec2(5.f, 630.f), vec2(max_sx + 5.f, sy + 5.f), { 0.f, 0.f, 0.f, 0.5f });

		if (!curr_msg.empty())
			g_ui->draw_text(curr_msg.c_str(), vec2(10.f, 630.f), text_size, { 1.f, 1.f, 1.f, 1.f }, false, jc::nums::QUARTER_PI, max_sx);
	}

	interaction_time += g_time->get_delta();
}