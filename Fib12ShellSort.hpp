#ifndef FIB12SHELLSORT_HPP
#define FIB12SHELLSORT_HPP

#include <limits>
#include "ShellSortTemplate.hpp"
#include "IntegralConstantArithmetic.hpp"

namespace Fib12NS {
    using namespace IntegralConstantArithmetic;

    template <class Numeric, size_t sz>
    struct Number {
        using type = add_t<typename Number<Numeric, sz - 1>::type,
                     add_t<typename Number<Numeric, sz - 2>::type, typename Number<Numeric, sz - 2>::type>>;
    };
    
    template <class Numeric>
    struct Number<Numeric, 0> {
        using type = IC<Numeric, 1>;
    };
    
    template <class Numeric>
    struct Number<Numeric, 1> {
        using type = IC<Numeric, 1>;
    };

    template <class Numeric = int>
    class Sequence {
        // Fibonacci(1, 2) number generator
        //
        
        static constexpr size_t table_size() {
            constexpr Numeric max_num = std::numeric_limits<Numeric>::max();
            Numeric f0(1), f1(1), f2(1);
            size_t sz = 2;
            while (((max_num - f1) / 2) >= f0) {
                f2 = f1 + f0 + f0;
                f0 = f1;
                f1 = f2;
                sz++;
            }

            return sz;
        }
        template <size_t ...I>
        static constexpr decltype(auto) gen_seq(std::index_sequence<I...>) {
            return std::integer_sequence<Numeric, Number<Numeric, sizeof...(I) - I>::type::value...>{};
        }

        public:
        using type = decltype(gen_seq(std::make_index_sequence<table_size() - 1>())); 
    };

    template<class Iterator, class Comparator=std::less<>>
    void shellsort(Iterator first, Iterator last, Comparator comp = Comparator()) noexcept {
        ShellSortTemplate::sort<Iterator, Sequence<typename std::iterator_traits<Iterator>::difference_type>, Comparator>(first, last, comp);
    }

};

#endif
