#ifndef HIB63SHELLSORT_HPP
#define HIB63SHELLSORT_HPP

#include <limits>
#include "ShellSortTemplate.hpp"
#include "IntegralConstantArithmetic.hpp"

namespace Hib63NS {
    using namespace IntegralConstantArithmetic; 

    template <class Numeric, size_t sz>
    struct Number {
        using type = add_t<typename Number<Numeric, sz - 1>::type,
                     add_t<typename Number<Numeric, sz - 1>::type, IC<Numeric, 1>>>;
    };
    
    template <class Numeric>
    struct Number<Numeric, 0> {
        using type = IC<Numeric, 0>;
    };
    
    template <class Numeric>
    struct Number<Numeric, 1> {
        using type = IC<Numeric, 1>;
    };

    template <class Numeric = int>
    class Sequence {
        // Hibbard63 number generator
        //
        
        static constexpr size_t table_size() {
            return std::numeric_limits<Numeric>::digits + 1;
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
