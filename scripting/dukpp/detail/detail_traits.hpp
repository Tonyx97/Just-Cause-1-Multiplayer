#pragma once

#include <functional>

#include <duktape/duktape.h>

namespace dukpp::detail {
    template<typename T, typename... Ts>
    struct contains : std::disjunction<std::is_same<T, Ts>...> {
    };

    template<typename... Ts>
    struct last_type {
        template<typename T>
        struct tag {
            using type = T;
        };
        // Use a fold-expression to fold the comma operator over the parameter pack.
        using type = typename decltype((tag<Ts>{}, ...))::type;
    };

    template<typename T, typename...>
    struct first_type {
        using type = T;
    };

    template<size_t i, typename... Args>
    struct nth_type {
        using type = typename std::tuple_element<i, std::tuple<Args...> >::type;
    };

    template<typename...Ts>
    using tuple_cat_t = decltype(std::tuple_cat(std::declval<Ts>()...));

    template<typename T, typename...Ts>
    using remove_t = tuple_cat_t<
            typename std::conditional<
                    std::is_same<T, Ts>::value,
                    std::tuple<>,
                    std::tuple<Ts>>::type...>;

    //////////////////////////////////////////////////////////////////////////////////////////////

    // Credit to LuaState for this code:
    // https://github.com/AdUki/LuaState/blob/master/include/Traits.h

    template<size_t...>
    struct index_tuple {
    };

    template<size_t I, typename IndexTuple, typename ... Types>
    struct make_indexes_impl;

    template<size_t I, size_t... Indexes, typename T, typename ... Types>
    struct make_indexes_impl<I, index_tuple<Indexes...>, T, Types...> {
        typedef typename make_indexes_impl<I + 1, index_tuple<Indexes..., I>, Types...>::type type;
    };

    template<size_t I, size_t... Indexes>
    struct make_indexes_impl<I, index_tuple<Indexes...> > {
        typedef index_tuple<Indexes...> type;
    };

    template<typename ... Types>
    struct make_indexes : make_indexes_impl<0, index_tuple<>, Types...> {
    };

    //////////////////////////////////////////////////////////////////////////////////////////////

    template<std::size_t... Is>
    struct indexes {
    };

    template<std::size_t N, std::size_t... Is>
    struct indexes_builder : indexes_builder<N - 1, N - 1, Is...> {
    };

    template<std::size_t... Is>
    struct indexes_builder<0, Is...> {
        typedef indexes<Is...> index;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////

    // This mess is used to use function arugments stored in an std::tuple to an
    // std::function, function pointer, or method.

    // std::function
    template<class Ret, class... Args, size_t... Indexes>
    Ret apply_helper(std::function<Ret(Args...)> pf, index_tuple<Indexes...>, std::tuple<Args...> &&tup) noexcept {
        return pf(std::forward<Args>(std::get<Indexes>(tup))...);
    }

    template<class Ret, class ... Args>
    Ret apply(std::function<Ret(Args...)> pf, const std::tuple<Args...> &tup) noexcept {
        return apply_helper(pf, typename make_indexes<Args...>::type(), std::tuple<Args...>(tup));
    }

    // function pointer
    template<class Ret, class... Args, class... BakedArgs, size_t... Indexes>
    Ret apply_fp_helper(Ret(*pf)(Args...), index_tuple<Indexes...>, std::tuple<BakedArgs...> &&tup) noexcept {
        return pf(std::forward<Args>(std::get<Indexes>(tup))...);
    }

    template<class Ret, class... Args, class... BakedArgs, size_t... Indexes>
    Ret apply_fp_helper_ctx(duk_context *ctx, Ret(*pf)(Args...), index_tuple<Indexes...>,
                            std::tuple<BakedArgs...> &&tup) noexcept {
        return reinterpret_cast<Ret(*)(duk_context *, Args...)>(pf)(ctx, std::forward<Args>(std::get<Indexes>(tup))...);
    }

    template<class Ret, class ... Args, class ... BakedArgs>
    Ret apply_fp(Ret(*pf)(Args...), const std::tuple<BakedArgs...> &tup) noexcept {
        return apply_fp_helper(pf, typename make_indexes<BakedArgs...>::type(), std::tuple<BakedArgs...>(tup));
    }

    template<class Ret, class ... Args, class ... BakedArgs>
    Ret apply_fp_ctx(duk_context *ctx, Ret(*pf)(Args...), const std::tuple<BakedArgs...> &tup) noexcept {
        return apply_fp_helper_ctx(ctx, pf, typename make_indexes<BakedArgs...>::type(), std::tuple<BakedArgs...>(tup));
    }

    // method pointer
    template<class Cls, class Ret, class... Args, class... BakedArgs, size_t... Indexes>
    Ret
    apply_method_helper(Ret(Cls::*pf)(Args...),
                        index_tuple<Indexes...>, Cls *obj,
                        std::tuple<BakedArgs...> &&tup) noexcept {
        return (*obj.*pf)(std::forward<Args>(std::get<Indexes>(tup))...);
    }

    template<class Cls, class Ret, class ... Args, class... BakedArgs>
    Ret apply_method(Ret(Cls::*pf)(Args...), Cls *obj, const std::tuple<BakedArgs...> &tup) noexcept {
        return apply_method_helper(pf, typename make_indexes<Args...>::type(), obj, std::tuple<BakedArgs...>(tup));
    }

    // const method pointer
    template<class Cls, class Ret, class... Args, class... BakedArgs, size_t... Indexes>
    Ret apply_method_helper(Ret(Cls::*pf)(Args...) const, index_tuple<Indexes...>, Cls *obj,
                            std::tuple<BakedArgs...> &&tup) noexcept {
        return (*obj.*pf)(std::forward<Args>(std::get<Indexes>(tup))...);
    }

    template<class Cls, class Ret, class ... Args, class... BakedArgs>
    Ret apply_method(Ret(Cls::*pf)(Args...) const, Cls *obj, const std::tuple<BakedArgs...> &tup) noexcept {
        return apply_method_helper(pf, typename make_indexes<Args...>::type(), obj, std::tuple<BakedArgs...>(tup));
    }

    // constructor
    template<class Cls, typename... Args, size_t... Indexes>
    Cls *apply_constructor_helper(index_tuple<Indexes...>, std::tuple<Args...> &&tup) noexcept {
        return new Cls(std::forward<Args>(std::get<Indexes>(tup))...);
    }

    template<class Cls, typename... Args>
    Cls *apply_constructor(const std::tuple<Args...> &tup) noexcept {
        return apply_constructor_helper<Cls>(typename make_indexes<Args...>::type(), std::tuple<Args...>(tup));
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
}
