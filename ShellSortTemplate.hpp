#ifndef SHELLSORTTEMPLATE_HPP
#define SHELLSORTTEMPLATE_HPP

#include <algorithm>
#include <iterator>
#include <utility>

namespace ShellSortTemplate {
    template<class Iterator, class Compare, class T, T Gap, T ...Seq>
    constexpr void sort_impl(Iterator first, Iterator last, Compare comp, const T size, std::integer_sequence<T, Gap, Seq...>) noexcept {
        if (size > Gap) {
            const auto h = first + Gap;
            for (auto i = h; i < last; i++) {
                if (comp(*i, *(i - Gap))) {
                    auto v = std::move(*i);

                    auto j = i;

                    do {
                        *j = std::move(*(j - Gap));
                        j -= Gap;
                    } while (j >= h && comp(v, *(j - Gap)));

                    *j = std::move(v);
                }
            }
        }

        if constexpr (sizeof...(Seq) > 0)
            sort_impl<Iterator, Compare, T, Seq...>(first, last, comp, size, std::integer_sequence<T, Seq...>{});
    }

    template<class Iterator, class Sequence, class Comparator = std::less<>>
    constexpr void sort(Iterator first, Iterator last, Comparator comp = Comparator()) noexcept {
        sort_impl(first, last, comp, std::distance(first, last), typename Sequence::type{});
    }
}

#endif
