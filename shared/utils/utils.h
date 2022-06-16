#pragma once

#include <codecvt>
#include <functional>

namespace util
{
	namespace string
	{
		inline std::vector<std::string> split(const std::string& str, char delimiter)
		{
			std::stringstream ss(str);

			std::vector<std::string> out;

			std::string curr;

			while (std::getline(ss, curr, delimiter))
				out.push_back(curr);

			return out;
		}

		inline bool split_left(const std::string& str, std::string& left, std::string& right, char delimiter)
		{
			if (auto delimiter_pos = str.find(delimiter); delimiter_pos != std::string::npos)
			{
				std::string temp = str;

				left = temp.substr(0, delimiter_pos);
				right = temp.substr(delimiter_pos + 1);

				return true;
			}

			return false;
		}

		inline std::wstring color_to_string(uint32_t color)
		{
			std::wstringstream ss;

			ss << L"##" << std::hex << std::setw(8) << std::setfill(L'0') << color;

			return ss.str();
		}

		inline bool bool_from_str(const std::string& str)
		{
			return !str.compare("true");
		}

		namespace impl
		{
			template <typename T>
			inline constexpr size_t constexpr_strlen(const T* val)
			{
				size_t size = 0;
				auto p = val;
				while (*p++ != 0) ++size;
				return size;
			}

			template <typename Tx, typename Ty>
			inline auto split_str(const Tx& str, Ty delimitator_beg, Ty delimitator_end)
			{
				std::boyer_moore_horspool_searcher bms(delimitator_beg, delimitator_end);
				std::vector<Tx> words;

				const auto str_end = str.end();

				auto curr_it = str.begin();

				for (auto&& [beg, end] = bms(str.begin(), str_end);
					beg != str_end;
					curr_it = end, std::tie(beg, end) = bms(end, str_end))
				{
					if (const auto& word = Tx(curr_it, beg); !word.empty())
						words.push_back(word);
				}

				if (curr_it != str_end)
					words.push_back(Tx(curr_it, str_end));

				return words;
			}
		}

		template <typename Tx>
		inline auto split(const Tx& str, const Tx& delimiter)
		{
			return impl::split_str(str, delimiter.begin(), delimiter.end());
		}

		template <typename Tx>
		inline auto split(const Tx& str, const char* delimiter)
		{
			return impl::split_str(str, delimiter, delimiter + std::strlen(delimiter));
		}

		template <typename Tx>
		inline auto split(const Tx& str, const wchar_t* delimiter)
		{
			return impl::split_str(str, delimiter, delimiter + std::wcslen(delimiter));
		}

		inline void clear_resize(std::string& str, size_t new_size)
		{
			str.clear();
			str.resize(new_size);
		}
	}

	namespace time
	{
		std::string get_str_date();
		std::string get_str_time();
	}

	namespace hash
	{
		// jenkins hashing algorithm (std::string and std::wstring)
		template <typename T, std::enable_if_t<std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>>* = nullptr>
		constexpr auto JENKINS(const T& str)
		{
			uint32_t hash = 0;

			for (auto c : str)
			{
				hash += static_cast<char>(c) & ~0x20;
				hash += (hash << 10);
				hash ^= (hash >> 6);
			}

			hash += (hash << 3);
			hash ^= (hash >> 11);
			hash += (hash << 15);

			return hash;
		}

		// jenkins hashing algorithm
		template <typename T, std::enable_if_t<std::is_same_v<T, const char*> || std::is_same_v<T, const wchar_t*>>* = nullptr>
		constexpr auto JENKINS(T word)
		{
			uint32_t hash = 0;

			do
			{
				hash += static_cast<char>(*word) & ~0x20;
				hash += (hash << 10);
				hash ^= (hash >> 6);
			} while (*++word);

			hash += (hash << 3);
			hash ^= (hash >> 11);
			hash += (hash << 15);

			return hash;
		}
	}

	namespace win
	{
		inline std::wstring get_clipboard_text()
		{
			if (!OpenClipboard(nullptr))
				return {};

			HANDLE data = GetClipboardData(CF_UNICODETEXT);
			if (!data)
				return {};

			auto text = static_cast<wchar_t*>(GlobalLock(data));
			if (!text)
				return {};

			std::wstring out(text);

			GlobalUnlock(data);

			CloseClipboard();

			return out;
		}
	}
}