# pragma once

#include <string_view>
#include <vector>
#include <fstream>
#include <type_traits>

#include <duktape/duktape.h>

#include "detail/detail_function.hpp"
#include "detail/detail_eval.hpp"
#include "this_context.hpp"
#include "variadic_args.hpp"
#include "class.hpp"

namespace dukpp {

    template<typename T>
    struct return_type : return_type<decltype(&T::operator())> {
    };
    // For generic types, directly use the result of the signature of its 'operator()'
    template<typename ClassType, typename ReturnType, typename... Args>
    struct return_type<ReturnType(ClassType::*)(Args...) const> {
        using type = ReturnType;
    };

    struct context_view {

        struct proxy {
            context_view *mCtx;
            std::string idx;

            proxy(context_view *ctx, std::string_view idx)
                    : mCtx(ctx), idx(idx) {}

            // Register a function.

            template<typename RetType, typename... Ts>
            proxy &operator=(RetType (*funcToCall)(Ts...)) {
                // TODO: need more smart logic to provide this_context
                auto evalFunc = detail::FuncInfoHolder<RetType, Ts...>::FuncRuntime::call_native_function;
                mCtx->actually_register_global_function<false>(idx.data(), evalFunc, funcToCall);
                return *this;
            }

            template<typename RetType, typename... Ts>
            proxy &operator=(RetType (*funcToCall)(this_context, Ts...)) {
                // TODO: need more smart logic to provide this_context
                auto evalFunc = detail::FuncInfoHolder<RetType, Ts...>::FuncRuntime::call_native_ctx_function;
                mCtx->actually_register_global_function<true>(idx.data(), evalFunc, funcToCall);
                return *this;
            }

            // register a global object
            // auto myDog = new Dog("Vector");
            // ctx["dog"] = myDog;
            template<typename T>
            proxy &operator=(T const &obj) {
                mCtx->push(obj);
                duk_put_global_string(mCtx->get_duk_context(), idx.data());
                return *this;
            }

            proxy operator[](std::string_view idx) {
                if (!duk_has_prop_string(mCtx->mCtx, -1, this->idx.c_str()))
                    duk_put_prop_string(mCtx->mCtx, -1, this->idx.c_str());
                else
                    duk_get_prop_string(mCtx->mCtx, -1, this->idx.c_str());
                return proxy(mCtx, idx);
            }
        };

        friend struct proxy;

        duk_context *mCtx;

        context_view(duk_context *ctx) noexcept: mCtx(ctx) {}

        context_view(this_context *ctx) noexcept: mCtx(ctx->mCtx) {}

        [[nodiscard]] duk_context *get_duk_context() const noexcept {
            return mCtx;
        }

        proxy operator[](std::string_view idx) {
            duk_push_global_object(mCtx);
            return proxy(this, idx);
        }

        template<typename RetT = void>
        typename std::enable_if<std::is_void<RetT>::value, RetT>::type eval(std::string_view const &code) const {
            int prev_top = duk_get_top(mCtx);
            duk_eval_string(mCtx, code.data());
            duk_pop_n(mCtx, duk_get_top(mCtx) - prev_top);  // pop any results
        }

        template<typename RetT>
        typename std::enable_if<!std::is_void<RetT>::value, RetT>::type eval(std::string_view const &code) const {
            int prev_top = duk_get_top(mCtx);
            duk_eval_string(mCtx, code.data());
            auto out = read<RetT>();
            duk_pop_n(mCtx, duk_get_top(mCtx) - prev_top);  // pop any results
            return out;
        }


        template<typename RetT = void>
        typename std::enable_if<std::is_void<RetT>::value, RetT>::type peval(std::string_view const &code) const {
            int prev_top = duk_get_top(mCtx);
            int rc = duk_peval_string(mCtx, code.data());
            if (rc != 0) {
                throw duk_error_exception(mCtx, rc);
            }

            duk_pop_n(mCtx, duk_get_top(mCtx) - prev_top);  // pop any results
        }

        template<typename RetT>
        typename std::enable_if<!std::is_void<RetT>::value, RetT>::type peval(std::string_view const &code) const {
            int prev_top = duk_get_top(mCtx);

            RetT ret;
            detail::SafeEvalData<RetT> data{
                    code.data(), &ret
            };

            int rc = duk_safe_call(mCtx, &detail::eval_safe<RetT>, (void *) &data, 0, 1);
            if (rc != 0) {
                throw duk_error_exception(mCtx, rc);
            }
            duk_pop_n(mCtx, duk_get_top(mCtx) - prev_top);  // pop any results
            return ret;
        }

        void eval_file(std::string const &filename) const {
            std::ifstream file(filename);
            file.seekg(0, std::ios::end);
            std::vector<char> code(static_cast<size_t>(file.tellg()));
            file.seekg(0, std::ios::beg);
            file.read(&code[0], code.size());
            duk_eval_string(mCtx, code.data());
        }

        template<typename RetT = void, typename... Ts>
        typename std::enable_if<std::is_void<RetT>::value, RetT>::type call(std::string const &fn, Ts ...args) const {
            duk_get_global_string(mCtx, fn.data());
            push(args...);
            duk_call(mCtx, sizeof...(args));
        }

        template<typename RetT, typename... Ts>
        typename std::enable_if<!std::is_void<RetT>::value, RetT>::type call(std::string const &fn, Ts ...args) const {
            int prev_top = duk_get_top(mCtx);
            call(fn, args...);
            auto val = read<RetT>();
            int cur_top = duk_get_top(mCtx);
            duk_pop_n(mCtx, cur_top - prev_top);
            return val;
        }

        [[nodiscard]] duk_value copy_value_from_stack(duk_idx_t idx = -1) const {
            return duk_value::copy_from_stack(mCtx, idx);
        }

        [[nodiscard]] duk_value take_value_from_stack(duk_idx_t idx = -1) const {
            return duk_value::take_from_stack(mCtx, idx);
        }

        template<typename Cls>
        unmanaged_class<Cls> register_unmanaged_class(char const *name) const {
            return unmanaged_class<Cls>(mCtx, name);
        }

        template<typename Cls>
        managed_class<Cls> register_class(char const *name) const {
            return managed_class<Cls>(mCtx, name);
        }

        /**
         * WARNING: THIS IS NOT "PROTECTED." If an error occurs while reading (which is possible if you didn't
         *          explicitly check the type), the fatal Duktape error handler will be invoked, and the program
         *          will probably abort.
         */
        template<typename RetT>
        RetT read(duk_idx_t arg_idx = -1) const {
            // ArgStorage has some static_asserts in it that validate value types,
            // so we typedef it to force ArgStorage<RetType> to compile and run the asserts
            typedef typename types::ArgStorage<RetT>::type ValidateReturnType;

            return std::move(types::DukType<typename types::Bare<RetT>::type>::template read<RetT>(mCtx, arg_idx));
        }

        /**
         * @brief      Push a value onto the duktape stack.
         *
         * WARNING: THIS IS NOT "PROTECTED." If an error occurs when pushing (unlikely, but possible),
         *          the Duktape fatal error handler will be invoked (and the program will probably terminate).
         *
         * @param      ctx    duktape context
         * @param[in]  val    value to push
         */
        template<typename FullT>
        void push(FullT const &val) const {
            // ArgStorage has some static_asserts in it that validate value types,
            // so we typedef it to force ArgStorage<RetType> to compile and run the asserts
            typedef typename types::ArgStorage<FullT>::type ValidateReturnType;

            types::DukType<typename types::Bare<FullT>::type>::template push<FullT>(mCtx, std::move(val));
        }

        template<typename T, typename... ArgTs>
        void push(T const &arg, ArgTs... args) const {
            push(mCtx, arg);
            push(mCtx, args...);
        }

        inline void push() const {
            // no-op
        }

    private:
        template<bool isCtxFirstArg, typename RetType, typename... Ts>
        void actually_register_global_function(char const *name, duk_c_function const &evalFunc,
                                               RetType (*funcToCall)(Ts...)) const noexcept {
            duk_idx_t nargs = sizeof...(Ts);

            if constexpr (detail::contains<this_context, Ts...>()) {
                static_assert(std::is_same<this_context, typename detail::last_type<Ts...>::type>() || isCtxFirstArg,
                              "this_context must be first or last argument");
                nargs -= 1;
            }

            if constexpr (detail::contains<variadic_args, Ts...>() ||
                          detail::contains<variadic_args const &, Ts...>() ||
                          detail::contains<variadic_args const &&, Ts...>()) {
                nargs = DUK_VARARGS;
            }

            duk_push_c_function(mCtx, evalFunc, nargs);

            static_assert(sizeof(RetType(*)(Ts...)) == sizeof(void *),
                          "Function pointer and data pointer are different sizes");
            duk_push_pointer(mCtx, reinterpret_cast<void *>(funcToCall));
            duk_put_prop_string(mCtx, -2, DUK_HIDDEN_SYMBOL("func_ptr"));

            duk_put_prop_string(mCtx, -2, name);
        }
    };

    struct context : context_view {
        context() noexcept: context_view(duk_create_heap_default()) {}

        ~context() noexcept {
            duk_destroy_heap(mCtx);
        }
    };
}
