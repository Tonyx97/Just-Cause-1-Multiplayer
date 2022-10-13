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
#include <functional>

#include <lua/lua.hpp>

#define FORMAT(t, a) std::vformat(t, std::make_format_args(a...))
#define FORMATV(t, ...) std::vformat(t, std::make_format_args(__VA_ARGS__))

#define LUA_GET_TYPENAME(i) lua_typename(_state, lua_type(_state, i))

namespace luas
{
	struct lua_fn;
	class variadic_args;
	class state;
}

extern "C"
{
	using error_callback_t = int(*)(const char*);
	using custom_stack_pusher_t = int(*)(const luas::state& state, const std::any& v);

	inline error_callback_t fatal_error_callback = nullptr;
	inline error_callback_t error_callback = nullptr;
	inline custom_stack_pusher_t custom_stack_pusher = nullptr;
};

#define TYPEINFO(x) const_cast<std::type_info*>(&typeid(x))

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
	template <typename T>
	struct value_ok
	{
		T first;
		bool second;
	};

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
		concept is_string_array = std::is_array_v<std::remove_cvref_t<T>> && std::is_same_v<std::remove_all_extents_t<std::remove_cvref_t<T>>, char>;

		template <typename T>
		concept is_string_ptr = std::is_pointer_v<T> && std::is_same_v<std::remove_cvref_t<std::remove_pointer_t<T>>, char>;

		template <typename T>
		concept is_string = std::is_same_v<T, std::string> || is_string_ptr<T> || is_string_array<T>;

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
		concept is_vector = is_specialization<T, std::vector>::value && !std::is_same_v<typename T::value_type, std::any>;

		template <typename T>
		concept is_any_vector = is_specialization<T, std::vector>::value && std::is_same_v<typename T::value_type, std::any>;

		template <typename T>
		concept is_set = is_specialization<T, std::set>::value || is_specialization<T, std::unordered_set>::value;

		template <typename T>
		concept is_map = is_specialization<T, std::map>::value || is_specialization<T, std::unordered_map>::value;

		template <typename T>
		struct fn_return_type { using type = T; };

		template <typename R, typename... A>
		struct fn_return_type<R(*)(A...)> { using type = R; };

		template <typename R, typename... A>
		struct fn_return_type<R(__thiscall*)(A...)> { using type = R; };

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
		using remove_member_ptr_fn_v = remove_member_ptr_fn<T>::type;

		template <typename T>
		struct keep_member_ptr_fn { using type = void; };

		template <typename T, typename R, typename... A>
		struct keep_member_ptr_fn<R(T::*)(A...)> { using type = R(__thiscall*)(T*, A...); };

		template <typename T, typename R, typename... A>
		struct keep_member_ptr_fn<R(T::*)(A...) const> { using type = R(__thiscall*)(T*, A...); };

		template <typename T, typename R, typename... A>
		struct keep_member_ptr_fn<R(T::*)(A...) volatile> { using type = R(__thiscall*)(T*, A...); };

		template <typename T, typename R, typename... A>
		struct keep_member_ptr_fn<R(T::*)(A...) const volatile> { using type = R(__thiscall*)(T*, A...); };

		template <typename T>
		using keep_member_ptr_fn_v = keep_member_ptr_fn<T>::type;

		template <typename T>
		struct first_arg_member_fn { using type = void; };

		template <typename T, typename R, typename F, typename... A>
		struct first_arg_member_fn<R(T::*)(F, A...)> { using type = F; };

		template <typename T, typename R, typename F, typename... A>
		struct first_arg_member_fn<R(T::*)(F, A...) const> { using type = F; };

		template <typename T, typename R, typename F, typename... A>
		struct first_arg_member_fn<R(T::*)(F, A...) volatile> { using type = F; };

		template <typename T, typename R, typename F, typename... A>
		struct first_arg_member_fn<R(T::*)(F, A...) const volatile> { using type = F; };

		template <typename T>
		struct function_type { using type = std::decay<typename remove_member_ptr_fn<decltype(&std::decay_t<T>::operator())>::type>::type; };

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

		template <typename T>
		struct remove_cvref
		{
			using type = std::remove_cvref_t<T>;

			static_assert(!std::is_reference_v<type> && !std::is_const_v<type>);
		};

		template <typename T>
		using remove_cvref_t = remove_cvref<T>::type;

		template <typename T>
		concept does_ret_type_fit_in_eax = std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_pointer_v<T>;

		template <typename T>
		using aligned_storage = std::aligned_storage_t<sizeof(T), alignof(T)>;
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

	struct property_wrapper_base {};

	template <typename S, typename G>
	struct property_wrapper : public property_wrapper_base
	{
		std::string key;

		S s;
		G g;

		template <typename Sx, typename Gx>
		property_wrapper(const std::string& key, Sx&& s, Gx&& g) : key(key), s(std::move(s)), g(std::move(g)) {}
	};

	template <typename S, typename G>
	inline auto property(const std::string& key, S&& s, G&& g)
	{
		return property_wrapper<std::decay_t<S>, std::decay_t<G>>(key, std::forward<S>(s), std::forward<G>(g));
	}

	struct function_wrapper_base {};

	template <typename T>
	struct function_wrapper : public function_wrapper_base
	{
		std::string key;

		T value;

		template <typename T>
		function_wrapper(const std::string& key, T&& v) : key(key), value(std::move(v)) {}
	};

	template <typename T>
	inline auto function(const std::string& key, T&& v)
	{
		return function_wrapper<std::decay_t<T>>(key, std::forward<T>(v));
	}

	struct member_access_fns
	{
		void* write = nullptr,
			* read = nullptr;

		member_access_fns() {}

		template <typename S, typename G>
		member_access_fns(S&& s, G&& g)
		{
			write = std::bit_cast<void*>(std::move(s));
			read = std::bit_cast<void*>(std::move(g));
		}
	};

	class state_info
	{
	private:

		class oop_class
		{
		public:

			std::string name;

		private:

			std::unordered_map<std::string, member_access_fns> fields;
			std::unordered_map<type_info*, void*> functions;

		public:

			member_access_fns* get_field_info(const std::string& key)
			{
				auto it = fields.find(key);
				return it != fields.end() ? &it->second : nullptr;
			}

			void* get_function(type_info* type)
			{
				auto it = functions.find(type);
				return it != functions.end() ? it->second : nullptr;
			}

			template <typename T>
			void add_function(type_info* type, T&& v) { functions[type] = std::bit_cast<void*>(std::move(v)); }

			template <typename S, typename G>
			void add_field(const std::string& key, S&& setter, G&& getter) { fields[key] = member_access_fns(setter, getter); }
		};

		std::unordered_map<type_info*, oop_class> classes;

	public:

		oop_class* add_class(type_info* type) { return &classes[type]; }

		oop_class* get_class(type_info* type)
		{
			auto it = classes.find(type);
			return it != classes.end() ? &it->second : nullptr;
		}

		bool has_class(type_info* type) const { return classes.contains(type); }
	};

	inline std::unordered_map<lua_State*, state_info> states_info;

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

		lua_State* _state = nullptr;

	private:

		template <typename ObjType>
		struct ctor_caller
		{
			template <typename... A, typename... In>
			static void _impl(state& _s, In&&... args) requires (detail::is_empty_args<A...>)
			{
				new (_s.new_userdata<ObjType>()) ObjType(args...);
			}

			template <typename T, typename... A, typename... In>
			static void _impl(state& _s, In&&... args)
			{
				using type = detail::remove_cvref_t<T>;

				type value; _s.pop(value);

				_impl<A...>(_s, std::forward<type>(value), std::forward<In>(args)...);
			}

			template <typename T>
			struct caller { using type = T; };

			template <typename R, typename... A>
			struct caller<R(A...)>
			{
				static void _do(state& _s) { _impl<A...>(_s); }
			};

			template <typename Ctor>
			static void call(state& _s) { caller<Ctor>::_do(_s); }
		};

		template <typename T>
		struct class_fn_caller { using type = T; };

		template <typename R, typename Tx, typename... A>
		struct class_fn_caller<R(__thiscall*)(Tx*, A...)>
		{
			template <typename... Args, typename... In>
			static int _impl(state& _s, void* fn, Tx* _this, In&&... args) requires (detail::is_empty_args<Args>)
			{
				if constexpr (std::is_void_v<R>)
					std::bit_cast<R(__thiscall*)(Tx*, A...)>(fn)(_this, args...);
				else if constexpr (detail::does_ret_type_fit_in_eax<R>)
					return _s.push(std::bit_cast<R(__thiscall*)(Tx*, A...)>(fn)(_this, args...));
				else
				{
					using _R = detail::aligned_storage<R>;

					_R out;

					const auto ret = std::bit_cast<_R* (__thiscall*)(Tx*, _R*, A&&...)>(fn)(_this, &out, std::forward<A>(args)...);

					// the memory for the return value is not allocated so just call the dtor

					const auto casted_ret = std::bit_cast<R*>(ret);

					int c = _s.push(*casted_ret);

					casted_ret->~R();

					return c;
				}

				return 0;
			}

			template <typename T, typename... Args, typename... In>
			static int _impl(state& _s, void* fn, Tx* _this, In&&... args)
			{
				using type = detail::remove_cvref_t<T>;

				type value; _s.pop(value);

				return _impl<Args...>(_s, fn, _this, std::forward<In>(args)..., std::forward<type>(value));
			}

			static int call(state& _s, void* fn)
			{
				variadic_arg_check<A...>();

				Tx* _this; _s.pop(_this);

				return _impl<A...>(_s, fn, _this);
			}
		};

		static constexpr auto OOP_CREATE_FN_NAME() { return "create"; }

		template <typename T = int, typename... A>
		static T _throw_error(lua_State* vm, const std::string& err, const A&... args) { _on_error(vm, FORMATV(err, args...)); return T{}; }

		static int read_only(lua_State* L)
		{
			luaL_error(L, "Property %s is read-only", lua_tostring(L, lua_upvalueindex(1)));
			lua_pushnil(L);
			return 1;
		}

		static int write_only(lua_State* L)
		{
			luaL_error(L, "Property %s is write-only", lua_tostring(L, lua_upvalueindex(1)));
			lua_pushnil(L);
			return 1;
		}

		static int oop_obj_create(lua_State* L)
		{
			state s(L);

			if (!s.is_table(1))
				return 0;

			int stack = s.get_top();

			s.push(OOP_CREATE_FN_NAME());
			s.get_raw(1);

			if (s.is_function(-1))
			{
				for (int i = 2; i <= stack; ++i)
					s.push_value(i);

				s.call_protected(stack - 1, LUA_MULTRET);

				return s.get_top() - stack;
			}

			s.pop_n();

			return 1;
		}

		static int index_function(lua_State* L)
		{
			state s(L);

			s.push_value(s.upvalue_index(1));

			// first we look for a function

			s.push("__class");
			s.get_raw(-2);

			if (!s.is_table(-1))
			{
				s.pop_n();
				return 1;
			}

			s.push_value(2);
			s.get_raw(-2);
			s.remove(-2);

			if (s.is_function(-1))
			{
				// found the function, clean up and return

				s.remove(-2);

				return 1;
			}

			s.pop_n();

			// function not found, look for property

			s.push("__get");
			s.get_raw(-2);

			if (!s.is_table(-1))
			{
				s.pop_n();
				return 1;
			}
			
			s.push_value(2);
			s.get_raw(-2);
			s.remove(-2);

			if (s.is_function(-1))
			{
				// found the property

				s.remove(-2);

				s.push_value(1);	// push field
				s.push_value(2);	// push userdata
				s.call_protected(2, 1);

				return 1;
			}

			s.pop_n();

			return 1;
		}

		static int newindex_function(lua_State* L)
		{
			state s(L);

			s.push_value(s.upvalue_index(1));

			s.push("__set");
			s.get_raw(-2);

			if (!s.is_table(-1))
			{
				s.pop_n();
				return 0;
			}

			s.push_value(2);
			s.get_raw(-2);
			s.remove(-2);

			if (s.is_function(-1))
			{
				// found the property

				s.push_value(3);	// push new value
				s.push_value(1);	// push userdata
				s.push_value(2);	// push field

				s.call_protected(3, 0);

				s.pop_n();

				return 0;
			}

			s.pop_n();

			return 0;
		}

		template <typename T = int, typename... A>
		T throw_error(const std::string& err, A&&... args) const { _on_error(_state, FORMATV(err, args...)); return T {}; }

		void push_fn(const std::string& fn) const
		{
			get_global(fn);

			if (!is_function(-1))
				throw_error("Function {} undefined", fn);
		}

		template <typename Key, typename Value, typename Fn>
		bool iterate_table(const Fn& fn, int i) const
		{
			if (!is_table(i))
				return true;

			const auto _fail = [this]() { pop_n(2); return false; };

			push_nil();

			while (next(i - 1))
			{
				const auto [k, k_ok] = value_from_type<Key>(-2);

				if (!k_ok)
					return _fail();

				const auto [v, v_ok] = value_from_type<Value>(-1);

				if (!v_ok)
					return _fail();

				fn(k, v);

				pop_n();
			}

			return true;
		}

		template <typename T, typename DT = detail::remove_cvref_t<T>>
		int _push(const T& value) const requires(std::is_same_v<DT, variadic_args>);

		template <typename T, typename DT = detail::remove_cvref_t<T>>
		int _push(T&& value) const requires(detail::is_vector<DT> || detail::is_set<DT>)
		{
			push_table();

			for (int i = 1; const auto & v : value)
			{
				push(i++, v);
				set_table(-3);
			}

			return 1;
		}

		template <typename T>
		int _push(T&& value) const requires(detail::is_any_vector<T>)
		{
			check_fatal(custom_stack_pusher, "Custom stack pusher used but not specified");

			int c = 0;

			for (const auto& v : value)
				c += custom_stack_pusher(*this, v);

			return c;
		}

		template <typename T>
		int _push(T&& value) const requires(detail::is_map<T>)
		{
			push_table();

			for (const auto& [k, v] : value)
			{
				push(k, v);
				set_table(-3);
			}

			return 1;
		}

		template <typename T, typename DT = detail::remove_cvref_t<T>>
		int _push(T&& value) const requires(detail::is_bool<DT>) { push_bool(value); return 1; }

		template <typename T, typename DT = detail::remove_cvref_t<T>>
		int _push(T&& value) const requires(detail::is_integer<DT>) { push_int(value); return 1; }

		template <typename T, typename DT = detail::remove_cvref_t<T>>
		int _push(T&& value) const requires(std::is_floating_point_v<DT>) { push_number(value); return 1; }

		template <typename T, typename DT = std::remove_cvref_t<T>>
		int _push(T&& value) const requires(detail::is_string<DT>) { push_string(std::forward<T>(value)); return 1; }

		template <typename T, typename DT = std::remove_cvref_t<T>>
		int _push(T&& value) const
		{
			if constexpr (detail::is_userdata<DT>)
				return push_userdata(value);
			else if constexpr (!std::is_abstract_v<DT>)
				if (const auto state_info = get_info())
					if (const auto class_info = state_info->get_class(TYPEINFO(DT)))
					{
						//static_assert(std::is_constructible_v<DT>, "Return type must have default constructor");

						// create the default object

						const auto ptr = new (new_userdata<DT>()) DT();

						// move the value to the new memory

						*ptr = std::move(value);

						// set class' metatable

						get_class(class_info->name);
						set_metatable(-2);

						return 1;
					}

			return 0;
		}

		template <typename T>
		void _pop(T& value, int& i) const requires(detail::is_bool<T>) { value = to_bool(i++).first; }

		template <typename T>
		void _pop(T& value, int& i) const requires(detail::is_integer<T>) { value = static_cast<T>(to_int(i++).first); }

		template <typename T>
		void _pop(T& value, int& i) const requires(std::is_floating_point_v<T>) { value = static_cast<T>(to_number(i++).first); }

		template <typename T>
		void _pop(T& value, int& i) const requires(detail::is_string<T>) { value = T(to_string(i++).first); }

		template <typename T>
		void _pop(T& value, int& i) const requires(detail::is_userdata<T>) { value = to_userdata<T>(i++); }

		template <typename T>
		void _pop(T& value, int& i) const requires(detail::is_vector<T> || detail::is_set<T>)
		{
			int table_index = 0;

			value.resize(raw_len(i));

			iterate_table<int, typename T::value_type>([&](const auto&, const auto& v)
			{ value[table_index++] = v; }, i);
		}

		template <typename T>
		void _pop(T& value, int& i) const requires(detail::is_map<T>)
		{
			iterate_table<typename T::key_type, typename T::mapped_type>([&](const auto& k, const auto& v)
			{ value[k] = v; }, i);
		}

		template <typename T>
		void _pop(T& value, int& i) const requires(std::is_same_v<T, lua_fn>);

		template <typename T>
		void _pop(T& value, int& i) const requires(std::is_same_v<T, variadic_args>);

		/*
		* fallthrough pop when there is no other pop function
		* that takes T type
		*/
		template <typename T>
		void _pop(T& value, int& i) const
		{
			if (const auto state_info = get_info())
				if (const auto class_info = state_info->get_class(TYPEINFO(T)))
				{
					value = *to_userdata<T*>(i++);

					get_class(class_info->name);
					set_metatable(-2);
				}
		}

		void get_global(const std::string& name) const { lua_getglobal(_state, name.c_str()); }

		bool call_protected(int nargs, int nreturns) const
		{
			if (lua_pcall(_state, nargs, nreturns, 0) == 0)
				return true;

			throw_error(lua_tostring(_state, -1));

			return false;
		}

		void init_oop()
		{
			push_table();
			set_field(LUA_REGISTRYINDEX, "mt");
			get_field(LUA_REGISTRYINDEX, "mt");
			push_table();
			push_c_fn(oop_obj_create);
			set_field(-2, "__call");
			set_field(-2, "Generic");
			pop_n();
		}

		void begin_class()
		{
			push_table();

			push("__class");	push_table();	get_class("Generic"); set_metatable(-2); set_raw(-3);
			push("__get");		push_table();	set_raw(-3);
			push("__set");		push_table();	set_raw(-3);
			push("__index");	push_value(-2); push_c_closure(index_function); set_raw(-3);
			push("__newindex");	push_value(-2); push_c_closure(newindex_function); set_raw(-3);
		}

		void add_class_metamethod(const std::string& method_name, lua_CFunction fn)
		{
			if (!fn)
				return;

			push(method_name);
			push(method_name);
			push_c_closure(fn);
			set_raw(-3);
		}

		void add_class_function(const std::string& fn_name, lua_CFunction fn)
		{
			if (fn)
			{
				push("__class");
				get_raw(-2);

				push(fn_name);
				push(fn_name);
				push_c_closure(fn);
				set_raw(-3);
				pop_n();
			}
		}

		void add_class_variable(const std::string& variable_name, lua_CFunction set, lua_CFunction get)
		{
			push("__set");
			get_raw(-2);

			if (!set)
			{
				push(variable_name);
				push(variable_name);
				push_c_closure(read_only);
				set_raw(-3);
			}
			else
			{
				push(variable_name);
				push(variable_name);
				push_c_closure(set);
				set_raw(-3);
			}

			pop_n();

			push("__get");
			get_raw(-2);

			if (!get)
			{
				push(variable_name);
				push(variable_name);
				push_c_closure(write_only);
				set_raw(-3);
			}
			else
			{
				push(variable_name);
				push(variable_name);
				push_c_closure(get);
				set_raw(-3);
			}

			pop_n();
		}

		void end_class(const std::string& class_name)
		{
			push("mt");
			get_raw(LUA_REGISTRYINDEX);

			// store in registry

			push_value(-2);
			set_field(-2, class_name.c_str());
			pop_n();

			get_field(-1, "__class");
			set_global(class_name.c_str());
			pop_n();
		}

	public:

		state() {}
		state(lua_State* _state) : _state(_state) {}									// mostly for views
		state(lua_State* _state, bool oop) : _state(_state) { if (oop) init_oop(); }	// used by luas::ctx
		~state() { make_invalid(); }

		template <typename T, typename Ctor, typename... A>
		bool register_class(const std::string& name, A&&... args)
		{
			static const auto type_info = TYPEINFO(T);

			auto create = [](lua_State* L)
			{
				state s(L);

				if (const auto class_info = s.get_info()->get_class(type_info))
				{
					ctor_caller<T>::call<Ctor>(s);

					s.get_class(class_info->name);
					s.set_metatable(-2);
				}
				else s.push_nil();

				return 1;
			};

			auto destroy = [](lua_State* L)
			{
				state s(L);

				T* obj; s.pop(obj, -1);

				if (obj)
				{
					// call the dtor manually because the memory will be
					// freed by lua

					obj->~T();

					s.push_bool(true);
				}
				else s.push_bool(false);

				return 1;
			};

			// create state info if it doesn't exist

			const auto state_info = get_info();

			if (state_info->has_class(type_info))
				return false;

			const auto class_info = state_info->add_class(type_info);

			class_info->name = name;

			// start defining the class

			auto iterate_args = []<typename I, typename Ix = std::remove_cvref_t<I>, typename... IA>(const auto& self, state* s, I && v, IA&&... args)
			{
				// register properties

				const auto state_info = s->get_info();

				if constexpr (std::derived_from<Ix, property_wrapper_base>)
				{
					s->add_class_variable(v.key, [](lua_State* L)
					{
						// setter

						state s(L);

						std::string field; s.pop(field);

						if (const auto field_info = s.get_info()->get_class(type_info)->get_field_info(field))
							return class_fn_caller<detail::keep_member_ptr_fn_v<decltype(Ix::s)>>::call(s, field_info->write);

						return s.push_nil();
					}, [](lua_State* L)
					{
						// getter

						state s(L);

						std::string field; s.pop(field);

						if (const auto field_info = s.get_info()->get_class(type_info)->get_field_info(field))
							return class_fn_caller<detail::keep_member_ptr_fn_v<decltype(Ix::g)>>::call(s, field_info->read);

						return s.push_nil();
					});

					state_info->get_class(type_info)->add_field(v.key, v.s, v.g);
				}
				else if constexpr (std::derived_from<Ix, function_wrapper_base>)
				{
					s->add_class_function(v.key, [](lua_State* L)	// method
					{
						state s(L);

						if (const auto fn = s.get_info()->get_class(type_info)->get_function(TYPEINFO(Ix::value)))
							return class_fn_caller<detail::keep_member_ptr_fn_v<decltype(Ix::value)>>::call(s, fn);

						return s.push_nil();
					});

					state_info->get_class(type_info)->add_function(TYPEINFO(Ix::value), v.value);
				}

				if constexpr (sizeof...(IA) > 0)
					self(self, s, std::forward<IA>(args)...);
			};

			begin_class();
			add_class_function("create", create);
			add_class_metamethod("__gc", destroy);

			iterate_args(iterate_args, this, std::forward<A>(args)...);

			end_class(name);

			return true;
		}

		lua_State* get() const { return _state; }
		lua_State* operator * () const { return get(); }

		state_info* get_info() const { return &states_info[_state]; }

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

		void close()
		{
			check_fatal(_state, "Invalid state");

			// remove the state entry from the class member access map

			states_info.erase(_state);

			// close lua state

			lua_close(_state);
		}

		template <typename... A>
		void gc(int what, A&&... args) { lua_gc(_state, what, args...); }
		void make_invalid() { _state = nullptr; }
		void open_libs() const { luaL_openlibs(_state); }
		void set_global(const char* index) const { lua_setglobal(_state, index); }
		void set_table(int i) const { lua_settable(_state, i); }
		void unref(int v) const { luaL_unref(_state, LUA_REGISTRYINDEX, v); }
		void push_bool(bool v) const { lua_pushboolean(_state, v); }
		void push_int(lua_Integer v) const { lua_pushinteger(_state, v); }
		void push_number(lua_Number v) const { lua_pushnumber(_state, v); }
		void push_string(const std::string& v) const { lua_pushstring(_state, v.c_str()); }
		void push_table() const { lua_newtable(_state); }
		void set_metatable(int i) const { lua_setmetatable(_state, i); }
		void set_raw(int i) const { lua_rawset(_state, i); }
		void set_field(int i, const char* k) { lua_setfield(_state, i, k); }
		void push_c_fn(lua_CFunction fn) { lua_pushcfunction(_state, fn); }
		void remove(int i) const { lua_remove(_state, i); }
		void get_class(const std::string& class_name) const
		{
			push("mt");
			get_raw(LUA_REGISTRYINDEX);

			check_fatal(is_table(-1), "get_class expected a table");

			push(class_name);
			get_raw(-2);
			remove(-2);
		}

		void pop_n(int n = 1) const { lua_pop(_state, n); }

		void exec_string(const std::string_view& code) const
		{
			if (luaL_dostring(_state, code.data()) != 0)
				throw_error(lua_tostring(_state, -1));
		}

		int push() const { return 0; }

		template <typename T, typename... A>
		int push(T&& value, A&&... args) const
		{
			auto c = _push(std::forward<T>(value));
			return c + push(std::forward<A>(args)...);
		}

		template <typename T>
		T get_global_var(const std::string& name) const
		{
			get_global(name);

			T value;

			pop<T>(value);

			return value;
		}

		template <typename T>
		void pop(T& out, int i = -1) const
		{
			_pop(out, i);
			pop_n();
		}

		template <typename T>
		int pop_read(T& out, int i = -1) const
		{
			_pop(out, i);
			return i;
		}

		int push_userdata(void* v) const { if (v) lua_pushlightuserdata(_state, v); else push_nil(); return 1; }
		int push_nil() const { lua_pushnil(_state); return 1; }
		int push_value(int i) const { lua_pushvalue(_state, i); return 1; }
		int get_top() const { return lua_gettop(_state); }
		int get_field(int i, const char* k) const { return lua_getfield(_state, i, k); }
		int ref() const { return luaL_ref(_state, LUA_REGISTRYINDEX); }
		int get_raw(int i, int n) const { return lua_rawgeti(_state, i, n); }
		int get_raw(int i) const { return lua_rawget(_state, i); }
		int upvalue_index(int i) const { return lua_upvalueindex(i); }
		int next(int i) const { return lua_next(_state, i); }

		lua_Unsigned raw_len(int i) const { return lua_rawlen(_state, i); }

		bool is_table(int i) const { return lua_istable(_state, i); }
		bool is_function(int i) const { return lua_isfunction(_state, i); }
		bool is_nil(int i) const { return lua_isnil(_state, i); }

		value_ok<bool> to_bool(int i) const
		{
			if (!lua_isboolean(_state, i))
				return { throw_error<bool>("Expected 'bool' value, got '{}'", LUA_GET_TYPENAME(i)), false };

			return { !!lua_toboolean(_state, i), true };
		}

		template <typename T = int>
		value_ok<T> to_int(int i) const
		{
			if (!lua_isinteger(_state, i))
				return { throw_error<T>("Expected 'integer' value, got '{}'", LUA_GET_TYPENAME(i)), false };

			return { static_cast<T>(lua_tointeger(_state, i)), true };
		}

		template <typename T = float>
		value_ok<T> to_number(int i) const
		{
			if (!lua_isnumber(_state, i))
				return { throw_error<T>("Expected 'number' value, got '{}'", LUA_GET_TYPENAME(i)), false };

			return { static_cast<T>(lua_tonumber(_state, i)), true };
		}

		value_ok<std::string> to_string(int i) const
		{
			if (!lua_isstring(_state, i))
				return { throw_error<std::string>("Expected 'string' value, got '{}'", LUA_GET_TYPENAME(i)), false };

			return { lua_tostring(_state, i), true };
		}

		template <typename T = void*>
		T to_userdata(int i) const
		{
			if (is_nil(i))
				return nullptr;

			if (!lua_isuserdata(_state, i) && !lua_islightuserdata(_state, i))
				return throw_error<T>("Expected '{}' value, got '{}'", typeid(T).name(), LUA_GET_TYPENAME(i));

			return reinterpret_cast<T>(lua_touserdata(_state, i));
		}

		template <typename T>
		constexpr value_ok<T> value_from_type(int i) const
		{
			if constexpr (detail::is_bool<T>)				return to_bool(i);
			else if constexpr (detail::is_integer<T>)		return to_int<T>(i);
			else if constexpr (std::is_floating_point_v<T>) return to_number<T>(i);
			else if constexpr (detail::is_string<T>)		return to_string(i);
			else if constexpr (detail::is_userdata<T>)
			{
				if (const auto ud = to_userdata<T>(i))
					return { ud, true };
			}

			return { {}, false };
		}

		template <typename T>
		void push_c_closure(T&& value, int n = 1) { lua_pushcclosure(_state, value, n); }

		template <typename T>
		T* new_userdata() const { return static_cast<T*>(lua_newuserdata(_state, sizeof(T))); }

		template <typename... A>
		bool call_safe_fn(const std::string& fn, int nreturns = 0, A&&... args) const
		{
			push_fn(fn);

			return call_protected(push(args...), nreturns);
		}

		template <typename... A>
		bool call_safe(int nreturns = 0, A&&... args) const
		{
			return call_protected(push(args...), nreturns);
		}

		bool call_safe(int nreturns, const variadic_args& va) const;
	};

	class variadic_args
	{
	private:

		state vm {};

		// storage for values indices

		int first = 0,
			last = 0,
			current = 0;

		mutable int stack_offset = 0;

	public:

		variadic_args() {}
		variadic_args(const state* vm, int first, int last) : vm(vm->get()), first(first), last(last), current(first) {}
		~variadic_args() { vm.make_invalid(); }

		void set_stack_offset(int v) const { stack_offset = v; }

		int begin() const { return first + stack_offset; }
		int end() const { return last + stack_offset; }

		template <typename T>
		T get(int i) const { return vm.value_from_type<T>(begin() + i).first; }

		int get_type(int i) const { return lua_type(*vm, begin() + i); }

		int size() const { return (end() - begin()) + 1; }

		int push_all() const
		{
			const auto _begin = begin();

			// we push the same index because we are doing it in reverse
			// order so everytime we push, the same index will be the next
			// parameter

			int c = 0;

			for (int i = _begin; i <= end(); ++i)
				c += vm.push_value(_begin);

			return c;
		}
	};

	template <typename... A>
	static constexpr void variadic_arg_check()
	{
		if constexpr (detail::is_type_in_variadics<variadic_args, A...>::value())
			static_assert(detail::is_type_last_in_variadics<variadic_args, A...>::value(), "variadic_args must appear at the end");
	}

	template <typename Fn>
	struct lua_c_caller
	{
		template <typename... A, typename... In>
		static int _impl(state& _s, int nargs, [[maybe_unused]] int i, In&&... args) requires (detail::is_empty_args<A>)
		{
			using return_type = detail::fn_return_type_v<Fn>;

			const auto pop_args = [&]() { if (nargs > 0) _s.pop_n(nargs); };
			const auto fn = *_s.to_userdata<Fn*>(_s.upvalue_index(1));

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
					tuple::for_each(ret, [&]<typename T>(T&& item) { _s.push(item); });

					return std::tuple_size_v<return_type>;
				}
				else if constexpr (!std::is_void_v<return_type>)
					return _s.push(ret);
			}

			return 0;
		}

		template <typename T, typename... A, typename... In>
		static int _impl(state& _s, int nargs, int i, In&&... args)
		{
			using type = detail::remove_cvref_t<T>;

			type value;
			
			return _impl<A...>(_s, nargs, _s.pop_read(value, i), std::forward<In>(args)..., std::forward<type>(value));
		}

		template <typename T>
		struct caller { using type = T; };

		template <typename R, typename... A>
		struct caller<R(*)(A...)>
		{
			static int _do(state& _s, int i) { variadic_arg_check<A...>(); return _impl<A...>(_s, -i, i); }
		};

		template <typename R, typename... A>
		struct caller<R(*)(state&, A...)>
		{
			static int _do(state& _s, int i) { variadic_arg_check<A...>(); return _impl<A...>(_s, -i, i, _s); }
		};

		static int call(state& _s, int nargs) { return caller<Fn>::_do(_s, -nargs); }
	};

	struct lua_fn
	{
		state vm {};

		int ref = 0;

		lua_fn(const state* _vm) : vm(_vm->get()) { ref = vm.ref(); }
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
		void call(A&&... args) const requires(detail::is_empty_args<T...>)
		{
			vm.get_raw(LUA_REGISTRYINDEX, ref);
			vm.call_safe(0, args...);
		}

		template <typename... T, typename... A>
		std::tuple<T...> call(A&&... args) const
		{
			std::tuple<T...> out {};

			vm.get_raw(LUA_REGISTRYINDEX, ref);

			if (vm.call_safe(sizeof...(T), args...))
				tuple::for_each_reverse(out, [&]<typename T>(T& item) { item = vm.pop<T>(); });

			return out;
		}
	};

	inline bool state::call_safe(int nreturns, const variadic_args& va) const
	{
		// set the stack offset to -1 because we pushed the function before

		va.set_stack_offset(-1);
		
		// push all variadic arguments & call function

		return call_protected(push(va), nreturns);
	}

	template <typename T, typename DT>
	int state::_push(const T& value) const requires(std::is_same_v<DT, variadic_args>)
	{
		return value.push_all();
	}

	template <typename T>
	void state::_pop(T& value, int& i) const requires(std::is_same_v<T, lua_fn>)
	{
		const auto index = i++;

		if (is_function(index))
		{
			push_value(index);

			value = lua_fn(this);
		}
	}

	template <typename T>
	void state::_pop(T& value, int& i) const requires(std::is_same_v<T, variadic_args>)
	{
		// since variadic arguments are the last, set the index to -1 which is
		// where the first argument was pushed

		value = variadic_args(this, std::exchange(i, -1), -1);
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
			const auto function = [](lua_State* L) -> int
			{
				state s(L);

				return lua_c_caller<T>::call(s, s.get_top());
			};

			const auto fn_obj_loc = vm->new_userdata<T>();

			check_fatal(fn_obj_loc, "Could not allocate placeholder for function");

			new (fn_obj_loc) T(std::move(fn));

			vm->push_c_closure(function);
			vm->set_global(index);
		}

	public:

		ctx(bool oop = false)
		{
			vm = new state(luaL_newstate(), oop);

			check_fatal(vm, "Could not allocate vm");

			vm->set_panic();
			vm->open_libs();
			vm->gc(LUA_GCGEN, 20, 100);
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

		state* get() const { return vm; }

		lua_State* get_lua_state() const { return vm->get(); }

		void exec_string(const std::string& code) { vm->exec_string(code); }

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
		void add_global(const char* index, T&& value)
		{
			vm->push(value);
			vm->set_global(index);
		}

		template <typename T, typename Ctor, typename... A>
		bool register_class(const std::string& name, A&&... args)
		{
			return vm->register_class<T, Ctor, A...>(name, std::forward<A>(args)...);
		}
	};
};