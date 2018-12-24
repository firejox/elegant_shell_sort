#ifndef FIBFUZZYSHELLSORT_HPP
#define FIBFUZZYSHELLSORT_HPP

#include <limits>
#include "ShellSortTemplate.hpp"
#include "IntegralConstantArithmetic.hpp"

namespace FibFuzzyNS {
    using namespace IntegralConstantArithmetic;

    template <class Numeric, size_t sz> 
    struct Number {
        using fib_type = add_t<typename Number<Numeric, sz - 1>::fib_type, typename Number<Numeric, sz - 2>::fib_type>;
        using grand_type = neg_t<typename Number<Numeric, sz - 1>::grand_type>;

        using type = add_t<fib_type, grand_type>;
    };

    template <class Numeric>
    struct Number<Numeric, 0> {
        using type = IC<Numeric, 1>;
    };

    template <class Numeric>
    struct Number<Numeric, 1> {
        using fib_type = IC<Numeric, 5>;
        using grand_type = IC<Numeric, -1>;

        using type = add_t<fib_type, grand_type>;
    };

    template <class Numeric>
    struct Number<Numeric, 2> {
        using fib_type = IC<Numeric, 8>;
        using grand_type = IC<Numeric, 1>;

        using type = add_t<fib_type, grand_type>;
    };
    
    template <class Numeric = int>
    class Sequence {
        // Fibonacci grand number generator
        //
        
        static constexpr size_t table_size() {
            constexpr Numeric max_num = std::numeric_limits<Numeric>::max();
            Numeric f0(5), f1(8), f2(1);
            Numeric grand = -1;

            size_t sz = 3;
            while ((max_num - f1) >= (f0 + grand)) {
                f2 = f1 + f0;
                f0 = f1;
                f1 = f2;
                grand = - grand;
                sz++;
            }

            return sz;
        }
        template <size_t ...I>
        static constexpr decltype(auto) gen_seq(std::index_sequence<I...>) {
            return std::integer_sequence<Numeric, Number<Numeric, sizeof...(I) - I - 1>::type::value...>{};
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
