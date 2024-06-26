#pragma once

#include <codecvt>
#include <functional>
#include <random>
#include <filesystem>

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

		template <typename T>
		struct fn_return_type { using type = void; };

		template <typename R, typename... A>
		struct fn_return_type<R(*)(A...)> { using type = R; };

		template <typename R, typename... A>
		struct fn_return_type<R(__thiscall*)(A...)> { using type = R; };

		template <typename T>
		using fn_return_type_v = fn_return_type<T>::type;

		template <typename T>
		struct remove_member_ptr_fn { using type = void; };

		template <typename T, typename R, typename... A>
		struct remove_member_ptr_fn<R(T::*)(A...)> { typedef R type(A...); };

		template <typename T, typename R, typename... A>
		struct remove_member_ptr_fn<R(T::*)(A...) const> { typedef R type(A...); };

		template <typename T, typename R, typename... A>
		struct remove_member_ptr_fn<R(T::*)(A...) volatile> { typedef R type(A...); };

		template <typename T, typename R, typename... A>
		struct remove_member_ptr_fn<R(T::*)(A...) const volatile> { typedef R type(A...); };

		template <typename T>
		using remove_member_ptr_fn_v = remove_member_ptr_fn<T>::type;

		template <typename T>
		struct function_type { using type = std::decay<typename remove_member_ptr_fn<decltype(&std::decay_t<T>::operator())>::type>::type; };

		template <typename T>
		using function_type_v = function_type<T>::type;
	}

	namespace string
	{
		inline std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

		inline std::wstring convert(const std::string& str)
		{
			return converter.from_bytes(str);
		}

		inline std::string convert(const std::wstring& str)
		{
			return converter.to_bytes(str);
		}

		template <typename T>
		inline std::vector<T> split(const T& str, typename T::value_type delimiter)
		{
			std::vector<T> out;

			T curr;

			if constexpr (std::is_same_v<typename T::value_type, char>)
			{
				std::stringstream ss(str);

				while (std::getline(ss, curr, delimiter))
					out.push_back(curr);
			}
			else
			{
				std::wstringstream ss(str);

				while (std::getline(ss, curr, delimiter))
					out.push_back(curr);
			}

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
		std::string get_str_time_path();

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
					if (!fn(it->first, it->second))
						++it;
					else it = container.erase(it);
				}
			}

			template <typename T, typename Fn>
			inline void remove_free_any_if(T& container, const Fn& fn)
			{
				for (auto it = container.begin(); it != container.end();)
				{
					if (!fn(it->first, it->second))
						++it;
					else
					{
						JC_FREE(it->second);

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
		inline uint64_t seed = 0ull;

		inline void init_seed() { mt.seed(seed = __rdtsc()); }
		inline uint64_t set_seed(uint64_t v) { mt.seed(v); return std::exchange(seed, v); }

		template <typename T>
		T rand_int(T min, T max) { return std::uniform_int_distribution<T>(min, max)(mt); }

		template <typename T>
		T rand_flt(T min, T max) { return std::uniform_real_distribution<T>(min, max)(mt); }

		template <typename T>
		T rand_int(std::mt19937_64& v, T min, T max) { return std::uniform_int_distribution<T>(min, max)(v); }

		template <typename T>
		T rand_flt(std::mt19937_64& v, T min, T max) { return std::uniform_real_distribution<T>(min, max)(v); }
	}

	namespace cpp
	{
	}

	namespace pack
	{
		static constexpr float PI_RANGE_FACTOR = 10431.f;
		static constexpr float NEG1_POS1_FACTOR = 127.f;

		inline int16_t pack_pi_angle(float v) { return static_cast<int16_t>(v * PI_RANGE_FACTOR); }
		inline float unpack_pi_angle(int16_t v) { return static_cast<float>(v) / PI_RANGE_FACTOR; }

		inline int8_t pack_norm(float v) { return static_cast<int8_t>(v * NEG1_POS1_FACTOR); }
		inline float unpack_norm(int8_t v) { return static_cast<float>(v) / NEG1_POS1_FACTOR; }

		template <typename T = int16_t>
		inline T pack_float(float v, float factor) { return static_cast<T>(v * factor); }

		template <typename T = int16_t>
		inline float unpack_float(T v, float factor) { return static_cast<float>(v) / factor; }
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

		inline void set_clipboard_text(const std::string& text)
		{
			if (!OpenClipboard(nullptr)) return;

			EmptyClipboard();

			HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (text.size() + 1) * sizeof(char));

			if (!hglbCopy)
			{
				CloseClipboard();
				return;
			}

			char* lptstrCopy = static_cast<char*>(GlobalLock(hglbCopy));

			memcpy(lptstrCopy, text.c_str(), text.size() + 1);
			GlobalUnlock(hglbCopy);
			SetClipboardData(CF_TEXT, hglbCopy);
			CloseClipboard();
		}

		void get_desktop_resolution(int32_t& x, int32_t& y);
		void get_desktop_pos(int32_t& x, int32_t& y);

		bool set_current_directory(const std::wstring& new_dir);

		std::wstring get_current_directory();

		std::vector<uint8_t> load_resource(void* mod_base, int id, LPWSTR type);
	}

	namespace fs
	{
		using iterate_fn_t = std::function<void(const std::filesystem::directory_entry&)>;

		int64_t get_file_size(std::ifstream& file);

		uint64_t get_last_write_time(const std::string& filename);
		uint64_t file_size(const std::string& filename);

		void set_last_write_time(const std::string& filename, uint64_t new_time);
		
		std::string strip_parent_path(const std::string& str);

		std::vector<uint8_t> read_bin_file(const std::string& filename);
		std::vector<char> read_plain_file(const std::string& filename, bool zero_terminated = true);

		void remove(const std::string& path);
		void create_directory(const std::string& path);
		void create_directories(const std::string& path);
		void remove_empty_directories_in_directory(const std::string& path);
		void for_each_file_in_directory(const std::string& path, const iterate_fn_t& fn);

		bool is_empty(const std::string& path);
		bool is_directory(const std::string& path);
		bool create_bin_file(const std::string& filename, const std::vector<uint8_t>& data);
		bool create_text_file(const std::string& filename, const std::string& data);
	}

	namespace mem
	{
		inline void for_each_module(uint32_t pid, const std::function<bool(uintptr_t base_addr, uint32_t size, const wchar_t* name)>& fn, const std::vector<std::wstring>& ignored_mods)
		{
			MODULEENTRY32 mod_entry;

			mod_entry.dwSize = sizeof(MODULEENTRY32);

			HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);

			if (!Module32First(snapshot, &mod_entry))
			{
				CloseHandle(snapshot);
				return;
			}

			do {
				std::wstring mod_name(mod_entry.szModule);

				std::transform(mod_name.begin(), mod_name.end(), mod_name.begin(), ::tolower);

				if (std::find_if(ignored_mods.begin(), ignored_mods.end(), [&](const std::wstring& str)
					{
						return !str.compare(mod_name);
					}) == ignored_mods.end())
				{
					if (fn(std::bit_cast<uintptr_t>(mod_entry.modBaseAddr), mod_entry.modBaseSize, mod_name.c_str()))
						break;
				}
			} while (Module32Next(snapshot, &mod_entry));

			CloseHandle(snapshot);
		}
	}

	void init();
}