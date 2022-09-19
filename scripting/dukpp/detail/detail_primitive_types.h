#pragma once

#include <vector>
#include <map>
#include <cstdint>
#include <memory>

#include <duktape/duktape.h>
#include <optional>

#include "detail_typeinfo.hpp"
#include "../duk_value.hpp"
#include "../variadic_args.hpp"
#include "../this_context.hpp"

namespace dukpp::types {

#define DUKPP_SIMPLE_OPT_VALUE_TYPE(TYPE, DUK_IS_FUNC, DUK_GET_FUNC, DUK_PUSH_FUNC, PUSH_VALUE)      \
        template<>                                                                                   \
            struct DukType<std::optional<TYPE>> {                                                    \
                typedef std::true_type IsValueType;                                                  \
                                                                                                     \
                template<typename FullT>                                                             \
                static std::optional<TYPE> read(duk_context *ctx, duk_idx_t arg_idx) {               \
                    if (DUK_IS_FUNC(ctx, arg_idx) || duk_is_null(ctx, arg_idx)) {                    \
                        return static_cast<TYPE>(DUK_GET_FUNC(ctx, arg_idx));                        \
                    }                                                                                \
                    else {                                                                           \
                        return std::nullopt;                                                         \
                    }                                                                                \
                }                                                                                    \
                                                                                                     \
                template<typename FullT>                                                             \
                static void push(duk_context *ctx, std::optional<TYPE> const &value) {               \
                    if (value.has_value()) {                                                         \
                        DUK_PUSH_FUNC(ctx, PUSH_VALUE);                                              \
                    } else {                                                                         \
                        duk_push_null(ctx);                                                          \
                    }                                                                                \
                }                                                                                    \
            };
#define DUKPP_SIMPLE_VALUE_TYPE(TYPE, DUK_IS_FUNC, DUK_GET_FUNC, DUK_PUSH_FUNC, PUSH_VALUE, PUSH_OPT)       \
        template<>                                                                                          \
        struct DukType<TYPE> {                                                                              \
            typedef std::true_type IsValueType;                                                             \
                                                                                                            \
            template<typename FullT>                                                                        \
            static TYPE read(duk_context* ctx, duk_idx_t arg_idx) {                                         \
                if (DUK_IS_FUNC(ctx, arg_idx)) {                                                            \
                    return static_cast<TYPE>(DUK_GET_FUNC(ctx, arg_idx));                                   \
                } else {                                                                                    \
                    duk_int_t type_idx = duk_get_type(ctx, arg_idx);                                        \
                    duk_error(ctx, DUK_RET_TYPE_ERROR, "Argument %d: expected " #TYPE ", got %s", arg_idx,  \
                              detail::get_type_name(type_idx));                                             \
                    if (std::is_constructible<TYPE>::value) {                                               \
                        return TYPE();                                                                      \
                    }                                                                                       \
                }                                                                                           \
            }                                                                                               \
                                                                                                            \
            template<typename FullT>                                                                        \
            static void push(duk_context* ctx, TYPE value) {                                                \
                DUK_PUSH_FUNC(ctx, PUSH_VALUE);                                                             \
            }                                                                                               \
        };                                                                                                  \
    DUKPP_SIMPLE_OPT_VALUE_TYPE(TYPE, DUK_IS_FUNC, DUK_GET_FUNC, DUK_PUSH_FUNC, PUSH_OPT)

    DUKPP_SIMPLE_VALUE_TYPE(bool, duk_is_boolean, 0 != duk_get_boolean, duk_push_boolean, value, value.value())

    DUKPP_SIMPLE_VALUE_TYPE(uint8_t, duk_is_number, duk_get_uint, duk_push_uint, value, value.value())
    DUKPP_SIMPLE_VALUE_TYPE(uint16_t, duk_is_number, duk_get_uint, duk_push_uint, value, value.value())
    DUKPP_SIMPLE_VALUE_TYPE(uint32_t, duk_is_number, duk_get_uint, duk_push_uint, value, value.value())
    DUKPP_SIMPLE_VALUE_TYPE(uint64_t, duk_is_number, duk_get_number, duk_push_number, value,
                            value.value()) // have to cast to double

    DUKPP_SIMPLE_VALUE_TYPE(int8_t, duk_is_number, duk_get_int, duk_push_int, value, value.value())
    DUKPP_SIMPLE_VALUE_TYPE(int16_t, duk_is_number, duk_get_int, duk_push_int, value, value.value())
    DUKPP_SIMPLE_VALUE_TYPE(int32_t, duk_is_number, duk_get_int, duk_push_int, value, value.value())
    DUKPP_SIMPLE_VALUE_TYPE(int64_t, duk_is_number, duk_get_number, duk_push_number, value,
                            value.value()) // have to cast to double

#ifdef __APPLE__
    DUKPP_SIMPLE_VALUE_TYPE(size_t, duk_is_number, duk_get_number, duk_push_number, value, value.value())
    DUKPP_SIMPLE_VALUE_TYPE(time_t, duk_is_number, duk_get_number, duk_push_number, value, value.value())
#elif defined(__arm__)
    DUKPP_SIMPLE_VALUE_TYPE(time_t, duk_is_number, duk_get_uint, duk_push_uint, value, value.value())
#endif

    // signed char and unsigned char are surprisingly *both* different from char, at least in MSVC
    DUKPP_SIMPLE_VALUE_TYPE(char, duk_is_number, duk_get_int, duk_push_int, value, value.value())

    DUKPP_SIMPLE_VALUE_TYPE(float, duk_is_number, duk_get_number, duk_push_number, value, value.value())
    DUKPP_SIMPLE_VALUE_TYPE(double, duk_is_number, duk_get_number, duk_push_number, value, value.value())

    DUKPP_SIMPLE_VALUE_TYPE(std::string, duk_is_string, duk_get_string, duk_push_string, value.c_str(),
                            value.value().c_str())

#undef DUKPP_SIMPLE_VALUE_TYPE

    // We have to do some magic for char const * to work correctly.
    // We override the "bare type" and "storage type" to both be char const *.
    // char* is a bit tricky because its "bare type" should still be char const *, to differentiate it from just char
    template<>
    struct Bare<char *> {
        typedef char const *type;
    };
    template<>
    struct Bare<char const *> {
        typedef char const *type;
    };

    // the storage type should also be char const * - if we don't do this, it will end up as just "char"
    template<>
    struct ArgStorage<char const *> {
        typedef char const *type;
    };

    template<>
    struct DukType<char const *> {
        typedef std::true_type IsValueType;

        template<typename FullT>
        static char const *read(duk_context *ctx, duk_idx_t arg_idx) {
            if (duk_is_string(ctx, arg_idx)) {
                return duk_get_string(ctx, arg_idx);
            } else {
                duk_int_t type_idx = duk_get_type(ctx, arg_idx);
                duk_error(ctx, DUK_RET_TYPE_ERROR, "Argument %d: expected string, got %s", arg_idx,
                          detail::get_type_name(type_idx));
                return nullptr;
            }
        }

        template<typename FullT>
        static void push(duk_context *ctx, char const *value) {
            duk_push_string(ctx, value);
        }
    };

    template<>
    struct DukType<std::optional<char const *>> {
        typedef std::true_type IsValueType;

        template<typename FullT>
        static std::optional<char const *> read(duk_context *ctx, duk_idx_t arg_idx) {
            if (duk_is_string(ctx, arg_idx) || duk_is_null(ctx, arg_idx)) {
                return duk_get_string(ctx, arg_idx);
            } else {
                return std::nullopt;
            }
        }

        template<typename FullT>
        static void push(duk_context *ctx, std::optional<char const *> value) {
            if (value.has_value()) {
                duk_push_string(ctx, value.value());
            } else {
                duk_push_null(ctx);
            }
        }
    };

    // duk_value
    template<>
    struct DukType<duk_value> {
        typedef std::true_type IsValueType;

        template<typename FullT>
        static duk_value read(duk_context *ctx, duk_idx_t arg_idx) {
            try {
                return duk_value::copy_from_stack(ctx, arg_idx);
            } catch (duk_exception &e) {
                // only duk_exception can be thrown by duk_value::copy_from_stack
                duk_error(ctx, DUK_ERR_ERROR, e.what());
                return duk_value();
            }
        }

        template<typename FullT>
        static void push(duk_context *ctx, duk_value const &value) {
            if (value.context() == nullptr) {
                duk_error(ctx, DUK_ERR_ERROR, "duk_value is uninitialized");
                return;
            }

            if (value.context() != ctx) {
                duk_error(ctx, DUK_ERR_ERROR, "duk_value comes from a different context");
                return;
            }

            try {
                value.push();
            } catch (duk_exception &e) {
                // only duk_exception can be thrown by duk_value::copy_from_stack
                duk_error(ctx, DUK_ERR_ERROR, e.what());
            }
        }
    };

    // std::vector (as value)
    // TODO - probably leaks memory if duktape is using longjmp and an error is encountered while reading an element
    template<typename T>
    struct DukType<std::vector<T> > {
        typedef std::true_type IsValueType;

        template<typename FullT>
        static std::vector<T> read(duk_context *ctx, duk_idx_t arg_idx) {
            if (!duk_is_array(ctx, arg_idx)) {
                duk_int_t type_idx = duk_get_type(ctx, arg_idx);
                duk_error(ctx, DUK_ERR_TYPE_ERROR, "Argument %d: expected array, got %s", arg_idx,
                          detail::get_type_name(type_idx));
            }

            duk_size_t len = duk_get_length(ctx, arg_idx);
            const duk_idx_t elem_idx = duk_get_top(ctx);

            std::vector<T> vec;
            vec.reserve(len);
            for (duk_size_t i = 0; i < len; i++) {
                duk_get_prop_index(ctx, arg_idx, i);
                vec.push_back(DukType<typename Bare<T>::type>::template read<T>(ctx, elem_idx));
                duk_pop(ctx);
            }
            return vec;
        }

        template<typename FullT>
        static void push(duk_context *ctx, std::vector<T> const &value) {
            duk_idx_t obj_idx = duk_push_array(ctx);

            for (size_t i = 0; i < value.size(); i++) {
                DukType<typename Bare<T>::type>::template push<T>(ctx, value[i]);
                duk_put_prop_index(ctx, obj_idx, i);
            }
        }
    };

    // std::shared_ptr (as value)
    template<typename T>
    struct DukType<std::shared_ptr<T> > {
        typedef std::true_type IsValueType;

        static_assert(std::is_same<typename DukType<T>::IsValueType, std::false_type>::value,
                      "Dukglue can only use std::shared_ptr to non-value types!");

        template<typename FullT>
        static std::shared_ptr<T> read(duk_context *ctx, duk_idx_t arg_idx) {
            if (duk_is_null(ctx, arg_idx)) {
                return nullptr;
            }

            if (!duk_is_object(ctx, arg_idx)) {
                duk_int_t type_idx = duk_get_type(ctx, arg_idx);
                duk_error(ctx, DUK_RET_TYPE_ERROR, "Argument %d: expected shared_ptr object, got ", arg_idx,
                          detail::get_type_name(type_idx));
            }

            duk_get_prop_string(ctx, arg_idx, DUK_HIDDEN_SYMBOL("type_info"));
            if (!duk_is_pointer(ctx, -1)) { // missing type_info, must not be a native object
                duk_error(ctx, DUK_RET_TYPE_ERROR, "Argument %d: expected shared_ptr object (missing type_info)",
                          arg_idx);
            }

            // make sure this object can be safely returned as a T*
            auto info = static_cast<detail::TypeInfo *>(duk_get_pointer(ctx, -1));
            if (!info->can_cast<T>()) {
                duk_error(ctx, DUK_RET_TYPE_ERROR, "Argument %d: wrong type of shared_ptr object", arg_idx);
            }
            duk_pop(ctx);  // pop type_info

            duk_get_prop_string(ctx, arg_idx, DUK_HIDDEN_SYMBOL("shared_ptr"));
            if (!duk_is_pointer(ctx, -1)) {
                duk_error(ctx, DUK_RET_TYPE_ERROR, "Argument %d: not a shared_ptr object (missing shared_ptr)",
                          arg_idx);
            }
            void *ptr = duk_get_pointer(ctx, -1);
            duk_pop(ctx);  // pop pointer to shared_ptr

            return *((std::shared_ptr<T> *) ptr);
        }

        static duk_ret_t shared_ptr_finalizer(duk_context *ctx) {
            duk_get_prop_string(ctx, 0, DUK_HIDDEN_SYMBOL("shared_ptr"));
            auto ptr = (std::shared_ptr<T> *) duk_require_pointer(ctx, -1);
            duk_pop(ctx);  // pop shared_ptr ptr

            if (ptr != NULL) {
                delete ptr;

                // for safety, set the pointer to undefined
                // (finalizers can run multiple times)
                duk_push_undefined(ctx);
                duk_put_prop_string(ctx, 0, DUK_HIDDEN_SYMBOL("shared_ptr"));
            }

            return 0;
        }

        template<typename FullT>
        static void push(duk_context *ctx, std::shared_ptr<T> const &value) {
            detail::ProtoManager::make_script_object(ctx, value.get());

            // create + set shared_ptr
            duk_push_pointer(ctx, new std::shared_ptr<T>(value));
            duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("shared_ptr"));

            // set shared_ptr finalizer
            duk_push_c_function(ctx, &shared_ptr_finalizer, 1);
            duk_set_finalizer(ctx, -2);
        }
    };

    // std::map (as value)
    // TODO - probably leaks memory if duktape is using longjmp and an error is encountered while reading values
    template<typename T>
    struct DukType<std::map<std::string, T> > {
        typedef std::true_type IsValueType;

        template<typename FullT>
        static std::map<std::string, T> read(duk_context *ctx, duk_idx_t arg_idx) {
            if (!duk_is_object(ctx, arg_idx)) {
                duk_error(ctx, DUK_ERR_TYPE_ERROR, "Argument %d: expected object.", arg_idx);
            }

            std::map<std::string, T> map;
            duk_enum(ctx, -1, DUK_ENUM_OWN_PROPERTIES_ONLY);
            while (duk_next(ctx, -1, 1)) {
                map[duk_safe_to_string(ctx, -2)] = DukType<typename Bare<T>::type>::template read<T>(ctx, -1);
                duk_pop_2(ctx);
            }
            duk_pop(ctx);  // pop enum object
            return map;
        }

        template<typename FullT>
        static void push(duk_context *ctx, std::map<std::string, T> const &value) {
            duk_idx_t obj_idx = duk_push_object(ctx);
            for (auto const &kv : value) {
                DukType<typename Bare<T>::type>::template push<T>(ctx, kv.second);
                duk_put_prop_lstring(ctx, obj_idx, kv.first.data(), kv.first.size());
            }
        }
    };

    template<>
    struct DukType<variadic_args> {
        typedef std::true_type IsValueType;

        template<typename FullT>
        static variadic_args read(duk_context *ctx, duk_idx_t arg_idx) {
            duk_idx_t nargs = duk_get_top(ctx);

            variadic_args vargs;
            vargs.args.resize(nargs);
            for (duk_idx_t i = 0; i < nargs; i++) {
                vargs[nargs - 1 - i] = duk_value::copy_from_stack(ctx);
                duk_pop(ctx);
            }
            return vargs;
        }

        template<typename FullT>
        static void push(duk_context *ctx, variadic_args const &value) {
            for (auto const &v : value.args) {
                if (v.context() == nullptr) {
                    duk_error(ctx, DUK_ERR_ERROR, "duk_value is uninitialized");
                }

                if (v.context() != ctx) {
                    duk_error(ctx, DUK_ERR_ERROR, "duk_value comes from a different context");
                }

                try {
                    v.push();
                } catch (duk_exception &e) {
                    // only duk_exception can be thrown by duk_value::copy_from_stack
                    duk_error(ctx, DUK_ERR_ERROR, e.what());
                }
            }
        }
    };

    template<>
    struct DukType<this_context> {
        typedef std::true_type IsValueType;

        template<typename FullT>
        static this_context read(duk_context *ctx, duk_idx_t arg_idx) {
            return ctx;
        }

        template<typename FullT>
        static void push(duk_context *ctx, this_context const &value) {
            // static_assert(false, "Invalid operation");
        }
    };

    // std::function
    /*template <typename RetT, typename... ArgTs>
    struct DukType< std::function<RetT(ArgTs...)> > {
        typedef std::true_type IsValueType;

        template<typename FullT>
        static std::function<RetT(ArgTs...)> read(duk_context* ctx, duk_idx_t arg_idx) {
            duk_value callable = duk_value::copy_from_stack(ctx, -1, DUK_TYPE_MASK_OBJECT);
            return [ctx, callable] (ArgTs... args) -> RetT {
                dukpp::call<RetT>(ctx, callable, args...);
            };
        }

        template<typename FullT>
        static void push(duk_context* ctx, std::function<RetT(ArgTs...)> value) {
            static_assert(false, "Pushing an std::function has not been implemented yet. Sorry!");
        }
    };*/
}
