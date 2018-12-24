#ifndef A109110SHELLSORT_HPP
#define A109110SHELLSORT_HPP

#include <limits>
#include "ShellSortTemplate.hpp"
#include "IntegralConstantArithmetic.hpp"

namespace A109110NS {
    using namespace IntegralConstantArithmetic;

    template <class Numeric, size_t sz>
    struct Number {
        using type = substract_t< 
                        add_t<
                            multiply_t<typename Number<Numeric, sz - 1>::type, IC<Numeric, 2>>,
                            typename Number<Numeric, sz - 2>::type>,
                        typename Number<Numeric, sz - 3>::type>;
    };
    
    template <class Numeric>
    struct Number<Numeric, 0> {
        using type = IC<Numeric, 4>;
    };
    
    template <class Numeric>
    struct Number<Numeric, 1> {
        using type = IC<Numeric, 9>;
    };
    
    template <class Numeric>
    struct Number<Numeric, 2> {
        using type = IC<Numeric, 20>;
    };

    template <class Numeric = int>
    class Sequence {
        // A109110 number generator
        //
        
        static constexpr size_t table_size() {
            constexpr Numeric max_num = std::numeric_limits<Numeric>::max();
            Numeric f0(4), f1(9), f2(20), f3(1);
            constexpr auto two = IC<Numeric, 2>::value;
            size_t sz = 3;
            while (((max_num - f1 + f0) / two) >= f2) {
                f3 = f2 * two + f1 - f0;
                f0 = f1;
                f1 = f2;
                f2 = f3;
                sz++;
            }

            return sz;
        }

        template <size_t ...I>
        static constexpr decltype(auto) gen_seq(std::index_sequence<I...>) {
            return std::integer_sequence<Numeric, Number<Numeric, sizeof...(I) - I - 1>::type::value..., IC<Numeric, 1>::value>{};
        }

        public:
        using type = decltype(gen_seq(std::make_index_sequence<table_size()>())); 
    };

    template<class Iterator, class Comparator=std::less<>>
    void shellsort(Iterator first, Iterator last, Comparator comp = Comparator()) noexcept {
        ShellSortTemplate::sort<Iterator, Sequence<typename std::iterator_traits<Iterator>::difference_type>, Comparator>(first, last, comp);
    }

};
#endif
