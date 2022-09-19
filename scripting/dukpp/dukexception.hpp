#pragma once

#include <sstream>
#include <exception>

#include <duktape/duktape.h>

namespace dukpp {
    inline duk_ret_t throw_error(duk_context *ctx) {
        duk_error(ctx, DUK_ERR_TYPE_ERROR, "Property does not have getter or setter.");
        return DUK_RET_TYPE_ERROR;
    }

    class duk_exception : public std::exception {
    public:
        virtual char const *what() const noexcept override {
            return mMsg.c_str();
        }

        template<typename T>
        duk_exception &operator<<(T rhs) {
            std::stringstream ss;
            ss << mMsg << rhs;
            mMsg = ss.str();
            return *this;
        }

    protected:
        std::string mMsg;
    };

    class duk_error_exception : public duk_exception {
    public:
        duk_error_exception(duk_context *ctx, int return_code, bool pop_error = true) {
            if (return_code != 0) {
                duk_get_prop_string(ctx, -1, "stack");
                mMsg = duk_safe_to_string(ctx, -1);
                duk_pop(ctx);

                if (pop_error) {
                    duk_pop(ctx);
                }
            }
        }
    };
}
