//
// Created by Stanislav "Koncord" Zhukov on 18.01.2021.
//

# pragma once

#include "detail_types.hpp"

#include "../duk_value.hpp"

namespace dukpp {

    duk_value &duk_value::operator=(const duk_value &rhs) {
        if (this == &rhs) {
            throw duk_exception() << "Self-assignment is prohibited";
        }
        // free whatever we had
        release_ref_count();

        // copy things
        mContext = rhs.mContext;
        mType = rhs.mType;
        mPOD = rhs.mPOD;

        if (mType == Type::String) {
            mString = rhs.mString;
        } else if (mType == Type::Object) {
            // ref counting increment
            if (rhs.mRefCount == nullptr) {
                // not ref counted before, need to allocate memory
                const_cast<duk_value &>(rhs).mRefCount = new int(2);
                mRefCount = rhs.mRefCount;
            } else {
                // already refcounting, just increment
                mRefCount = rhs.mRefCount;
                *mRefCount = *mRefCount + 1;
            }
        } else if (mType == Type::Buffer) {
            mBufSize = rhs.mBufSize;
            mBufferType = rhs.mBufferType;
        }

        return *this;
    }

    bool duk_value::operator==(const duk_value &rhs) const {
        if (mType != rhs.mType || mContext != rhs.mContext) {
            return false;
        }

        switch (mType) {
            case Type::Undefined:
            case Type::NullRef:
                return true;
            case Type::Boolean:
                return mPOD.boolean == rhs.mPOD.boolean;
            case Type::Number:
                return mPOD.number == rhs.mPOD.number;
            case Type::String:
                return mString == rhs.mString;
            case Type::Object: {
                // todo: this could be optimized to only push ref_array once...
                this->push();
                rhs.push();
                bool equal = duk_equals(mContext, -1, -2) != 0;
                duk_pop_2(mContext);
                return equal;
            }
            case Type::Pointer:
                return mPOD.pointer == rhs.mPOD.pointer;
            case Type::Buffer:
            case Type::LightFunc:
            default:
                throw duk_exception() << "operator== not implemented (" << type_name() << ")";
        }
    }

    const char *duk_value::type_name() const noexcept {
        switch (mType) {
            case Type::Undefined:
                return "undefined";
            case Type::NullRef:
                return "null";
            case Type::Boolean:
                return "boolean";
            case Type::Number:
                return "number";
            case Type::String:
                return "string";
            case Type::Object:
                return "object";
            case Type::Buffer:
                return "buffer";
            case Type::Pointer:
                return "pointer";
            case Type::LightFunc:
                return "lightfunc";
        }
        return "?";
    }

    duk_value duk_value::copy_from_stack(duk_context *ctx, duk_idx_t idx) {
        duk_value value;
        value.mContext = ctx;
        value.mType = (Type) duk_get_type(ctx, idx);
        switch (value.mType) {
            case Type::Undefined:
                break;
            case Type::NullRef:
                value.mPOD.pointer = nullptr;
                break;
            case Type::Boolean:
                value.mPOD.boolean = duk_require_boolean(ctx, idx) != 0;
                break;
            case Type::Number:
                value.mPOD.number = duk_require_number(ctx, idx);
                break;
            case Type::String: {
                duk_size_t len;
                char const *data = duk_get_lstring(ctx, idx, &len);
                value.mString.assign(data, len);
                break;
            }
            case Type::Object:
                value.mPOD.ref_array_idx = stash_ref(ctx, idx);
                break;
            case Type::Pointer:
                value.mPOD.pointer = duk_require_pointer(ctx, idx);
                break;
            case Type::Buffer: {
                if (duk_is_fixed_buffer(ctx, idx)) {
                    value.mBufferType = BufferType::Fixed;
                } else if (duk_is_dynamic_buffer(ctx, idx)) {
                    value.mBufferType = BufferType::Dynamic;
                } else if (duk_is_external_buffer(ctx, idx)) {
                    value.mBufferType = BufferType::External;
                }
                value.mPOD.plain_buffer = (uint8_t *) duk_require_buffer(ctx, idx, &value.mBufSize);
                break;
            }
            case Type::LightFunc:
            default:
                throw duk_exception() << "Cannot turn type into duk_value (" << value.type_name() << ")";
        }

        return value;
    }

    void duk_value::push() const {

        switch (mType) {
            case Type::Undefined:
                duk_push_undefined(mContext);
                break;
            case Type::NullRef:
                duk_push_null(mContext);
                break;
            case Type::Boolean:
                duk_push_boolean(mContext, mPOD.boolean);
                break;
            case Type::Number:
                duk_push_number(mContext, mPOD.number);
                break;
            case Type::String:
                duk_push_lstring(mContext, mString.data(), mString.size());
                break;
            case Type::Object:
                push_ref_array(mContext);
                duk_get_prop_index(mContext, -1, mPOD.ref_array_idx);
                duk_remove(mContext, -2);
                break;
            case Type::Pointer:
                duk_push_pointer(mContext, mPOD.pointer);
                break;
            case Type::Buffer: {
                // todo: Maybe deep copy would be better?
                duk_push_external_buffer(mContext);
                duk_config_buffer(mContext, -1, mPOD.plain_buffer, mBufSize); // map existing buffer to new pointer
                break;
            }
            case Type::LightFunc:
            default:
                throw duk_exception() << "duk_value.push() not implemented for type (" << type_name() << ")";
        }
    }

    std::vector<duk_value> duk_value::as_array() const {
        std::vector<duk_value> vec;

        if (mType != Type::Object) {
            throw duk_exception() << "Expected array, got " << type_name();
        }

        int prev_top = duk_get_top(mContext);

        push();

        if (!duk_is_array(mContext, -1)) {
            duk_pop_n(mContext, duk_get_top(mContext) - prev_top);
            throw duk_exception() << "Expected array, got " << type_name();
        }

        duk_size_t len = duk_get_length(mContext, -1);
        const duk_idx_t elem_idx = duk_get_top(mContext);

        vec.reserve(len);

        for (duk_size_t i = 0; i < len; i++) {
            duk_get_prop_index(mContext, -1, i);
            vec.push_back(types::DukType<typename types::Bare<duk_value>::type>
                          ::template read<duk_value>(mContext, elem_idx));
            duk_pop(mContext);
        }

        duk_pop_n(mContext, duk_get_top(mContext) - prev_top);  // pop any results

        return vec;
    }

    std::map<std::string, duk_value> duk_value::as_map() const {
        std::map<std::string, duk_value> map;

        if (mType != Type::Object) {
            throw duk_exception() << "Expected array, got " << type_name();
        }

        int prev_top = duk_get_top(mContext);

        push();

        if (!duk_is_object(mContext, -1)) {
            duk_pop_n(mContext, duk_get_top(mContext) - prev_top);
            throw duk_exception() << "Expected array, got " << type_name();
        }

        duk_enum(mContext, -1, DUK_ENUM_OWN_PROPERTIES_ONLY);
        while (duk_next(mContext, -1, 1)) {
            map[duk_safe_to_string(mContext, -2)] = types::DukType<typename types::Bare<duk_value>::type>
            ::template read<duk_value>(mContext, -1);
            duk_pop_2(mContext);
        }

        duk_pop_n(mContext, duk_get_top(mContext) - prev_top);  // pop any results
        return map;
    }

    duk_value duk_value::deserialize(duk_context *ctx, const char *data, size_t data_len) {
        duk_value v;
        v.mContext = ctx;
        v.mType = *((Type *) data);

        char const *data_ptr = data + sizeof(Type);
        data_len -= sizeof(Type);

        switch (v.mType) {
            case Type::Undefined:
            case Type::NullRef:
                break;

            case Type::Boolean: {
                if (data_len < 1) {
                    throw duk_exception() << "Malformed boolean data";
                }

                v.mPOD.boolean = data[1] == 1;
                break;
            }

            case Type::Number: {
                if (data_len < sizeof(double)) {
                    throw duk_exception() << "Malformed number data";
                }

                v.mPOD.number = *((double *) data_ptr);
                break;
            }

            case Type::String: {
                if (data_len < sizeof(uint32_t)) {
                    throw duk_exception() << "Malformed string data (no length)";
                }
                uint32_t str_len = *((uint32_t *) data_ptr);

                if (data_len < sizeof(uint32_t) + str_len) {
                    throw duk_exception() << "Malformed string data (appears truncated)";
                }

                char const *str_data = (data_ptr + sizeof(uint32_t));
                v.mString.assign(str_data, str_len);
                break;
            }

            case Type::Object: {
                if (data_len < sizeof(uint32_t)) {
                    throw duk_exception() << "Malformed object JSON data (no length)";
                }
                uint32_t json_len = *((uint32_t *) data_ptr);

                if (data_len < sizeof(uint32_t) + json_len) {
                    throw duk_exception() << "Malformed object JSON data (appears truncated)";
                }

                char const *json_data = (data_ptr + sizeof(uint32_t));
                duk_push_lstring(ctx, json_data, json_len);
                int rc = duk_safe_call(ctx, &json_decode_safe, nullptr, 1, 1);
                if (rc) {
                    throw duk_error_exception(ctx, rc) << "Could not decode JSON";
                } else {
                    v.mPOD.ref_array_idx = stash_ref(ctx, -1);
                    duk_pop(ctx);
                }
                break;
            }

            default:
                throw duk_exception() << "not implemented";
        }

        return v;
    }

    std::vector<char> duk_value::serialize() const {
        std::vector<char> buff;
        buff.resize(sizeof(Type));
        *((Type *) buff.data()) = mType;

        switch (mType) {
            case Type::Undefined:
            case Type::NullRef:
                break;

            case Type::Boolean: {
                buff.push_back(mPOD.boolean ? 1 : 0);
                break;
            }

            case Type::Number: {
                buff.resize(buff.size() + sizeof(double));
                *((double *) (buff.data() + sizeof(Type))) = mPOD.number;
                break;
            }

            case Type::String: {
                if (mString.length() > static_cast<size_t>(UINT32_MAX)) {
                    throw duk_exception() << "String length larger than uint32_t max";
                }

                auto len = (uint32_t) mString.length();
                buff.resize(buff.size() + sizeof(uint32_t) + len);

                auto len_ptr = (uint32_t *) (buff.data() + sizeof(Type));
                *len_ptr = len;

                auto out_ptr = (char *) (buff.data() + sizeof(Type) + sizeof(uint32_t));
                strncpy(out_ptr, mString.data(), len);  // note: this will NOT be null-terminated
                break;
            }

            case Type::Object: {
                push();
                if (duk_is_function(mContext, -1)) {
                    duk_pop(mContext);
                    throw duk_exception() << "Functions cannot be serialized";
                    // well, technically they can...see the comments at the start of this method
                }

                std::string json = duk_json_encode(mContext, -1);
                duk_pop(mContext);

                if (json.length() > static_cast<size_t>(UINT32_MAX)) {
                    throw duk_exception() << "JSON length larger than uint32_t max";
                }

                uint32_t len = (uint32_t) json.length();
                buff.resize(buff.size() + sizeof(uint32_t) + len);

                auto len_ptr = (uint32_t *) (buff.data() + sizeof(Type));
                *len_ptr = len;

                char *out_ptr = (char *) (buff.data() + sizeof(Type) + sizeof(uint32_t));
                strncpy(out_ptr, json.data(), len);  // note: this will NOT be null-terminated
                break;
            }

            default:
                throw duk_exception() << "Type not implemented for serialization.";
        }

        return buff;
    }

    void duk_value::push_ref_array(duk_context *ctx) noexcept {
        duk_push_heap_stash(ctx);

        if (!duk_has_prop_string(ctx, -1, "dukpp_duk_value_refs")) {
            duk_push_array(ctx);

            // ref_array[0] = 0 (initialize free list as empty)
            duk_push_int(ctx, 0);
            duk_put_prop_index(ctx, -2, 0);

            duk_put_prop_string(ctx, -2, "dukpp_duk_value_refs");
        }

        duk_get_prop_string(ctx, -1, "dukpp_duk_value_refs");
        duk_remove(ctx, -2); // pop heap stash
    }

    duk_uint_t duk_value::stash_ref(duk_context *ctx, duk_idx_t idx) noexcept {
        push_ref_array(ctx);

        // if idx is relative, we need to adjust it to deal with the array we just pushed
        if (idx < 0) {
            idx--;
        }

        // find next free index
        // free indices are kept in a linked list, starting at ref_array[0]
        duk_get_prop_index(ctx, -1, 0);
        duk_uarridx_t next_free_idx = duk_get_uint(ctx, -1);
        duk_pop(ctx);

        if (next_free_idx == 0) {
            // no free spots in the array, make a new one at arr.length
            next_free_idx = (duk_uarridx_t) duk_get_length(ctx, -1);
        } else {
            // free spot found, need to remove it from the free list
            // ref_array[0] = ref_array[next_free_idx]
            duk_get_prop_index(ctx, -1, next_free_idx);
            duk_put_prop_index(ctx, -2, 0);
        }

        duk_dup(ctx, idx);  // copy value we are storing (since store consumes it)
        duk_put_prop_index(ctx, -2, next_free_idx);  // store it (consumes duplicated value)
        duk_pop(ctx);  // pop ref array

        return next_free_idx;
    }

    void duk_value::free_ref(duk_context *ctx, duk_uarridx_t ref_array_idx) noexcept {
        push_ref_array(ctx);

        // add this spot to the free list
        // refs[old_obj_idx] = refs[0] (implicitly gives up our reference)
        duk_get_prop_index(ctx, -1, 0);
        duk_put_prop_index(ctx, -2, ref_array_idx);

        // refs[0] = old_obj_idx
        duk_push_uint(ctx, ref_array_idx);
        duk_put_prop_index(ctx, -2, 0);

        duk_pop(ctx);  // pop ref array
    }

    void duk_value::release_ref_count() noexcept {
        if (mType == Type::Object) {
            if (mRefCount != nullptr) {
                // sharing with another duk_value, are we the only one left?
                if (*mRefCount > 1) {  // still someone else referencing this
                    *mRefCount = *mRefCount - 1;
                } else {
                    // not sharing anymore, we can free it
                    free_ref(mContext, mPOD.ref_array_idx);
                    delete mRefCount;
                }

                mRefCount = nullptr;
            } else {
                // not sharing with any other duk_value, free it
                free_ref(mContext, mPOD.ref_array_idx);
            }
            mType = Type::Undefined;
        }
    }
}
