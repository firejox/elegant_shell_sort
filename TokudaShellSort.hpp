#ifndef TOKUDASHELLSORT_HPP
#define TOKUDASHELLSORT_HPP

#include <limits>
#include "ShellSortTemplate.hpp"
#include "IntegralConstantArithmetic.hpp"

namespace TokudaNS {
    using namespace IntegralConstantArithmetic;

    template <class Numeric, size_t sz>
    struct Number {
        using type = add_t< 
                        add_t<
                            multiply_t<typename Number<Numeric, sz - 1>::type, IC<Numeric, 2>>,
                            divide_t<
                                add_t<typename Number<Numeric, sz - 1>::type,
                                      IC<Numeric, 3>>,
                                IC<Numeric, 4>>>,
                        IC<Numeric, 1>>;
    };
    
    template <class Numeric>
    struct Number<Numeric, 0> {
        using type = IC<Numeric, 1>;
    };

    template <class Numeric = int>
    class Sequence {
        // Proper Tokuda number generator
        //
        
        static constexpr size_t table_size() {
            constexpr Numeric max_num = std::numeric_limits<Numeric>::max();
            Numeric f0(1), f1(1);
            constexpr auto one = IC<Numeric, 1>::value;
            constexpr auto two = IC<Numeric, 2>::value;
            constexpr auto three = IC<Numeric, 3>::value;
            constexpr auto four = IC<Numeric, 4>::value;
            size_t sz = 1;
            while ((max_num - (f0 + three) / four - one) / f0 >= two) {
                f1 = f0 * two + (f0 + three) / four + one;
                f0 = f1;
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
