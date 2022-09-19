# pragma once

#include <duktape/duktape.h>

#include "detail/detail_class_proto.hpp"
#include "detail/detail_constructor.hpp"
#include "detail/detail_method.hpp"
#include "dukexception.hpp"

namespace dukpp {

    template<typename Cls, bool isManaged = false>
    struct _class {
        duk_context *ctx;

        _class(duk_context *ctx, char const *objectName) : ctx(ctx) {
            register_ctor(objectName);
            add_delete();
        }

        // todo: allow this_context in ctor
        template<typename... Ts>
        void register_ctor(char const *name) {
            duk_c_function constructor_func = detail::call_native_constructor<isManaged, Cls, Ts...>;

            duk_push_c_function(ctx, constructor_func, sizeof...(Ts));

            if constexpr (isManaged) {
                duk_c_function finalizer_func = detail::managed_finalizer<Cls>;
                // create new prototype with finalizer
                duk_push_object(ctx);
                // set the finalizer
                duk_push_c_function(ctx, finalizer_func, 1);
                duk_set_finalizer(ctx, -2);
            }
            detail::ProtoManager::push_prototype<Cls>(ctx);

            if constexpr (isManaged) {
                // hook prototype with finalizer up to real class prototype
                // must use duk_set_prototype, not set the .prototype property
                duk_set_prototype(ctx, -2);
            }

            // set constructor_func.prototype
            duk_put_prop_string(ctx, -2, "prototype");

            // set name = constructor_func
            duk_put_global_string(ctx, name);
        }

        template<typename BaseCls>
        _class &add_base_class() {
            static_assert(!std::is_pointer<BaseCls>::value && !std::is_pointer<Cls>::value
                          && !std::is_const<BaseCls>::value && !std::is_const<Cls>::value, "Use bare class names.");
            static_assert(std::is_base_of<BaseCls, Cls>::value, "Invalid class hierarchy!");

            // Cls.type_info->set_base(BaseCls.type_info)
            detail::ProtoManager::push_prototype<Cls>(ctx);
            duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("type_info"));
            auto derived_type_info = static_cast<detail::TypeInfo *>(duk_require_pointer(ctx, -1));
            duk_pop_2(ctx);

            detail::ProtoManager::push_prototype<BaseCls>(ctx);
            duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("type_info"));
            auto base_type_info = static_cast<detail::TypeInfo *>(duk_require_pointer(ctx, -1));
            duk_pop_2(ctx);

            derived_type_info->set_base(base_type_info);

            // also set up the prototype chain
            detail::ProtoManager::push_prototype<Cls>(ctx);
            detail::ProtoManager::push_prototype<BaseCls>(ctx);
            duk_set_prototype(ctx, -2);
            duk_pop(ctx);
        }

        template<typename RetType, typename... Ts>
        _class &add_method(char const *name, RetType(Cls::*method)(Ts...)) {
            return add_method<false, RetType, Ts...>(name, method);
        }

        template<typename RetType, typename... Ts>
        _class &add_method(char const *name, RetType(Cls::*method)(Ts...) const) {
            return add_method<true, RetType, Ts...>(name, method);
        }

        template<bool isConst, typename RetType, typename... Ts>
        _class &add_method(char const *name,
                           typename std::conditional<isConst, RetType(Cls::*)(Ts...) const, RetType(Cls::*)(
                                   Ts...)>::type method) {
            typedef detail::MethodInfo<isConst, Cls, RetType, Ts...> MethodInfo;

            duk_c_function method_func = MethodInfo::MethodRuntime::call_native_method;

            detail::ProtoManager::push_prototype<Cls>(ctx);

            duk_idx_t nargs = sizeof...(Ts);

            if constexpr (detail::contains<variadic_args, Ts...>() ||
                          detail::contains<variadic_args const &, Ts...>() ||
                          detail::contains<variadic_args const &&, Ts...>()) {
                nargs = DUK_VARARGS;
            }

            duk_push_c_function(ctx, method_func, nargs);

            duk_push_pointer(ctx, new typename MethodInfo::MethodHolder{method});
            duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("method_holder")); // consumes raw method pointer

            // make sure we free the method_holder when this function is removed
            duk_push_c_function(ctx, MethodInfo::MethodRuntime::finalize_method, 1);
            duk_set_finalizer(ctx, -2);

            duk_put_prop_string(ctx, -2, name); // consumes method function

            duk_pop(ctx); // pop prototype
            return *this;
        }

        // const getter, setter
        template<typename RetT, typename ArgT>
        _class &add_property(char const *name, RetT(Cls::*getter)() const, void(Cls::*setter)(ArgT)) {
            register_property<true, RetT, ArgT>(getter, setter, name);
            return *this;
        }

        // const getter, no-setter
        template<typename RetT>
        _class &add_property(char const *name, RetT(Cls::*getter)() const, std::nullptr_t setter) {
            register_property<true, RetT, RetT>(getter, setter, name);
            return *this;
        }

        // non-const getter, setter
        template<typename RetT, typename ArgT>
        _class &add_property(char const *name, RetT(Cls::*getter)(), void(Cls::*setter)(ArgT)) {
            register_property<false, RetT, ArgT>(getter, setter, name);
            return *this;
        }

        // non-const getter, no-setter
        template<typename RetT>
        _class &add_property(char const *name, RetT(Cls::*getter)(), std::nullptr_t setter) {
            register_property<false, RetT, RetT>(getter, setter, name);
        }

        // no getter, setter
        template<typename ArgT>
        _class &add_property(char const *name, std::nullptr_t getter, void(Cls::*setter)(ArgT)) {
            register_property<false, ArgT, ArgT>(getter, setter, name);
            return *this;
        }

        // no getter, no setter
        template<typename ValT>
        _class &add_property(char const *name, std::nullptr_t getter, std::nullptr_t setter) {
            static_assert(std::is_void<Cls>::value, "Must have getter or setter");
            return *this;
        }

        template<bool isConstGetter, typename RetT, typename ArgT>
        void
        register_property(typename std::conditional<isConstGetter, RetT(Cls::*)() const,
                RetT(Cls::*)()>::type getter,
                          void(Cls::*setter)(ArgT),
                          char const *name) {
            typedef detail::MethodInfo<isConstGetter, Cls, RetT> GetterMethodInfo;
            typedef detail::MethodInfo<false, Cls, void, ArgT> SetterMethodInfo;

            detail::ProtoManager::push_prototype<Cls>(ctx);

            // push key
            duk_push_string(ctx, name);

            // push getter
            if (getter != nullptr) {
                duk_c_function method_func = GetterMethodInfo::MethodRuntime::call_native_method;

                duk_push_c_function(ctx, method_func, 0);

                duk_push_pointer(ctx, new typename GetterMethodInfo::MethodHolder{getter});
                duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("method_holder")); // consumes raw method pointer

                // make sure we free the method_holder when this function is removed
                duk_push_c_function(ctx, GetterMethodInfo::MethodRuntime::finalize_method, 1);
                duk_set_finalizer(ctx, -2);
            } else {
                duk_push_c_function(ctx, throw_error, 1);
            }

            if (setter != nullptr) {
                duk_c_function method_func = SetterMethodInfo::MethodRuntime::call_native_method;

                duk_push_c_function(ctx, method_func, 1);

                duk_push_pointer(ctx, new typename SetterMethodInfo::MethodHolder{setter});
                duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("method_holder")); // consumes raw method pointer

                // make sure we free the method_holder when this function is removed
                duk_push_c_function(ctx, SetterMethodInfo::MethodRuntime::finalize_method, 1);
                duk_set_finalizer(ctx, -2);
            } else {
                duk_push_c_function(ctx, throw_error, 1);
            }

            duk_uint_t flags = DUK_DEFPROP_HAVE_GETTER
                               | DUK_DEFPROP_HAVE_SETTER
                               | DUK_DEFPROP_HAVE_CONFIGURABLE // set not configurable (from JS)
                               | DUK_DEFPROP_FORCE; // allow overriding built-ins and previously defined properties

            duk_def_prop(ctx, -4, flags);
            duk_pop(ctx);  // pop prototype
        }

    private:
        void add_delete() {
            duk_c_function delete_func;
            if constexpr (isManaged) {
                // managed objects cannot be freed from native code safely
                static auto stub = [](duk_context *) -> duk_ret_t { return 0; };
                delete_func = stub;
            } else {
                delete_func = detail::call_native_deleter<Cls>;
            }

            detail::ProtoManager::push_prototype<Cls>(ctx);
            duk_push_c_function(ctx, delete_func, 0);
            duk_put_prop_string(ctx, -2, "delete");
            duk_pop(ctx);  // pop prototype
        }
    };

    template<typename Cls>
    using managed_class = _class<Cls, true>;

    template<typename Cls>
    using unmanaged_class = _class<Cls, false>;

    inline void object_invalidate(duk_context *ctx, void *obj_ptr) noexcept {
        detail::object_invalidate(ctx, obj_ptr);
    }
}
