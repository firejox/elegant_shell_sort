#include <algorithm>
#include <iterator>
#include <limits>
#include <array>
#include <random>
#include <cstdio>
#include <chrono>
#include <ratio>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include <type_traits>

#include "Fib13ShellSort.hpp"
#include "A109110ShellSort.hpp"
#include "TokudaShellSort.hpp"
#include "FibFuzzyShellSort.hpp"

namespace FibonacciNS {
    // Fibonacci number generator
    //
    template <class Numeric, size_t sz>
        struct Number {
            static constexpr Numeric value = Number<Numeric, sz-1>::value + Number<Numeric, sz-2>::value;
        };

    template<class Numeric>
        struct Number<Numeric, 0> {
            static constexpr Numeric value = 1;
        };

    template<class Numeric>
        struct Number<Numeric, 1> {
            static constexpr Numeric value = 1;
        };

    // Build Fibonacci Table
    //
    template <class Numeric=int>
        struct Table {
            static constexpr size_t table_size() {
                constexpr Numeric max_num = std::numeric_limits<Numeric>::max();
                Numeric f0(1), f1(1), f2(1);
                size_t sz = 2;
                while ((max_num - f1) >= f0) {
                    f2 = f1 + f0;
                    f0 = f1;
                    f1 = f2;
                    sz++;
                }

                return sz;
            }

            template <std::size_t ...I>
                static constexpr auto gen_table(std::index_sequence<I...>) {
                    return std::array<Numeric, sizeof...(I)>{Number<Numeric, I>::value...};   
                }

            constexpr static auto table = gen_table(std::make_index_sequence<table_size()>());
        };

};


// Original version of Fibonacci Shell sort
//
template <typename It, typename Compare = std::less<typename std::iterator_traits<It>::value_type>>
void fib_shell_sort(It first, It last, Compare comp = Compare()) {
    typename std::iterator_traits<It>::difference_type current_array_size = std::distance(first, last);

    constexpr auto& table = FibonacciNS::Table<decltype(current_array_size)>::table;
    constexpr auto one = static_cast<decltype(current_array_size)>(1);

    for (auto k = std::prev(std::lower_bound(table.begin(), table.end(), current_array_size)); k > table.begin(); k--) {
        const auto gap = *k;
        const auto h = first + gap;

        for (auto i = h; i < last; i++) {
            if (comp(*i, *(i - gap))) {
                typename std::iterator_traits<It>::value_type v = std::move(*i);

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
// Variant version of Fibonacci Shell sort
//  Shell sort with the gap of two step Fibonacci number
//  becuse the Fibonacci number F_2n = \sum_{i=1}^{n} F_2i
//
template <typename It, typename Compare = std::less<typename std::iterator_traits<It>::value_type>>
void fib_shell_sort2(It first, It last, Compare comp = Compare()) {
    typename std::iterator_traits<It>::difference_type current_array_size = std::distance(first, last);

    constexpr auto& table = FibonacciNS::Table<decltype(current_array_size)>::table;
    constexpr auto one = static_cast<decltype(current_array_size)>(1);
    constexpr auto small_array_size = static_cast<decltype(current_array_size)>(10);

    // Because it is hard to maintain the correctness under the small array,
    // I use insertion sort to handle it.
    if (current_array_size < small_array_size) {
        for (auto i = first + one; i < last; i++) {
            if (comp(*i, *(i - one))) {
                typename std::iterator_traits<It>::value_type v = std::move(*i);
                auto j = i;

                do {
                    *j = std::move(*(j - one));
                    j--;
                } while (j > first && comp(v, *(j - one)));

                *j = std::move(v);
            }
        }
        return;
    }

    for (auto k = std::lower_bound(table.begin(), table.end(), current_array_size - one); k >= table.begin(); k-= ptrdiff_t(2)) {
        const auto gap = *k;

        for (auto i = first + gap, h = i; i < last; i++) {
            if (comp(*i, *(i - gap))) {
                typename std::iterator_traits<It>::value_type v = std::move(*i);

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

// Hybrid verion
//  Combine insertion sort into shell sort loop to give better 
//  performance on nearly sorted array.
//
template <typename It, typename Compare = std::less<typename std::iterator_traits<It>::value_type>>
void hyfib_shell_sort(It first, It last, Compare comp = Compare()) {
    typename std::iterator_traits<It>::difference_type current_array_size = std::distance(first, last);

    constexpr auto& table = FibonacciNS::Table<decltype(current_array_size)>::table;
    constexpr auto one = static_cast<decltype(current_array_size)>(1);
    constexpr auto zero = static_cast<decltype(current_array_size)>(0);
    constexpr auto small_array_size = static_cast<decltype(current_array_size)>(10);
    constexpr auto fib_step = static_cast<ptrdiff_t>(2);
    constexpr auto t_begin = table.begin();

    // Same as Variant version of Fibonacci Shell Sort
    if (current_array_size < small_array_size) {
        for (auto i = first + one; i < last; i++) {
            if (comp(*i, *(i - one))) {
                typename std::iterator_traits<It>::value_type v = std::move(*i);
                auto j = i;

                do {
                    *j = std::move(*(j - one));
                    j--;
                } while (j > first && comp(v, *(j - one)));

                *j = std::move(v);
            }
        }
        return;
    }

    auto k = std::lower_bound(table.begin(), table.end(), current_array_size - one);
    decltype(k) insert_lim = table.begin() + static_cast<ptrdiff_t>(!((k - table.begin()) & static_cast<ptrdiff_t>(1)));
    decltype(current_array_size) next_iter = zero;
    decltype(current_array_size) lim;
    decltype(first) i, j;

    for (; k >= t_begin; k-= fib_step, insert_lim += fib_step) {
        const auto gap = *k;
        const auto h = first + std::max(gap, next_iter);

        for (i = h; i < last; i++) {
            if (comp(*i, *(i - gap))) {
                typename std::iterator_traits<It>::value_type v = std::move(*i);

                j = i;

                do {
                    *j = std::move(*(j - gap));
                    j -= gap;
                } while (j >= h && comp(v, *(j - gap)));

                *j = std::move(v);
            }
        }

        if (gap == one)
            break;

        lim = *insert_lim;
        for (i = first + one; i < last; i++) {
            if (comp(*i, *(i - one))) {
                auto v = std::move(*i);
                j = i;

                do {
                    *j = std::move(*(j - one));
                    j--;
                    lim--;
                } while (j > first && comp(v, *(j - one)));

                *j = std::move(v);

                if (lim < one)
                    break;
            }
        }

        if (i == last)
            return;
        next_iter = i - first;
    }
}


namespace Benchmark {
    using TimeUnit = std::chrono::duration<double, std::milli>;
    using DeciSecond = std::chrono::duration<double, std::deci>;
    using Second = std::chrono::duration<double>;

    template<char ...ch>
        struct TaskName {
            static constexpr char name[sizeof...(ch) + 1] = {ch..., '\0'};
        };

#define tname(s) s##_tname

    template <class T, T... chars>
    constexpr TaskName<chars...> operator ""_tname() { return {}; }

    template<class Name, class Func>
        struct Task2 : Name, Func {
            static int cycles;
            static size_t size;
            static double mean;
            static double variance;
            static double stddev;
            static double relative_stddev;

            static double average_count;

            using of = Name;

            template<class Init>
                void run_warmup(TimeUnit warmup, Init &init) const {
                    int count = 0;
                    TimeUnit total(0);

                    using self = typename std::remove_cv<std::remove_reference_t<decltype(*this)>>::type;
                    constexpr auto cmp = [](const int &a, const int &b) constexpr ->bool { return a < b;};

                    while (warmup.count() > 0) {
                        init();

                        auto before = std::chrono::high_resolution_clock::now();

                        Func::operator()(cmp);

                        auto after = std::chrono::high_resolution_clock::now();

                        auto tmp = std::chrono::duration_cast<TimeUnit>(after - before);

                        total += tmp;
                        warmup -= tmp;
                        count++;
                    }

                    self::cycles = std::max(1, int(count / std::chrono::duration_cast<DeciSecond>(total).count()));
                }

            template<class Init>
                void run_calculate(TimeUnit calc_time, Init &init) const {
                    using self = typename std::remove_cv<std::remove_reference_t<decltype(*this)>>::type;
                    self::size = 0;
                    size_t counter = 0;
                    auto cmp = [&counter](const int &a, const int &b) ->bool { counter++; return a < b;};
                    
                    do {
                        TimeUnit subtotal(0);
                        counter = 0;

                        for (int i = 0; i < self::cycles; i++) {
                            init();

                            auto before = std::chrono::high_resolution_clock::now();

                            Func::operator()(cmp);

                            auto after = std::chrono::high_resolution_clock::now();

                            subtotal += std::chrono::duration_cast<TimeUnit>(after - before);
                        }

                        calc_time -= subtotal;

                        auto count_n = (double)counter / self::cycles;
                        auto measure_n = std::chrono::duration_cast<Second>(subtotal).count();
                        self::size++;

                        if (size > 1) {
                            auto delta = measure_n - self::mean;
                            self::mean += delta / self::size;
                            auto delta2 = measure_n - self::mean;
                            self::variance += delta * delta2;

                            delta = count_n - self::average_count;
                            self::average_count += delta / self::size;
                        } else {
                            self::mean = measure_n;
                            self::average_count = count_n;
                        }

                    } while(calc_time.count() > 0);

                    self::variance /= self::size;
                    self::stddev = std::sqrt(self::variance);
                    self::relative_stddev = 100.0 * (self::stddev / self::mean);
                }

            constexpr void clean() const {
                using self = typename std::remove_cv<std::remove_reference_t<decltype(*this)>>::type;

                self::cycles = 0;
                self::size = 0;
                self::mean = 0;
                self::variance = 0;
                self::stddev = 0;
                self::relative_stddev = 0;
                self::average_count = 0;
            }

        };
    template<class Name, class Func>
        int Task2<Name,Func>::cycles = 0;
    template<class Name, class Func>
        size_t Task2<Name,Func>::size = 0;
    template<class Name, class Func>
        double Task2<Name,Func>::mean = 0;
    template<class Name, class Func>
        double Task2<Name,Func>::variance = 0;
    template<class Name, class Func>
        double Task2<Name,Func>::stddev = 0;
    template<class Name, class Func>
        double Task2<Name,Func>::relative_stddev = 0;
    template<class Name, class Func>
        double Task2<Name,Func>::average_count = 0;

    template<class Name, class Func>
        Task2(Name, Func) -> Task2<Name, Func>;

    template<class init_func, class ...task_func>
        class IPS2 : init_func, task_func... {

            template<size_t ...I>
                constexpr void run(TimeUnit warmup, TimeUnit calc, bool is_tty, std::index_sequence<I...>) const {
                    ((([&]() constexpr {
                       const auto init = [this]() constexpr { init_func::operator()(); };
                       task_func::clean();
                       task_func::run_warmup(warmup, init);
                       task_func::run_calculate(calc, init);
                       report(I, is_tty, std::make_index_sequence<sizeof...(task_func)>());
                       })()),...);
                }

            template <size_t ...I>
                constexpr void report(size_t x, bool is_tty,std::index_sequence<I...>) const {
                    auto ftsk = fast_task(x, std::make_index_sequence<sizeof...(task_func)>());
                    auto stsk = slow_task(x, std::make_index_sequence<sizeof...(task_func)>());
                    auto lntsk = longest_name_task(x, std::make_index_sequence<sizeof...(task_func)>());
                    auto sz = snprintf(NULL, 0, "%5.2lf", (double)(ftsk) / stsk) - 3;

                    if (is_tty && x)
                        printf("\e[%zuA", x);


                    (((I <= x) && 
                      (([&]()constexpr ->bool{
                        printf("%*s", (int)lntsk, task_func::of::name);
                        auto m = std::log(task_func::mean) / std::log(10);

                        if (m < 3.0)
                        printf(" %6.2lf  (%6.2lfs )", task_func::mean, 1 / task_func::mean);
                        else if (m < 6.0)
                        printf(" %6.2lfk (%6.2lfms)", task_func::mean/1'000, 1'000 / task_func::mean);
                        else if (m < 9.0)
                        printf(" %6.2lfM (%6.2lfus)", task_func::mean/1'000'000, 1'000'000 / task_func::mean);
                        else
                        printf(" %6.2lfG (%6.2lfns)", task_func::mean/1'000'000'000, 1'000'000 / task_func::mean);

                        printf(" (±%5.2lf%%)", task_func::relative_stddev);

                        printf(" cmp: %14.2lf", task_func::average_count);

                        if (ftsk == task_func::size)
                        printf(" %*s fastest\n", sz + 3, "");
                        else
                        printf(" %*.2lf× slower\n", sz, (double)ftsk / task_func::size);

                        return true;
                      })())) &&...);

                }

            template <size_t...I>
                constexpr size_t fast_task(size_t x, std::index_sequence<I...>) const {
                    size_t sz = 0;
                    (((I <= x) && (((sz < task_func::size) && (sz = task_func::size)) ||true)) &&...);
                    return sz;
                }

            template <size_t...I>
                constexpr size_t slow_task(size_t x, std::index_sequence<I...>) const {
                    size_t sz = std::numeric_limits<size_t>::max();

                    (((I <= x) && (((sz > task_func::size) && (sz = task_func::size)) ||true)) &&...);
                    return sz;
                }

            template <size_t...I>
                constexpr size_t longest_name_task(size_t x, std::index_sequence<I...>) const {
                    size_t sz = 0;        
                    (((I <= x) && (((sz < sizeof(task_func::of::name)) && (sz = sizeof(task_func::of::name))) ||true)) &&...);
                    return sz;
                }
            public:

            IPS2(init_func init, task_func...tasks) : init_func(init), task_func(tasks)... {}

            constexpr void run(TimeUnit warmup, TimeUnit calc, bool is_tty = true) const {
                run(warmup, calc, is_tty, std::make_index_sequence<sizeof...(task_func)>());
            }
        };

    template <class init_func, class ...task_func>
        IPS2(init_func, task_func...) -> IPS2<init_func, task_func...>;

};

constexpr size_t size = 10'000'000;
std::array<int, size> arr;

int main() {
    using namespace std::chrono_literals;

    std::random_device rd;
    std::mt19937 gen(rd());

#if 1
    {
        printf("random integer array with the size %zu\n", size);
        using namespace Benchmark;

        IPS2 ips {
            [&gen]() {
                std::generate(arr.begin(), arr.end(), gen);
            },
                Task2 {
                    tname("fib shell sort"),
                    [](auto& cmp) { 
                        fib_shell_sort(arr.begin(), arr.end(), cmp);
                    }
                },
                Task2 {
                    tname("fib fuzzy shell sort"),
                    [](auto& cmp) {
                        FibFuzzyNS::shellsort(arr.begin(), arr.end(), cmp);
                    }
                },
                Task2 {
                    tname("std sort"),
                    [](auto& cmp) {
                        std::sort(arr.begin(), arr.end(), cmp);
                    }
                },
                Task2 {
                    tname("std sort heap"),
                    [](auto& cmp) { 
                        std::make_heap(arr.begin(), arr.end(), cmp);
                        std::sort_heap(arr.begin(), arr.end(), cmp); 
                    }
                },
                Task2 {
                    tname("std qsort"),
                    [](auto& cmp) {
                        std::qsort(arr.data(), size, sizeof(std::remove_pointer_t<decltype(arr.data())>),
                                [](const void *a, const void *b) {
                                return *static_cast<const int*>(a) - *static_cast<const int*>(b);
                                });
                    }
                }
        };

        ips.run(2s, 5s);

    }

    {
        printf("\nascend integer array with the size %zu\n", size);
        using namespace Benchmark;

        IPS2 ips {
            [](){
                for (int i = 0; i < size; i++)
                    arr[i] = i;
            },
                Task2 {
                    tname("fib shell sort"),
                    [](auto& cmp) { 
                        fib_shell_sort(arr.begin(), arr.end(), cmp);
                    }
                },
                Task2 {
                    tname("fib fuzzy shell sort"),
                    [](auto& cmp) {
                        FibFuzzyNS::shellsort(arr.begin(), arr.end(), cmp);
                    }
                },
                Task2 {
                    tname("std sort"),
                    [](auto& cmp) {
                        std::sort(arr.begin(), arr.end(), cmp);
                    }
                },
                Task2 {
                    tname("std sort heap"),
                    [](auto& cmp) { 
                        std::make_heap(arr.begin(), arr.end(), cmp);
                        std::sort_heap(arr.begin(), arr.end(), cmp); 
                    }
                },
                Task2 {
                    tname("std qsort"),
                    [](auto& cmp) {
                        std::qsort(arr.data(), size, sizeof(std::remove_pointer_t<decltype(arr.data())>),
                                [](const void *a, const void *b) {
                                return *static_cast<const int*>(a) - *static_cast<const int*>(b);
                                });
                    }
                }
        };

        ips.run(2s, 5s);
    }

    {
        printf("\ndescend integer array with the size %zu\n", size);
        using namespace Benchmark;

        IPS2 ips {
            [](){
                for (int i = 0; i < size; i++)
                    arr[i] = size - i;
            },
                Task2 {
                    tname("fib shell sort"),
                    [](auto& cmp) { 
                        fib_shell_sort(arr.begin(), arr.end(), cmp);
                    }
                },
                Task2 {
                    tname("fib fuzzy shell sort"),
                    [](auto& cmp) {
                        FibFuzzyNS::shellsort(arr.begin(), arr.end(), cmp);
                    }
                },
                Task2 {
                    tname("std sort"),
                    [](auto& cmp) {
                        std::sort(arr.begin(), arr.end(), cmp);
                    }
                },
                Task2 {
                    tname("std sort heap"),
                    [](auto& cmp) { 
                        std::make_heap(arr.begin(), arr.end(), cmp);
                        std::sort_heap(arr.begin(), arr.end(), cmp); 
                    }
                },
                Task2 {
                    tname("std qsort"),
                    [](auto& cmp) {
                        std::qsort(arr.data(), size, sizeof(std::remove_pointer_t<decltype(arr.data())>),
                                [](const void *a, const void *b) {
                                return *static_cast<const int*>(a) - *static_cast<const int*>(b);
                                });
                    }
                }

        };

        ips.run(2s, 5s);
    }

    {
        printf("\nCubic skew-like integer array with the size %zu\n", size);
        using namespace Benchmark;

        IPS2 ips {
            [&gen](){
                for (int i = 0; i < size; i++) {
                    double x = (2.0 * double(i) / size) - 1.0;
                    double v = x * x * x;
                    double w = (v + 1.0) / 2.0 * size + 1;

                    arr[i] = int(w);
                }

                std::shuffle(arr.begin(), arr.end(), gen);
            },
                Task2 {
                    tname("fib shell sort"),
                    [](auto& cmp) { 
                        fib_shell_sort(arr.begin(), arr.end(), cmp);
                    }
                },
                Task2 {
                    tname("fib fuzzy shell sort"),
                    [](auto& cmp) {
                        FibFuzzyNS::shellsort(arr.begin(), arr.end(), cmp);
                    }
                },
                Task2 {
                    tname("std sort"),
                    [](auto& cmp) {
                        std::sort(arr.begin(), arr.end(), cmp);
                    }
                },
                Task2 {
                    tname("std sort heap"),
                    [](auto& cmp) { 
                        std::make_heap(arr.begin(), arr.end(), cmp);
                        std::sort_heap(arr.begin(), arr.end(), cmp); 
                    }
                },
                Task2 {
                    tname("std qsort"),
                    [](auto& cmp) {
                        std::qsort(arr.data(), size, sizeof(std::remove_pointer_t<decltype(arr.data())>),
                                [](const void *a, const void *b) {
                                return *static_cast<const int*>(a) - *static_cast<const int*>(b);
                                });
                    }
                }

        };

        ips.run(2s, 5s);
    }

    {
        printf("\nQuinic skew-like integer array with the size %zu\n", size);
        using namespace Benchmark;

        IPS2 ips {
            [&gen](){
                for (int i = 0; i < size; i++) {
                    double x = (2.0 * double(i) / size) - 1.0;
                    double v = x * x * x * x * x;
                    double w = (v + 1.0) / 2.0 * size + 1;

                    arr[i] = int(w);
                }

                std::shuffle(arr.begin(), arr.end(), gen);
            },
                Task2 {
                    tname("fib shell sort"),
                    [](auto& cmp) { 
                        fib_shell_sort(arr.begin(), arr.end(), cmp);
                    }
                },
                Task2 {
                    tname("fib fuzzy shell sort"),
                    [](auto& cmp) {
                        FibFuzzyNS::shellsort(arr.begin(), arr.end(), cmp);
                    }
                },
                Task2 {
                    tname("std sort"),
                    [](auto& cmp) {
                        std::sort(arr.begin(), arr.end(), cmp);
                    }
                },
                Task2 {
                    tname("std sort heap"),
                    [](auto& cmp) { 
                        std::make_heap(arr.begin(), arr.end(), cmp);
                        std::sort_heap(arr.begin(), arr.end(), cmp); 
                    }
                },
                Task2 {
                    tname("std qsort"),
                    [](auto& cmp) {
                        std::qsort(arr.data(), size, sizeof(std::remove_pointer_t<decltype(arr.data())>),
                                [](const void *a, const void *b) {
                                return *static_cast<const int*>(a) - *static_cast<const int*>(b);
                                });
                    }
                }

        };

        ips.run(2s, 5s);
    }
#else
    {
        printf("Incemental size %zd started from 1024\n", size);

        using namespace Benchmark;

        for (size_t i = 1024; i <= size; i <<= 1) {
            printf("Array size %zu\n", i);

            IPS2 ips {
                [&gen, &i](){
                    std::generate(arr.begin(), arr.begin() + i, gen);

                },
                Task2 {
                    tname("fib shell sort"),
                    [&i](auto& cmp) { 
                        fib_shell_sort(arr.begin(), arr.begin() + i, cmp);
                    }
                },
                Task2 {
                    tname("fib fuzzy shell sort"),
                    [&i](auto& cmp) {
                        FibFuzzyNS::shellsort(arr.begin(), arr.begin() + i, cmp);
                    }
                },
                Task2 {
                    tname("std sort"),
                    [&i](auto& cmp) {
                        std::sort(arr.begin(), arr.begin() + i, cmp);
                    }
                },
                Task2 {
                    tname("std sort heap"),
                    [&i](auto& cmp) { 
                        std::make_heap(arr.begin(), arr.begin() + i, cmp);
                        std::sort_heap(arr.begin(), arr.begin() + i, cmp); 
                    }
                },
                Task2 {
                    tname("std qsort"),
                    [&i](auto& cmp) {
                        std::qsort(arr.data(), i, sizeof(std::remove_pointer_t<decltype(arr.data())>),
                                [](const void *a, const void *b) {
                                return *static_cast<const int*>(a) - *static_cast<const int*>(b);
                                });
                    }
                },
                Task2 {
                    tname("Proper Tokuda shell sort"),
                    [&i](auto &cmp) {
                        TokudaNS::shellsort(arr.begin(), arr.begin() + i, cmp);
                    }
                }
            };

            ips.run(2s, 5s);
        }

    }
#endif


    return 0;
}
