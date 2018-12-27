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

namespace FibNS {
    using namespace IntegralConstantArithmetic;
    
    template <class N, size_t sz>
    struct Number {
        using type = add_t<typename Number<N, sz - 1>::type, typename Number<N, sz - 2>::type>;
    };

    template <class N>
    struct Number<N, 0> {
        using type = IC<N, 1>;
    };

    template <class N>
    struct Number<N, 1> {
        using type = IC<N, 2>;
    };
    
    template <class Numeric = int>
    class Sequence {
        // Fibonacci number generator
        //
        
        static constexpr size_t table_size() {
            constexpr Numeric max_num = std::numeric_limits<Numeric>::max();
            Numeric f0(1), f1(2), f2(1);

            size_t sz = 2;
            while ((max_num - f1) >= f0) {
                f2 = f1 + f0;
                f0 = f1;
                f1 = f2;
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
                        auto m = task_func::mean;

                        if (m < 1.e3)
                        printf(" %6.2lf  (%6.2lfs )", task_func::mean, 1 / task_func::mean);
                        else if (m < 1.e6)
                        printf(" %6.2lfk (%6.2lfms)", task_func::mean / 1.e3, 1.e3 / task_func::mean);
                        else if (m < 1.e9)
                        printf(" %6.2lfM (%6.2lfus)", task_func::mean / 1.e6, 1.e6 / task_func::mean);
                        else
                        printf(" %6.2lfG (%6.2lfns)", task_func::mean / 1.e9, 1.e9 / task_func::mean);

                        printf(" (±%5.2lf%%)", task_func::relative_stddev);

                        printf(" cmp: %14.2lf", task_func::average_count);

                        if (ftsk == task_func::size)
                            printf(" %*s fastest\n", sz + 3, "");
                        else
                            printf(" %*.2lf× slower \n", sz, (double)ftsk / task_func::size);

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
                        FibNS::shellsort(arr.begin(), arr.end(), cmp);
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
                        FibNS::shellsort(arr.begin(), arr.end(), cmp);
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
                        FibNS::shellsort(arr.begin(), arr.end(), cmp);
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
                        FibNS::shellsort(arr.begin(), arr.end(), cmp);
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
                        FibNS::shellsort(arr.begin(), arr.end(), cmp);
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
                        FibNS::shellsort(arr.begin(), arr.begin() + i, cmp);
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
