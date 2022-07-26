#pragma once

#include <codecvt>
#include <functional>
#include <random>

namespace util
{
	namespace stl
	{
		template <typename T>
		struct is_vector
		{
			static constexpr bool value = false;
		};

		template <typename T, typename Alloc>
		struct is_vector<std::vector<T, Alloc>>
		{
			static constexpr bool value = true;
		};
	}

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

		template <typename T>
		inline std::string hex_to_str(T value)
		{
			std::ostringstream ss;
			ss << "0x" << std::hex << value;
			return ss.str();
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

		float get_time();
	}

	namespace container
	{
		template <typename T, typename Fn>
		void for_each_safe_ptr(T& container, const Fn& fn)
		{
			for (auto it = container.begin(); it != container.end();)
			{
				if (fn(&(*it)))
					++it;
				else it = container.erase(it);
			}
		}

		template <typename T, typename Fn>
		void for_each_safe(T& container, const Fn& fn)
		{
			for (auto it = container.begin(); it != container.end();)
			{
				if (fn(*it))
					++it;
				else it = container.erase(it);
			}
		}

		template <typename T, typename Fn>
		void remove_all(T& container, const Fn& fn)
		{
			auto it = container.begin();

			while (it != container.end())
			{
				fn(*it);

				it = container.erase(it);
			}

			container.clear();
		}

		template <typename Tx, typename Ty>
		auto remove_element(Tx& container, Ty element)
		{
			if (auto it = std::remove(container.begin(), container.end(), element); it != container.end())
				return container.erase(it);
			return container.end();
		}

		template <typename Tx, typename Ty>
		bool remove_check(Tx& container, Ty element)
		{
			if (auto it = std::remove(container.begin(), container.end(), element); it != container.end())
			{
				container.erase(it);

				return true;
			}

			return false;
		}

		template <typename T, typename Fn>
		auto remove_if(T& container, const Fn& fn)
		{
			if (auto it = std::remove_if(container.begin(), container.end(), fn); it != container.end())
				return container.erase(it);
			return container.end();
		}

		namespace stack
		{
			template <typename T>
			void free_all(T& container)
			{
				while (!container.empty())
				{
					JC_FREE(container.top());

					container.pop();
				}
			}

			template <typename T, typename Fn>
			void for_each_clear(T& container, const Fn& fn)
			{
				while (!container.empty())
				{
					fn(container.top());

					container.pop();
				}
			}

			template <typename T>
			T::value_type get_and_pop(T& container)
			{
				if (container.empty())
					return nullptr;

				auto e = container.top();

				container.pop();

				return e;
			}
		}

		namespace vector
		{
			template <typename T>
			inline void free_all(T& container)
			{
				if (container.empty())
					return;

				for (const auto& value : container)
					JC_FREE(value);

				container.clear();
			}

			template <typename T, typename Fn>
			inline void remove_and_free_if(T& container, const Fn& fn)
			{
				for (auto it = container.begin(); it != container.end();)
				{
					auto e = *it;

					if (fn(e))
						++it;
					else
					{
						JC_FREE(e);

						it = container.erase(it);
					}
				}
			}

			template <typename T, typename Fn>
			inline void single_remove_and_free_if(T& container, const Fn& fn)
			{
				for (auto e : container)
					if (!fn(e))
					{
						JC_FREE(e);

						return;
					}
			}
		}

		namespace map
		{
			template <typename T>
			inline void free_all(T& container)
			{
				if (container.empty())
					return;

				for (const auto& [key, value] : container)
					TVG_FREE(value);

				container.clear();
			}

			template <typename T, typename Fn>
			inline void remove_and_free_if(T& container, const Fn& fn)
			{
				for (auto it = container.begin(); it != container.end();)
				{
					if (fn(it->first, it->second))
						++it;
					else
					{
						TVG_FREE(it->second);

						it = container.erase(it);
					}
				}
			}

			template <typename T, typename Fn>
			inline void remove_if(T& container, const Fn& fn)
			{
				for (auto it = container.begin(); it != container.end();)
				{
					if (fn(it->first, it->second))
						++it;
					else it = container.erase(it);
				}
			}

			template <typename T, typename Fn>
			inline void remove_free_any_if(T& container, const Fn& fn)
			{
				for (auto it = container.begin(); it != container.end();)
				{
					if (fn(it->first, it->second))
						++it;
					else
					{
						TVG_FREE(it->second);

						container.erase(it);

						return;
					}
				}
			}
		}
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

	namespace rand
	{
		inline std::mt19937_64 mt;

		inline void init_seed() { mt.seed(__rdtsc()); }

		template <typename T>
		T rand_int(T min, T max) { return std::uniform_int_distribution<T>(min, max)(mt); }

		template <typename T>
		T rand_flt(T min, T max) { return std::uniform_real_distribution<T>(min, max)(mt); }
	}

	namespace cpp
	{
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

	void init();
}