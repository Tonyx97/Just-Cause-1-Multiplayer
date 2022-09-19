#pragma once

#include <duktape.h>

#include "detail/detail_traits.hpp"  // for index_tuple/make_indexes
#include "dukexception.hpp"
#include "context.hpp"

// This file has some useful utility functions for users.
// Hopefully this saves you from wading through the implementation.

namespace dukpp {
    /**
     * WARNING: THIS IS NOT "PROTECTED." If an error occurs while reading (which is possible if you didn't
     *          explicitly check the type), the fatal Duktape error handler will be invoked, and the program
     *          will probably abort.
     */
    template<typename RetT>
    void read(duk_context *ctx, duk_idx_t arg_idx, RetT *out) {
        // ArgStorage has some static_asserts in it that validate value types,
        // so we typedef it to force ArgStorage<RetType> to compile and run the asserts
        typedef typename types::ArgStorage<RetT>::type ValidateReturnType;

        *out = types::DukType<typename types::Bare<RetT>::type>::template read<RetT>(ctx, arg_idx);
    }


// methods

    // leaves return value on stack
    template<typename ObjT, typename... ArgTs>
    void call_method(context_view &ctx, ObjT const &obj, char const *method_name, ArgTs... args) {
        auto rctx = ctx.get_duk_context();
        ctx.push(obj);
        duk_get_prop_string(rctx, -1, method_name);

        if (duk_check_type(rctx, -1, DUK_TYPE_UNDEFINED)) {
            duk_error(rctx, DUK_ERR_REFERENCE_ERROR, "Method does not exist", method_name);
            return;
        }

        if (!duk_is_callable(rctx, -1)) {
            duk_error(rctx, DUK_ERR_TYPE_ERROR, "Property is not callable");
            return;
        }

        duk_swap_top(rctx, -2);
        ctx.push(args...);
        duk_call_method(rctx, sizeof...(args));
    }


    namespace detail {
        template<typename RetT, typename ObjT, typename... ArgTs>
        struct SafeMethodCallData {
            ObjT const *obj;
            char const *method_name;
            std::tuple<ArgTs...> args;
            RetT *out;
        };

        template<typename ObjT, typename... ArgTs, size_t... Indexes>
        void call_method_safe_helper(context_view &ctx, ObjT const &obj, char const *method_name,
                                     std::tuple<ArgTs...> &tup, index_tuple<Indexes...> indexes) {
            call_method(ctx, obj, method_name, std::forward<ArgTs>(std::get<Indexes>(tup))...);
        }

        template<typename RetT, typename ObjT, typename... ArgTs>
        typename std::enable_if<std::is_void<RetT>::value, duk_idx_t>::type
        call_method_safe(context_view &ctx, void *udata) {
            typedef SafeMethodCallData<RetT, ObjT, ArgTs...> DataT;
            DataT *data = (DataT *) udata;
            call_method_safe_helper(ctx, *(data->obj), data->method_name, data->args,
                                    typename make_indexes<ArgTs...>::type());
            return 1;
        }

        template<typename RetT, typename ObjT, typename... ArgTs>
        typename std::enable_if<!std::is_void<RetT>::value, duk_idx_t>::type
        call_method_safe(context_view &ctx, void *udata) {
            typedef SafeMethodCallData<RetT, ObjT, ArgTs...> DataT;
            auto data = (DataT *) udata;

            call_method_safe_helper(ctx, *(data->obj), data->method_name, data->args,
                                    typename make_indexes<ArgTs...>::type());
            data->out = ctx.read<ObjT>();
            return 1;
        }
    }


    template<typename RetT, typename ObjT, typename... ArgTs>
    typename std::enable_if<std::is_void<RetT>::value, RetT>::type
    pcall_method(duk_context *ctx, ObjT const &obj, char const *method_name, ArgTs... args) {
        detail::SafeMethodCallData<RetT, ObjT, ArgTs...> data{
                &obj, method_name, std::tuple<ArgTs...>(args...), nullptr
        };

        auto rc = duk_safe_call(ctx, &detail::call_method_safe<RetT, ObjT, ArgTs...>, (void *) &data, 0, 1);
        if (rc != 0) {
            throw duk_error_exception(ctx, rc);
        }

        duk_pop(ctx);  // remove result from stack
    }

    template<typename RetT, typename ObjT, typename... ArgTs>
    typename std::enable_if<!std::is_void<RetT>::value, RetT>::type
    pcall_method(duk_context *ctx, ObjT const &obj, char const *method_name, ArgTs... args) {
        RetT out;
        detail::SafeMethodCallData<RetT, ObjT, ArgTs...> data{
                &obj, method_name, std::tuple<ArgTs...>(args...), &out
        };

        auto rc = duk_safe_call(ctx, &detail::call_method_safe<RetT, ObjT, ArgTs...>, (void *) &data, 0, 1);
        if (rc != 0) {
            throw duk_error_exception(ctx, rc);
        }

        duk_pop(ctx);  // remove result from stack
        return std::move(out);
    }

// calls

    // leaves return value on the stack
    template<typename ObjT, typename... ArgTs>
    void call(context_view &ctx, ObjT const &func, ArgTs... args) {
        auto rctx = ctx.get_duk_context();
        ctx.push(func);
        if (!duk_is_callable(rctx, -1)) {
            duk_pop(rctx);
            duk_error(rctx, DUK_ERR_TYPE_ERROR, "Object is not callable");
            return;
        }

        ctx.push(args...);
        duk_call(rctx, sizeof...(args));
    }


// safe call

    namespace detail {
        template<typename RetT, typename ObjT, typename... ArgTs>
        struct SafeCallData {
            ObjT const *obj;
            std::tuple<ArgTs...> args;
            RetT *out;
        };

        template<typename ObjT, typename... ArgTs, size_t... Indexes>
        void call_safe_helper(dcontext_view &ctx, ObjT const &obj, std::tuple<ArgTs...> &tup,
                              index_tuple<Indexes...> indexes) {
            call(ctx, obj, std::forward<ArgTs>(std::get<Indexes>(tup))...);
        }

        // leaves result on stack
        template<typename RetT, typename ObjT, typename... ArgTs>
        typename std::enable_if<std::is_void<RetT>::value, duk_ret_t>::type
        call_safe(context_view &ctx, void *udata) {
            typedef SafeCallData<RetT, ObjT, ArgTs...> DataT;
            auto data = (DataT *) udata;

            call_safe_helper(ctx, *(data->obj), data->args, typename make_indexes<ArgTs...>::type());
            return 1;
        }

        // leaves result on stack
        // The result is read into RetT here because it can potentially trigger an error (with duk_error).
        // If we did it "above" this function, that error would trigger a panic instead of error handling.
        template<typename RetT, typename ObjT, typename... ArgTs>
        typename std::enable_if<!std::is_void<RetT>::value, duk_ret_t>::type
        call_safe(context_view &ctx, void *udata) {
            typedef SafeCallData<RetT, ObjT, ArgTs...> DataT;
            auto data = (DataT *) udata;

            call_safe_helper(ctx, *(data->obj), data->args, typename make_indexes<ArgTs...>::type());
            data->out = ctx.read<ObjT>();
            return 1;
        }

    }


    // Unlike duktape, this will remove the return value from the stack!
    template<typename RetT, typename ObjT, typename... ArgTs>
    typename std::enable_if<std::is_void<RetT>::value, RetT>::type
    pcall(context_view &ctx, ObjT const &obj, ArgTs... args) {
        auto rctx = ctx.get_duk_context();
        detail::SafeCallData<RetT, ObjT, ArgTs...> data{
                &obj, std::tuple<ArgTs...>(args...), nullptr
        };

        duk_int_t rc = duk_safe_call(rctx, &detail::call_safe<RetT, ObjT, ArgTs...>, (void *) &data, 0, 1);
        if (rc != 0) {
            throw duk_error_exception(rctx, rc);
        }
        duk_pop(rctx);  // remove result from stack
    }

    template<typename RetT, typename ObjT, typename... ArgTs>
    typename std::enable_if<!std::is_void<RetT>::value, RetT>::type
    pcall(context_view &ctx, ObjT const &obj, ArgTs... args) {
        auto rctx = ctx.get_duk_context();
        RetT result;
        detail::SafeCallData<RetT, ObjT, ArgTs...> data{
                &obj, std::tuple<ArgTs...>(args...), &result
        };

        duk_int_t rc = duk_safe_call(rctx, &detail::call_safe<RetT, ObjT, ArgTs...>, (void *) &data, 0, 1);
        if (rc != 0) {
            throw duk_error_exception(rctx, rc);
        }

        duk_pop(rctx);  // remove result from stack
        return std::move(result);
    }

    // same as dukpp::pcall, but leaves the result or error on the stack and returns the Duktape return code
    template<typename ObjT, typename... ArgTs>
    duk_int_t pcall_raw(context_view &ctx, ObjT const &obj, ArgTs... args) {
        auto rctx = ctx.get_duk_context();
        detail::SafeCallData<void, ObjT, ArgTs...> data{
                &obj, std::tuple<ArgTs...>(args...), nullptr
        };

        return duk_safe_call(rctx, &detail::call_safe<void, ObjT, ArgTs...>, (void *) &data, 0, 1);
    }
}
