#pragma once

#include <duktape/duktape.h>

#include "detail_stack.hpp"

namespace dukpp::detail {
    // This struct can be used to generate a Duktape C function that
    // pulls the argument values off the stack (with type checking),
    // calls the appropriate function with them, and puts the function's
    // return value (if any) onto the stack.
    template<typename RetType, typename... Ts>
    struct FuncInfoHolder {
        typedef RetType(*FuncType)(Ts...);

        struct FuncRuntime {
            // Pull the address of the function to call from the
            // Duktape function object at run time.
            static duk_ret_t call_native_function(duk_context *ctx) {
                duk_push_current_function(ctx);
                duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("func_ptr"));
                void *fp_void = duk_require_pointer(ctx, -1);
                if (fp_void == nullptr) {
                    duk_error(ctx, DUK_RET_TYPE_ERROR, "what even");
                    return DUK_RET_TYPE_ERROR;
                }

                duk_pop_2(ctx);

                static_assert(sizeof(RetType(*)(Ts...)) == sizeof(void *),
                              "Function pointer and data pointer are different sizes");
                auto funcToCall = reinterpret_cast<RetType(*)(Ts...)>(fp_void);

                actually_call(ctx, funcToCall, detail::get_stack_values<Ts...>(ctx));
                return std::is_void<RetType>::value ? 0 : 1;
            }

            static duk_ret_t call_native_ctx_function(duk_context *ctx) {
                duk_push_current_function(ctx);
                duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("func_ptr"));
                void *fp_void = duk_require_pointer(ctx, -1);
                if (fp_void == nullptr) {
                    duk_error(ctx, DUK_RET_TYPE_ERROR, "what even");
                    return DUK_RET_TYPE_ERROR;
                }

                duk_pop_2(ctx);

                static_assert(sizeof(RetType(*)(Ts...)) == sizeof(void *),
                              "Function pointer and data pointer are different sizes");
                auto funcToCall = reinterpret_cast<RetType(*)(Ts...)>(fp_void);

                actually_call_ctx(ctx, funcToCall, detail::get_stack_values<Ts...>(ctx));
                return std::is_void<RetType>::value ? 0 : 1;
            }

            template<typename Dummy = RetType, typename... BakedTs>
            static typename std::enable_if<std::is_void<Dummy>::value>::type
            actually_call(duk_context *, RetType(*funcToCall)(Ts...), std::tuple<BakedTs...> const &args) {
                detail::apply_fp(funcToCall, args);
            }

            template<typename Dummy = RetType, typename... BakedTs>
            static typename std::enable_if<std::is_void<Dummy>::value>::type
            actually_call_ctx(duk_context *ctx, RetType(*funcToCall)(Ts...), std::tuple<BakedTs...> const &args) {
                detail::apply_fp_ctx(ctx, funcToCall, args);
            }

            template<typename Dummy = RetType, typename... BakedTs>
            static typename std::enable_if<!std::is_void<Dummy>::value>::type
            actually_call(duk_context *ctx, RetType(*funcToCall)(Ts...), std::tuple<BakedTs...> const &args) {
                // ArgStorage has some static_asserts in it that validate value types,
                // so we typedef it to force ArgStorage<RetType> to compile and run the asserts
                typedef typename types::ArgStorage<RetType>::type ValidateReturnType;

                RetType return_val = detail::apply_fp(funcToCall, args);

                types::DukType<typename types::Bare<RetType>::type>::template push<RetType>(ctx, std::move(return_val));
            }

            template<typename Dummy = RetType, typename... BakedTs>
            static typename std::enable_if<!std::is_void<Dummy>::value>::type
            actually_call_ctx(duk_context *ctx, RetType(*funcToCall)(Ts...), std::tuple<BakedTs...> const &args) {
                // ArgStorage has some static_asserts in it that validate value types,
                // so we typedef it to force ArgStorage<RetType> to compile and run the asserts
                typedef typename types::ArgStorage<RetType>::type ValidateReturnType;

                RetType return_val = detail::apply_fp_ctx(ctx, funcToCall, args);

                types::DukType<typename types::Bare<RetType>::type>::template push<RetType>(ctx, std::move(return_val));
            }
        };
    };
}
