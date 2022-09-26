#pragma once

#include <intrin.h>
#include <format>
#include <type_traits>
#include <any>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <lua/lua.hpp>

#define FORMAT(t, a) std::vformat(t, std::make_format_args(a...))
#define FORMATV(t, ...) std::vformat(t, std::make_format_args(__VA_ARGS__))

#define LUA_GET_TYPENAME(i) lua_typename(_state, lua_type(_state, i))

extern "C"
{
	using error_callback_t = int(*)(const char*);

	inline error_callback_t fatal_error_callback = nullptr;
	inline error_callback_t error_callback = nullptr;
};

template <typename... A>
inline void check_fatal(bool condition, const char* text, A... args)
{
	if (condition)
		return;

	fatal_error_callback(FORMAT(text, args).c_str());

	// there is no recoverable way out of this...
	
	exit(EXIT_FAILURE);
}

namespace luas
{
	struct lua_fn;
	class variadic_args;
	class state;

	template <typename T>
	using value_ok = std::pair<T, bool>;

	namespace detail
	{
		template <typename, template <typename...> typename>
		struct is_specialization : std::false_type {};

		template <template <typename...> typename C, typename... A>
		struct is_specialization<C<A...>, C> : std::true_type {};

		template <typename... A>
		concept is_empty_args = sizeof...(A) == 0;

		template <typename T>
		concept is_tuple = is_specialization<T, std::tuple>::value;

		template <typename T>
		concept is_string_array = std::is_array_v<T> && std::is_same_v<std::remove_all_extents_t<std::remove_cvref_t<T>>, char>;

		template <typename T>
		concept is_string = std::is_same_v<T, std::string>;

		template <typename T>
		concept is_bool = std::is_same_v<T, bool>;

		template <typename T>
		concept is_integer = std::is_integral_v<T> && !is_bool<T>;

		template <typename T>
		concept is_integer_or_bool = is_integer<T> || is_bool<T>;

		template <typename T>
		concept is_state = std::is_same_v<T, state>;

		template <typename T>
		concept is_userdata = std::is_pointer_v<T>;

		template <typename T>
		concept is_vector = is_specialization<T, std::vector>::value;

		template <typename T>
		concept is_set = is_specialization<T, std::set>::value || is_specialization<T, std::unordered_set>::value;

		template <typename T>
		concept is_map = is_specialization<T, std::map>::value || is_specialization<T, std::unordered_map>::value;

		template <typename T>
		struct fn_return_type { using type = T; };

		template <typename R, typename... A>
		struct fn_return_type<R(*)(A...)> { using type = R; };

		template <typename T>
		using fn_return_type_v = fn_return_type<T>::type;

		template <typename T>
		struct fn_args_count { static const int value = 0; };

		template <typename R, typename... A>
		struct fn_args_count<R(*)(A...)> { static const int value = sizeof...(A); };

		template <typename R, typename... A>
		struct fn_args_count<R(*)(state&, A...)> { static const int value = sizeof...(A); };

		template <typename T>
		static constexpr auto fn_args_count_v = fn_args_count<T>::value;

		struct tuple_iterator
		{
			template <typename Fn, typename... A>
			static void iterate_impl(const Fn& fn) requires (sizeof...(A) == 0) {}

			template <typename Fn, typename T, typename... A>
			static void iterate_impl(const Fn& fn)
			{
				fn(T{});	// kinda hacky but compiler optimizes it

				iterate_impl<Fn, A...>(fn);
			}

			template <typename T>
			struct iterator { using type = T; };

			template <typename... A>
			struct iterator<std::tuple<A...>>
			{
				template <typename Fn>
				static void iterate(const Fn& fn) { iterate_impl<Fn, A...>(fn); }
			};
		};

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
		struct function_type { using type = std::decay<typename remove_member_ptr_fn<decltype(&std::decay<T>::type::operator())>::type>::type; };

		template <typename T>
		using function_type_v = function_type<T>::type;

		template <typename V, typename... Args>
		struct is_type_last_in_variadics
		{
			template <typename T, typename... A>
			static constexpr bool impl() requires (sizeof...(A) == 0) { return std::is_same_v<V, T>; }

			template <typename T, typename... A>
			static constexpr bool impl() { return impl<A...>(); }

			static constexpr bool value() { if constexpr (sizeof...(Args) > 0) return impl<Args...>(); else return false; }
		};

		template <typename V, typename... Args>
		struct is_type_in_variadics
		{
			template <typename T, typename... A>
			static constexpr bool impl() requires (sizeof...(A) == 0) { return false; }

			template <typename T, typename... A>
			static constexpr bool impl()
			{
				if constexpr (std::is_same_v<V, T>)
					return true;

				return impl<A...>();
			}

			static constexpr bool value() { if constexpr (sizeof...(Args) > 0) return impl<Args...>(); else return false; }
		};
	}

	namespace tuple
	{
		// for each
		//
		template <int i = 0, typename T, int size = std::tuple_size_v<std::decay_t<T>>, typename Fn, typename... A>
		constexpr void for_each(T&& tuple, Fn&& fn, A&&... args)
		{
			if constexpr (i < size)
			{
				std::invoke(fn, args..., std::get<i>(tuple));

				if constexpr (i + 1 < size)
					for_each<i + 1>(std::forward<T>(tuple), std::forward<Fn>(fn), std::forward<A>(args)...);
			}
		}

		// reverse for each
		//
		template <typename T, int i = std::tuple_size_v<std::decay_t<T>> - 1, typename Fn, typename... A>
		constexpr void for_each_reverse(T&& tuple, Fn&& fn, A&&... args)
		{
			if constexpr (i >= 0)
			{
				std::invoke(fn, args..., std::get<i>(tuple));

				if constexpr (i - 1 >= 0)
					for_each_reverse<T, i - 1>(std::forward<T>(tuple), std::forward<Fn>(fn), std::forward<A>(args)...);
			}
		}
	}

	class state
	{
	public:

		static inline void _on_error(lua_State* vm, const std::string& err)
		{
			lua_Debug dbg;

			lua_getstack(vm, 1, &dbg);
			lua_getinfo(vm, ">nSl", &dbg);

			std::string dbg_info;

			if (dbg.name)					dbg_info = FORMATV("[Fn: {}, Line {}] {}", dbg.name, dbg.currentline, err);
			else if (dbg.currentline != -1) dbg_info = FORMATV("[Line {}] {}", dbg.currentline, err);
			else							dbg_info = FORMATV("{}", err);

			error_callback(dbg_info.c_str());
		}

	private:

		lua_State* _state = nullptr;

		template <typename T = int, typename... A>
		static T _throw_error(lua_State* vm, const std::string& err, const A&... args) { _on_error(vm, FORMATV(err, args...)); return T{}; }

		template <typename T = int, typename... A>
		T throw_error(const std::string& err, A&&... args) { _on_error(_state, FORMATV(err, args...)); return T {}; }

		void push_fn(const std::string& fn)
		{
			get_global(fn);

			if (!lua_isfunction(_state, -1))
				throw_error("Function {} undefined", fn);
		}

		template <typename Key, typename Value, typename Fn>
		bool iterate_table(const Fn& fn, int i)
		{
			if (!verify_table(i))
				return true;

			const auto _fail = [this]() { pop_n(2); return false; };

			push_nil();

			while (next(i - 1))
			{
				const auto [key, key_ok] = to_type<Key>(-2);

				if (!key_ok)
					return _fail();

				const auto [value, value_ok] = to_type<Value>(-1);

				if (!value_ok)
					return _fail();

				fn(key, value);

				pop_n(1);
			}

			return true;
		}

		template <typename T>
		void _push(const T& value) requires(detail::is_bool<T>) { push_bool(value); }

		template <typename T>
		void _push(const T& value) requires(detail::is_integer<T>) { push_int(value); }

		template <typename T>
		void _push(const T& value) requires(std::is_floating_point_v<T>) { push_number(value); }

		template <typename T>
		void _push(const T& value) requires(detail::is_string<T> || detail::is_string_array<T>) { push_string(value); }

		template <typename T>
		void _push(const T& value) requires(detail::is_userdata<T> && !std::is_trivial_v<std::remove_pointer_t<T>>) { push_userdata(value); }

		template <typename T>
		void _push(const T& value) requires(detail::is_vector<T> || detail::is_set<T>)
		{
			push_table();

			for (int i = 1; const auto& v : value)
			{
				push(i++, v);
				set_table(-3);
			}
		}

		template <typename T>
		void _push(const T& value) requires(detail::is_map<T>)
		{
			push_table();

			for (const auto& [k, v] : value)
			{
				push(k, v);
				set_table(-3);
			}
		}

		template <typename T>
		T _pop(int& i) requires(detail::is_bool<T>) { return to_bool(i++).first; }

		template <typename T>
		T _pop(int& i) requires(detail::is_integer<T>) { return static_cast<T>(to_int(i++).first); }

		template <typename T>
		T _pop(int& i) requires(std::is_floating_point_v<T>) { return static_cast<T>(to_number(i++).first); }

		template <typename T>
		T _pop(int& i) requires(detail::is_string<T>) { return T(to_string(i++).first); }

		template <typename T>
		T _pop(int& i) requires(detail::is_userdata<T>) { return to_userdata<T>(i++).first; }

		template <typename T>
		T _pop(int& i) requires(detail::is_vector<T> || detail::is_set<T>)
		{
			int table_index = 0;

			if (T out(static_cast<size_t>(raw_len(i))); iterate_table<int, typename T::value_type>([&](const auto& key, const auto& value)
				{
					out[table_index++] = value;
				}, i)) return out;

			return {};
		}

		template <typename T>
		T _pop(int& i) requires(detail::is_map<T>)
		{
			if (T out; iterate_table<typename T::key_type, typename T::mapped_type>([&](const auto& key, const auto& value)
				{
					out[key] = value;
				}, i)) return out;

			return {};
		}

		template <typename T>
		T _pop(int& i) requires(std::is_same_v<T, lua_fn>);

		template <typename T>
		T _pop(int& i) requires(std::is_same_v<T, variadic_args>);

		void get_global(const std::string& name) { lua_getglobal(_state, name.c_str()); }

		bool call_protected(int nargs, int nreturns)
		{
			if (lua_pcall(_state, nargs, nreturns, 0) == 0)
				return true;

			throw_error(lua_tostring(_state, -1));

			return false;
		}

		void print_stack()
		{
			printf_s("\n------------ stack ------------\n");
			
			int top = lua_gettop(_state);

			for (int i = 1; i <= top; i++)
			{
				switch (int t = lua_type(_state, i))
				{
				case LUA_TSTRING: printf_s("\t'%s'", lua_tostring(_state, i)); break;
				case LUA_TBOOLEAN: printf_s(lua_toboolean(_state, i) ? "\ttrue" : "\tfalse"); break;
				case LUA_TNUMBER: printf_s("\t%g", lua_tonumber(_state, i)); break;
				default: printf_s("\t%s", lua_typename(_state, t)); break;
				}

				printf_s("\n");
			}
		}

	public:

		state() {}
		state(lua_State* _state) : _state(_state) {}
		~state() { make_invalid(); }

		lua_State* get() const { return _state; }
		lua_State* operator * () const { return get(); }

		operator bool() const { return !!_state; }

		void set_panic()
		{
			lua_atpanic(_state, [](lua_State* vm)
			{
				const auto str = lua_tostring(vm, -1);

				lua_pop(vm, 1);

				return fatal_error_callback(str);
			});
		}

		void close() { check_fatal(_state, "Invalid state"); lua_close(_state); }
		void make_invalid() { _state = nullptr; }
		void open_libs() { luaL_openlibs(_state); }
		void set_global(const char* index) { lua_setglobal(_state, index); }
		void set_table(int i) { lua_settable(_state, i); }
		void unref(int v) { luaL_unref(_state, LUA_REGISTRYINDEX, v); }
		void push_bool(bool v) { lua_pushboolean(_state, v); }
		void push_int(lua_Integer v) { lua_pushinteger(_state, v); }
		void push_number(lua_Number v) { lua_pushnumber(_state, v); }
		void push_string(const std::string& v) { lua_pushstring(_state, v.c_str()); }
		void push_userdata(void* v) { lua_pushlightuserdata(_state, v); }
		void push_nil() { lua_pushnil(_state); }
		void push_value(int i) { lua_pushvalue(_state, i); }
		void push_table() { lua_newtable(_state); }

		void push() {}
		void pop_n(int n) { lua_pop(_state, n); }
		void exec_string(const std::string_view& code)
		{
			if (luaL_dostring(_state, code.data()) != 0)
				throw_error(lua_tostring(_state, -1));
		}

		template <typename T, typename... A, typename NT = std::remove_cvref_t<T>>
		void push(const T& value, const A&... args)
		{
			_push(value);
			push(args...);
		}

		template <typename T>
		T pop_read(int i = -1) { return _pop<T>(i); }

		template <typename T>
		T pop_track(int& i) { return _pop<T>(i); }

		template <typename T>
		T pop(int i = -1)
		{
			auto value = _pop<T>(i);

			pop_n(1);

			return value;
		}

		int get_top() const { return lua_gettop(_state); }
		int ref() const { return luaL_ref(_state, LUA_REGISTRYINDEX); }
		int get_raw(int i, int n) const { return lua_rawgeti(_state, i, n); }
		int upvalue_index(int i) const { return lua_upvalueindex(i); }
		int next(int i) { return lua_next(_state, i); }

		lua_Unsigned raw_len(int i) const { return lua_rawlen(_state, i); }

		bool verify_table(int i) { return lua_istable(_state, i) ? true : throw_error<bool>("Expected 'table' value, got '{}'", LUA_GET_TYPENAME(i)); }

		value_ok<bool> to_bool(int i)
		{
			if (!lua_isboolean(_state, i))
				return { throw_error<bool>("Expected 'bool' value, got '{}'", LUA_GET_TYPENAME(i)), false };

			return { !!lua_toboolean(_state, i), true };
		}

		template <typename T = int>
		value_ok<T> to_int(int i)
		{
			if (!lua_isinteger(_state, i))
				return { throw_error<T>("Expected 'integer' value, got '{}'", LUA_GET_TYPENAME(i)), false };

			return { static_cast<T>(lua_tointeger(_state, i)), true };
		}

		template <typename T = float>
		value_ok<T> to_number(int i)
		{
			if (!lua_isnumber(_state, i))
				return { throw_error<T>("Expected 'number' value, got '{}'", LUA_GET_TYPENAME(i)), false };

			return { static_cast<T>(lua_tonumber(_state, i)), true };
		}

		value_ok<std::string> to_string(int i)
		{
			if (!lua_isstring(_state, i))
				return { throw_error<std::string>("Expected 'string' value, got '{}'", LUA_GET_TYPENAME(i)), false };

			return { lua_tostring(_state, i), true };
		}

		template <typename T>
		value_ok<T> to_userdata(int i)
		{
			if (!lua_isuserdata(_state, i))
				return { throw_error<std::nullptr_t>("Expected '{}' value, got '{}'", typeid(T).name(), LUA_GET_TYPENAME(i)), false };

			return { static_cast<T>(lua_touserdata(_state, i)), true };
		}

		template <typename T>
		constexpr value_ok<T> to_type(int i)
		{
			if constexpr (detail::is_bool<T>) return to_bool(i);
			else if constexpr (detail::is_integer<T>) return to_int(i);
			else if constexpr (std::is_floating_point_v<T>) return to_number(i);
			else if constexpr (detail::is_string<T>) return to_string(i);
			else if constexpr (detail::is_userdata<T>) return to_userdata<T>(i);

			return { {}, false };
		}

		template <typename T>
		void push_c_closure(const T& value) { lua_pushcclosure(_state, value, 1); }

		template <typename T>
		T* new_userdata() { return static_cast<T*>(lua_newuserdata(_state, sizeof(T))); }

		template <typename... A>
		bool call_safe_fn(const std::string& fn, int nreturns = 0, const A&... args)
		{
			push_fn(fn);
			push(args...);

			return call_protected(sizeof...(A), nreturns);
		}

		template <typename... A>
		bool call_safe(int nreturns = 0, const A&... args)
		{
			push(args...);

			return call_protected(sizeof...(A), nreturns);
		}
	};

	class variadic_args
	{
	private:

		state vm {};

		// storage for values indices

		int first = 0,
			last = 0,
			current = 0;

	public:

		variadic_args(state* vm, int first, int last) : vm(vm->get()), first(first), last(last), current(first) {}
		~variadic_args() { vm.make_invalid(); }

		template <typename T>
		T get(int i) { return vm.pop_read<T>(first + i); }

		int size() const { return (last - first) + 1; }
	};

	template <typename Fn>
	struct lua_c_caller
	{
		template <typename... A, typename... In>
		static int push_and_call_impl(state& _s, int nargs, [[maybe_unused]] int i, In&&... args) requires (detail::is_empty_args<A>)
		{
			using return_type = detail::fn_return_type_v<Fn>;

			const auto pop_args = [&]() { if (nargs > 0) _s.pop_n(nargs); };
			const auto fn = *_s.to_userdata<Fn*>(_s.upvalue_index(1)).first;

			if constexpr (std::is_void_v<return_type>)
			{
				fn(args...);
				pop_args();
			}
			else
			{
				const auto ret = fn(args...);

				pop_args();

				if constexpr (detail::is_tuple<return_type>)
				{
					tuple::for_each(ret, [&]<typename T>(const T& item) { _s.push(item); });

					return std::tuple_size_v<return_type>;
				}
				else if constexpr (!std::is_void_v<return_type>)
				{
					if constexpr (!detail::is_vector<return_type> && !detail::is_set<return_type> && !detail::is_map<return_type>)
						static_assert(std::is_trivial_v<return_type>, "Return type is non-trivial");

					_s.push(ret);

					return 1;
				}
			}

			return 0;
		}

		template <typename T, typename... A, typename... In>
		static int push_and_call_impl(state& _s, int nargs, int i, In&&... args)
		{
			return push_and_call_impl<A...>(_s, nargs, i, args..., _s.pop_track<T>(i));
		}

		template <typename T>
		struct caller { using type = T; };

		template <typename... A>
		static constexpr void variadic_arg_check()
		{
			if constexpr (detail::is_type_in_variadics<variadic_args, A...>::value())
				static_assert(detail::is_type_last_in_variadics<variadic_args, A...>::value(), "variadic_args must appear at the end");
		}

		template <typename R, typename... A>
		struct caller<R(*)(A...)>
		{
			static int push_and_call(state& _s, int nargs) { variadic_arg_check<A...>(); return push_and_call_impl<A...>(_s, nargs, -nargs); }
		};
		
		template <typename R, typename... A>
		struct caller<R(*)(state&, A...)>
		{
			static int push_and_call(state& _s, int nargs) { variadic_arg_check<A...>(); return push_and_call_impl<A...>(_s, nargs, -nargs, _s); }
		};

		static int call(state& _s, int nargs) { return caller<Fn>::push_and_call(_s, nargs); }
	};

	struct lua_fn
	{
		state vm {};

		int ref = 0;

		lua_fn(state* _vm) : vm(_vm->get()) { ref = vm.ref(); }
		~lua_fn() { free_ref(); }

		lua_fn() {}
		lua_fn(const lua_fn&) = delete;
		lua_fn(lua_fn&& other) { *this = std::move(other); }

		void free_ref()
		{
			if (valid())
			{
				vm.unref(ref);
				vm.make_invalid();
			}
		}

		lua_fn& operator=(const lua_fn&) = delete;
		lua_fn& operator=(lua_fn&& other)
		{
			// free the current ref if we own one already

			free_ref();

			// move the ref

			if (other)
			{
				vm = other.vm;
				ref = std::exchange(other.ref, 0);

				other.vm.make_invalid();
			}

			return *this;
		}

		operator bool() const { return valid(); }

		// todo - use this shit to throw errors in calls etc
		//
		bool valid() const { return !!vm; }

		template <typename... T, typename... A>
		void call(A&&... args) requires(detail::is_empty_args<T...>)
		{
			vm.get_raw(LUA_REGISTRYINDEX, ref);
			vm.call_safe(0, args...);
		}

		template <typename... T, typename... A>
		std::tuple<T...> call(A&&... args)
		{
			std::tuple<T...> out {};

			vm.get_raw(LUA_REGISTRYINDEX, ref);

			if (vm.call_safe(sizeof...(T), args...))
				tuple::for_each_reverse(out, [&]<typename T>(T& item) { item = vm.pop<T>(); });

			return out;
		}
	};

	template <typename T>
	T state::_pop(int& i) requires(std::is_same_v<T, lua_fn>)
	{
		const auto index = i++;

		if (!lua_isfunction(_state, index))
			return throw_error<T>("Expected 'function' value, got '{}'", LUA_GET_TYPENAME(index));

		push_value(index);

		return lua_fn(this);
	}

	template <typename T>
	T state::_pop(int& i) requires(std::is_same_v<T, variadic_args>)
	{
		// since variadic arguments are the last, set the index to -1 which is
		// where the first argument was pushed

		return variadic_args(this, std::exchange(i, -1), -1);
	}

	class ctx
	{
	private:

		/**
		 * VARIABLES
		 */

		state* vm = nullptr;

		/**
		 * FUNCTIONS
		 */

		template <typename T, typename Fn>
		void register_fn(const char* index, Fn&& fn)
		{
			const auto function = [](lua_State* _state) -> int
			{
				state vm(_state);

				static constexpr int expected_nargs = detail::fn_args_count_v<T>;

				// todo - do check somewhere else
				// 
				//if (const int nargs = vm.get_top(); expected_nargs == nargs)
					return lua_c_caller<T>::call(vm, vm.get_top());
				//else return state::_throw_error(*vm, "Expected {} arguments, got {}", expected_nargs, nargs);
			};

			const auto fn_obj_loc = vm->new_userdata<T>();

			check_fatal(fn_obj_loc, "Could not allocate placeholder for function");

			new (fn_obj_loc) T(std::move(fn));

			vm->push_c_closure(function);
			vm->set_global(index);
		}

	public:

		ctx(bool open_libs = true)
		{
			vm = new state(luaL_newstate());

			check_fatal(vm, "Could not allocate new");

			vm->set_panic();

			if (open_libs)
				vm->open_libs();
		}

		ctx(const ctx&) = delete;
		ctx(ctx&& other) = delete;

		~ctx()
		{
			vm->close();

			delete vm;
		}

		ctx& operator=(const ctx&) = delete;
		ctx& operator=(ctx&&) = delete;

		void exec_string(const std::string_view& code) { vm->exec_string(code); }

		template <typename... T, typename... A>
		void call_safe(const std::string& fn, A&&... args) requires(detail::is_empty_args<T...>)
		{
			vm->call_safe_fn(fn, 0, args...);
		}

		template <typename... T, typename... A>
		std::tuple<T...> call_safe(const std::string& fn, A&&... args)
		{
			std::tuple<T...> out {};

			if (vm->call_safe_fn(fn, sizeof...(T), args...))
				tuple::for_each_reverse(out, [&]<typename T>(T& item) { item = vm->pop<T>(); });

			return out;
		}

		template <typename T>
		void add_function(const char* index, T&& fn)
		{
			register_fn<detail::function_type_v<T>>(index, std::forward<T>(fn));
		}

		template <typename T>
		void add_global(const char* index, const T& value)
		{
			vm->push(value);
			vm->set_global(index);
		}
	};
};