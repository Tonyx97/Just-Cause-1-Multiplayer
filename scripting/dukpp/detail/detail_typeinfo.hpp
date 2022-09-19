#pragma once

#include <typeindex>

#include <duktape/duktape.h>

namespace dukpp::detail {
    // same as duk_get_type_name, which is private for some reason *shakes fist*
    inline char const *get_type_name(duk_int_t type_idx) noexcept {
        static char const *names[] = {
                "none",
                "undefined",
                "null",
                "boolean",
                "number",
                "string",
                "object",
                "buffer",
                "pointer",
                "lightfunc"
        };

        if (type_idx >= 0 && type_idx < (duk_int_t) (sizeof(names) / sizeof(names[0]))) {
            return names[type_idx];
        } else {
            return "unknown";
        }
    }

    class TypeInfo {
    public:
        TypeInfo(std::type_index &&idx) noexcept: index_(idx), base_(nullptr) {}

        TypeInfo(TypeInfo const &rhs) noexcept: index_(rhs.index_), base_(rhs.base_) {}

        inline void set_base(TypeInfo *base) noexcept {
            base_ = base;
        }

        template<typename T>
        [[nodiscard]] bool can_cast() const noexcept {
            if (index_ == typeid(T)) {
                return true;
            }

            if (base_) {
                return base_->can_cast<T>();
            }

            return false;
        }

        inline bool operator<(TypeInfo const &rhs) const noexcept { return index_ < rhs.index_; }

        inline bool operator<=(TypeInfo const &rhs) const noexcept { return index_ <= rhs.index_; }

        inline bool operator>(TypeInfo const &rhs) const noexcept { return index_ > rhs.index_; }

        inline bool operator>=(TypeInfo const &rhs) const noexcept { return index_ >= rhs.index_; }

        inline bool operator==(TypeInfo const &rhs) const noexcept { return index_ == rhs.index_; }

        inline bool operator!=(TypeInfo const &rhs) const noexcept { return index_ != rhs.index_; }

    private:
        std::type_index index_;
        TypeInfo *base_;
    };
}
