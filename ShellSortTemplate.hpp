#ifndef SHELLSORTTEMPLATE_HPP
#define SHELLSORTTEMPLATE_HPP

#include <algorithm>
#include <iterator>
#include <utility>

namespace ShellSortTemplate {
    template<class Iterator, class Compare, class T, T ...Seq>
    constexpr void sort_impl(Iterator first, Iterator last, Compare comp, const T size, std::integer_sequence<T, Seq...>) noexcept {
        for (const auto gap : {Seq...}) {
            if (size > gap) {
                const auto h = first + gap;
                for (auto i = h; i < last; i++) {
                    if (comp(*i, *(i - gap))) {
                        auto v = std::move(*i);
                        auto j = i;

                        do {
                            *j = std::move(*(j - gap));
                            j -= gap;
                        } while (j >= h && comp(v, *(j - gap)));

                        *j = std::move(v);
                    }
                }
            }
        }
    }

    template<class Iterator, class Sequence, class Comparator = std::less<>>
    constexpr void sort(Iterator first, Iterator last, Comparator comp = Comparator()) noexcept {
        sort_impl(first, last, comp, std::distance(first, last), typename Sequence::type{});
    }
}

#endif
