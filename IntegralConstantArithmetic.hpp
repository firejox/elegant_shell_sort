#ifndef INTEGRALCONSTANTARITHMETIC_HPP
#define INTEGRALCONSTANTARITHMETIC_HPP

#include <type_traits>

namespace IntegralConstantArithmetic {
    template<class T, T ...I>
    using ISeq = std::integer_sequence<T, I...>;

    template<class T, T v>
    using IC = std::integral_constant<T, v>;

    template<class T, T v1, T v2>
    constexpr auto add(IC<T, v1>, IC<T, v2>) -> IC<T, v1 + v2>;

    template<class IC1, class IC2>
    using add_t = decltype(add(IC1(), IC2()));

    template<class T, T v>
    constexpr auto neg(IC<T, v>) -> IC<T, -v>;

    template<class IC1>
    using neg_t = decltype(neg(IC1()));

    template<class T, T v1, T v2>
    constexpr auto substract(IC<T, v1>, IC<T, v2>) -> IC<T, v1 - v2>;

    template<class IC1, class IC2>
    using substract_t = decltype(substract(IC1(), IC2()));

    template<class T, T v1, T v2>
    constexpr auto multiply(IC<T, v1>, IC<T, v2>) -> IC<T, v1 * v2>;

    template<class IC1, class IC2>
    using multiply_t = decltype(multiply(IC1(), IC2()));

    template<class T, T v1, T v2>
    constexpr auto divide(IC<T, v1>, IC<T, v2>) -> IC<T, v1 / v2>;

    template<class IC1, class IC2>
    using divide_t = decltype(divide(IC1(), IC2()));

    template<class T, T v1, T v2>
    constexpr auto remainder(IC<T, v1>, IC<T, v2>) -> IC<T, v1 % v2>;

    template<class IC1, class IC2>
    using remainder_t = decltype(remainder(IC1(), IC2()));
};

#endif

