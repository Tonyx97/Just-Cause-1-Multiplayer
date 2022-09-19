# pragma once

#include <iomanip>
#include "duk_value.hpp"
#include "detail/detail_types.hpp"

namespace dukpp {
    // Prints duk_value to out stream
    inline std::ostream &operator<<(std::ostream &os, duk_value const &value) noexcept {
        switch (value.type()) {
            case duk_value::Type::Undefined:
            case duk_value::Type::NullRef:
                os << value.type_name();
                break;
            case duk_value::Type::Boolean: {
                std::ios state(nullptr);
                state.copyfmt(os);
                os << std::boolalpha << value.as_bool();
                os.copyfmt(state);
                break;
            }
            case duk_value::Type::Number:
                os << value.as_double();
                break;
            case duk_value::Type::String:
                os << value.as_string();
                break;
            case duk_value::Type::Pointer: {
                std::ios state(nullptr);
                state.copyfmt(os);
                os << "[pointer:" << std::hex << "0x" << value.as_pointer() << ']';
                os.copyfmt(state);
                break;
            }
            case duk_value::Type::Object: {
                auto ctx = value.context();
                int prev_top = duk_get_top(ctx);
                value.push();
                if (duk_is_array(ctx, -1)) {
                    os << "[array: [";
                    auto arr = value.as_array();
                    for (auto it = arr.begin(); it != arr.end(); ++it) {
                        os << *it;
                        if (std::next(it) != arr.end()) {
                            os << ", ";
                        }
                    }
                    os << "]]";
                } else if (duk_is_function(ctx, -1)) {
                    os << '[' << "function" << ": " << duk_safe_to_string(ctx, -1) << "]";
                } else if (duk_is_object(ctx, -1)) {
                    os << "[object: {";
                    auto map = value.as_map();
                    for (auto it = map.begin(); it != map.end(); ++it) {
                        os << it->first << ": " << it->second;
                        if (std::next(it) != map.end()) {
                            os << ", ";
                        }
                    }
                    os << "}]";
                } else {
                    os << '[' << value.type_name() << ": not implemented]";
                }
                duk_pop_n(ctx, duk_get_top(ctx) - prev_top);  // pop any results
                break;
            }
            case duk_value::Type::Buffer: {
                duk_size_t buf_size;
                duk_value::BufferType buf_type;
                auto buf = value.as_plain_buffer(&buf_size, &buf_type);
                std::stringstream ss;
                os << "[plain ";
                switch (buf_type) {
                    case duk_value::BufferType::Fixed:
                        os << "fixed";
                        break;
                    case duk_value::BufferType::Dynamic:
                        os << "dynamic";
                        break;
                    case duk_value::BufferType::External:
                        os << "external";
                        break;
                    case duk_value::BufferType::Undefined:
                        os << "undefined";
                        break;
                }
                os << " buffer: ";
                for (duk_size_t i = 0; i < buf_size; ++i) {
                    ss << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (uint32_t) buf[i];
                    if (i + 1 != buf_size) {
                        ss << ", ";
                    }
                }
                os << ss.str() << "]";
                break;
            }
            case duk_value::Type::LightFunc:
                os << '[' << value.type_name() << ": not implemented]";
        }
        return os;
    }
}
