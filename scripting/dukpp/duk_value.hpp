#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <ostream>

#include <duktape/duktape.h>

#include "dukexception.hpp"

namespace dukpp {
// A variant class for Duktape values.
// This class is not really dependant on the rest of dukpp, but the rest of dukpp is integrated to support it.
// Script objects are persisted by copying a reference to the object into an array in the heap stash.
// When we need to push a reference to the object, we just look up that reference in the stash.

// duk_values can be copied freely. We use reference counting behind the scenes to keep track of when we need
// to remove our reference from the heap stash. Memory for reference counting is only allocated once a duk_value
// is copied (either by copy constructor or operator=). std::move can be used if you are trying to avoid ref counting
// for some reason.

// One script object can have multiple, completely separate duk_values "pointing" to it - in this case, there will be
// multiple entries in the "ref array" that point to the same object. This will happen if the same script object is
// put on the stack and turned into a duk_value multiple times independently (copy-constructing/operator=-ing
// duk_values will not do this!). This is okay, as we are only keeping track of these objects to prevent garbage
// collection (and access them later). This could be changed to use a map structure to look up one canonical entry per
// script object in the "ref array" (I guess it would be more like a ref map in this case), but this would require a map
// lookup every time we construct a duk_value. The performance difference probably isn't *that* noticeable (a good map
// would probably be amortized constant-time lookup), but I am guessing constructing many separate duk_values that point
// to the same script object isn't a very common thing.
    class duk_value {
    public:
        enum class Type : uint8_t {
            //None = DUK_TYPE_NONE,
            Undefined = DUK_TYPE_UNDEFINED,
            NullRef = DUK_TYPE_NULL,
            Boolean = DUK_TYPE_BOOLEAN,
            Number = DUK_TYPE_NUMBER,
            String = DUK_TYPE_STRING,
            Object = DUK_TYPE_OBJECT,
            Buffer = DUK_TYPE_BUFFER,
            Pointer = DUK_TYPE_POINTER,
            LightFunc = DUK_TYPE_LIGHTFUNC  // not implemented
        };

        enum class BufferType : uint8_t {
            Undefined = 0,
            Fixed = 1,
            Dynamic = 2,
            External = 3
        };

        // default constructor just makes an undefined-type duk_value
        inline duk_value() : mContext(nullptr), mType(Type::Undefined), mRefCount(nullptr) {}

        virtual ~duk_value() {
            // release any references we have
            release_ref_count();
        }

        // move constructor
        inline duk_value(duk_value &&move) noexcept: mContext(move.mContext), mType(move.mType),
                                                     mPOD(move.mPOD), mBufSize(move.mBufSize),
                                                     mBufferType(move.mBufferType), mRefCount(move.mRefCount) {
            if (mType == Type::String)
                mString = std::move(move.mString);

            move.mType = Type::Undefined;
            move.mRefCount = nullptr;
            move.mBufSize = 0;
            move.mBufferType = BufferType::Undefined;
        }

        duk_value &operator=(const duk_value &rhs);

        // copy constructor
        inline duk_value(const duk_value &copy) : duk_value() {
            *this = copy;
        }

        // equality operator
        bool operator==(const duk_value &rhs) const;

        inline bool operator!=(const duk_value &rhs) const {
            return !(*this == rhs);
        }

        // copies the object at idx on the stack into a new duk_value and returns it
        static duk_value copy_from_stack(duk_context *ctx, duk_idx_t idx = -1);

    protected:
        static duk_ret_t json_decode_safe(duk_context *ctx, void *) {
            duk_json_decode(ctx, -1);
            return 1;
        }

    public:
        static_assert(sizeof(char) == 1, "Serialization probably broke");
        static duk_value deserialize(duk_context *ctx, char const *data, size_t data_len);

        // Important limitations:
        // - The returned value is binary and will not behave well if you treat it like a string (it will almost certainly contain '\0').
        //   If you need to transport it like a string, maybe try encoding it as base64.
        // - Strings longer than 2^32 (UINT32_MAX) characters will throw an exception. You can raise this to be a uint64_t if you need
        //   really long strings for some reason (be sure to change duk_value::deserialize() as well).
        // - Objects are encoded to JSON and then sent like a string. If your object can't be encoded as JSON (i.e. it's a function),
        //   this will not work. This can be done, but I chose not to because it poses a security issue if you deserializing untrusted data.
        //   If you require this functionality, you'll have to add it yourself with using duk_dump_function(...).
        [[nodiscard]] std::vector<char> serialize() const;

        // same as above (copy_from_stack), but also removes the value we copied from the stack
        static duk_value take_from_stack(duk_context *ctx, duk_idx_t idx = -1) {
            duk_value val = copy_from_stack(ctx, idx);
            duk_remove(ctx, idx);
            return val;
        }

        // push the value we hold onto the stack
        inline void push() const;

        // various (type-safe) getters
        inline bool as_bool() const {
            if (mType != Type::Boolean) {
                throw duk_exception() << "Expected boolean, got " << type_name();
            }
            return mPOD.boolean;
        }

        inline double as_double() const {
            if (mType != Type::Number) {
                throw duk_exception() << "Expected number, got " << type_name();
            }
            return mPOD.number;
        }

        inline float as_float() const {
            if (mType != Type::Number) {
                throw duk_exception() << "Expected number, got " << type_name();
            }
            return static_cast<float>(mPOD.number);
        }

        inline duk_int_t as_int() const {
            if (mType != Type::Number) {
                throw duk_exception() << "Expected number, got " << type_name();
            }
            return static_cast<int32_t>(mPOD.number);
        }

        inline duk_uint_t as_uint() const {
            if (mType != Type::Number) {
                throw duk_exception() << "Expected number, got " << type_name();
            }
            return static_cast<uint32_t>(mPOD.number);
        }

        inline void *as_pointer() const {
            if (mType != Type::Pointer && mType != Type::NullRef) {
                throw duk_exception() << "Expected pointer or null, got " << type_name();
            }
            return mPOD.pointer;
        }

        inline uint8_t *as_plain_buffer(duk_size_t *size = nullptr, BufferType *bufferType = nullptr) const {
            if (mType != Type::Buffer) {
                throw duk_exception() << "Expected buffer, got " << type_name();
            }

            if (size != nullptr) {
                *size = mBufSize;
            }

            if (bufferType != nullptr) {
                *bufferType = mBufferType;
            }

            return mPOD.plain_buffer;
        }

        inline const std::string &as_string() const {
            if (mType != Type::String) {
                throw duk_exception() << "Expected string, got " << type_name();
            }
            return mString;
        }

        inline char const *as_c_string() const {
            if (mType != Type::String) {
                throw duk_exception() << "Expected string, got " << type_name();
            }
            return mString.data();
        }

        std::vector<duk_value> as_array() const;

        std::map<std::string, duk_value> as_map() const;

        inline Type type() const {
            return mType;
        }

        // same as duk_get_type_name(), but that's internal to Duktape, so we shouldn't use it
        inline char const *type_name() const noexcept;

        inline duk_context *context() const noexcept {
            return mContext;
        }

    private:
        // THIS IS COMPLETELY UNRELATED TO DETAIL_REFS.H.
        // detail_refs.h stores a mapping of native object -> script object.
        // This just stores arbitrary script objects (which likely have no native object backing them).
        // If I was smarter I might merge the two implementations, but this one is simpler
        // (since we don't need the std::map here).
        static void push_ref_array(duk_context *ctx) noexcept;

        // put a new reference into the ref array and return its index in the array
        static duk_uint_t stash_ref(duk_context *ctx, duk_idx_t idx) noexcept;

        // remove ref_array_idx from the ref array and add its spot to the free list (at refs[0])
        static void free_ref(duk_context *ctx, duk_uarridx_t ref_array_idx) noexcept;

        // this is for reference counting - used to release our reference based on the state
        // of mRefCount. If mRefCount is NULL, we never got copy constructed, so we have ownership
        // of our reference and can free it. If it's not null and above 1, we decrement the counter
        // (someone else owns the reference). If it's not null and equal to 1, we are the last owner
        // of a previously shared reference, so we can free it.
        void release_ref_count() noexcept;

        duk_context *mContext;
        Type mType;  // our type - one of the standard Duktape DUK_TYPE_* values

        // This holds the plain-old-data types. Since this is a variant,
        // we hold only one value at a time, so this is a union to save
        // a bit of space.
        union ValueTypes {
            bool boolean;
            double number;
            void *pointer;  // if mType == NULLREF, this is 0 (otherwise holds pointer value when mType == POINTER)
            uint8_t *plain_buffer;
            duk_uarridx_t ref_array_idx;
        } mPOD;

        duk_size_t mBufSize;
        BufferType mBufferType;

        std::string mString;  // if it's a string, we store it with std::string
        int *mRefCount;  // if mType == OBJECT and we're sharing, this will point to our ref counter
    };
}
