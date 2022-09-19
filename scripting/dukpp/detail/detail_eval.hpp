# pragma once

#include <duktape/duktape.h>

namespace dukpp::detail {
    template<typename RetT>
    void read(duk_context *ctx, duk_idx_t arg_idx, RetT *out) {
        // ArgStorage has some static_asserts in it that validate value types,
        // so we typedef it to force ArgStorage<RetType> to compile and run the asserts
        typedef typename types::ArgStorage<RetT>::type ValidateReturnType;

        *out = types::DukType<typename types::Bare<RetT>::type>::template read<RetT>(ctx, arg_idx);
    }

    template<typename RetT>
    struct SafeEvalData {
        char const *str;
        RetT *out;
    };

    template<typename RetT>
    duk_ret_t eval_safe(duk_context *ctx, void *udata) {
        auto data = (SafeEvalData<RetT> *) udata;

        duk_eval_string(ctx, data->str);
        read(ctx, -1, data->out);
        return 1;
    }
}
