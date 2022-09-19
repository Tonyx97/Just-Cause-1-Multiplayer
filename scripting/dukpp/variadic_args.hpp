# pragma once

#include <vector>

#include <duktape/duktape.h>

#include "duk_value.hpp"

namespace dukpp {
    struct variadic_args {
        typedef std::vector<duk_value> va_t;
        va_t args;

        va_t::iterator begin() noexcept { return args.begin(); }

        [[nodiscard]] va_t::const_iterator begin() const noexcept { return args.begin(); }

        va_t::iterator end() noexcept { return args.end(); }

        [[nodiscard]] va_t::const_iterator end() const noexcept { return args.end(); }

        duk_value &operator[](va_t::size_type const pos) noexcept { return args[pos]; }

        void push(duk_value &&value) noexcept { args.push_back(value); }

        void push(duk_value const &&value) noexcept { args.push_back(value); }
    };
}
