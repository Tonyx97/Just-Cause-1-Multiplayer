# pragma once

#include <duktape/duktape.h>

namespace dukpp {
    struct this_context {
        duk_context *mCtx;

        this_context(duk_context *ctx) noexcept: mCtx(ctx) {}

        operator duk_context *() const noexcept { return duk_context(); }

        duk_context *operator->() const noexcept { return duk_context(); }

        duk_context *duk_context() const noexcept { return mCtx; }
    };
}
