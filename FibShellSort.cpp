#include <algorithm>
#include <functional>
#include <iterator>
#include <limits>
#include <array>
#include <random>
#include <cstdio>
#include <chrono>
#include <functional>
#include <string>
#include <ratio>
#include <cmath>
#include <memory>

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

    for (auto k = std::lower_bound(table.begin(), table.end(), current_array_size - one); k > table.begin(); k--) {
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

    for (; k >= table.begin(); k-= fib_step, insert_lim += fib_step) {
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

        if (gap == one)
            break;

        auto lim = *insert_lim;
        auto i = first + one;
        for (; i < last; i++) {
            if (comp(*i, *(i - one))) {
                typename std::iterator_traits<It>::value_type v = std::move(*i);
                auto j = i;

                do {
                    *j = std::move(*(j - one));
                    j--;
                    lim--;
                } while ((j > first && lim > zero) && comp(v, *(j - one)));

                *j = std::move(v);

                if (lim == zero)
                    break;
            }
        }

        if (i == last)
            return;

        lim = *insert_lim;

        for (i = last - one; i > first; i--) {
            if (comp(*i, *(i - one))) {
                typename std::iterator_traits<It>::value_type v = std::move(*i);
                auto j = i;

                do {
                    *j = std::move(*(j - one));
                    j++;
                    lim--;
                } while ((j < (last - one) && lim > zero) && comp(*(j + one), v));

                *j = std::move(v);

                if (lim == zero)
                    break;
            }
        }

        if (i == first)
            return;
    }
}

namespace Benchmark {
    using TimeUnit = std::chrono::duration<double, std::milli>;
    using DeciSecond = std::chrono::duration<double, std::deci>;
    using Second = std::chrono::duration<double>;

    struct Task {
        struct Impl {
            std::string name;
            std::function<void(Task&)> action;


            int cycles = 0;
            size_t size = 0;
            double mean = 0.0;
            double variance = 0.0;
            double stddev = 0.0;
            double relative_stddev = 0.0;
            
            size_t counter = 0;
            double average_count = 0.0;

            bool finished = false;
        };

        std::shared_ptr<Impl> self;

        template <class T>
        bool operator()(const T &a, const T &b) {
            self->counter ++;
            return a < b;
        }

        void run_warmup(TimeUnit warmup, std::function<void()> &init) {
            int count = 0;
            TimeUnit total(0);

            while (warmup.count() > 0) {
                init();

                auto before = std::chrono::high_resolution_clock::now();
                
                self->action(*this);
                
                auto after = std::chrono::high_resolution_clock::now();
                
                auto tmp = std::chrono::duration_cast<TimeUnit>(after - before);
                
                total += tmp;
                warmup -= tmp;
                count++;
            }

            self->cycles = std::max(1, int(count / std::chrono::duration_cast<DeciSecond>(total).count()));
        }

        void run_calculate(TimeUnit calc_time, std::function<void()> &init) {
            std::vector<Second> measure;
            std::vector<double> count;

            measure.reserve(size_t (self->cycles * std::chrono::duration_cast<DeciSecond>(calc_time).count()));
            count.reserve(size_t (self->cycles * std::chrono::duration_cast<DeciSecond>(calc_time).count()));


            do {
                TimeUnit subtotal(0);
                self->counter = 0;

                for (int i = 0; i < self->cycles; i++) {
                    init();

                    auto before = std::chrono::high_resolution_clock::now();
                    
                    self->action(*this);
                    
                    auto after = std::chrono::high_resolution_clock::now();

                    subtotal += std::chrono::duration_cast<TimeUnit>(after - before);
                }

                calc_time -= subtotal;

                count.push_back((double)self->counter / self->cycles);
                measure.push_back(std::chrono::duration_cast<Second>(subtotal));
            } while(calc_time.count() > 0);

            self->finished = true;
            self->size = measure.size();
            self->mean = std::accumulate(measure.begin(), measure.end(), 0.0, [](double a, Second b) {
                        return a + b.count();
                    }) / self->size;
            self->variance = std::accumulate(measure.begin(), measure.end(), 0.0, [this](double a, Second b) {
                        double tmp = b.count() - this->self->mean;
                        return double(a + tmp * tmp);
                    }) / self->size;
            self->stddev = std::sqrt(self->variance);
            self->relative_stddev = 100.0 * (self->stddev / self->mean);
            self->average_count = std::accumulate(count.begin(), count.end(), 0.0) / self->size;
        }
    };
    
    class IPS {
        TimeUnit warmup;
        TimeUnit calc_time;
        std::function<void()> initialize;
        std::vector<Task> tasks;
    public:

        template <class Func=std::function<void()>, class ...Args>
        IPS(TimeUnit warmup, TimeUnit calc_time, Func f, Args... args) : 
            warmup(warmup), calc_time(calc_time), initialize(f), tasks{Task{std::shared_ptr<Task::Impl>(new Task::Impl{std::get<0>(args), std::get<1>(args)})}...} {}

        void run() {
            for (auto&& task : tasks) {
                task.run_warmup(warmup, initialize);
                task.run_calculate(calc_time, initialize);

                report();
            }
        }

        void report() {
            auto fastest = std::max_element(tasks.begin(), tasks.end(), [](Task &a, Task&b) {
                    if (a.self->finished)
                        return b.self->finished && (a.self->size < b.self->size);
                    return false;
                });

            auto longest_name = std::max_element(tasks.begin(), tasks.end(), [](Task &a, Task&b) {
                    if (a.self->finished)
                        return b.self->finished && (a.self->name.size() < b.self->name.size());
                    return false;
                });

            auto slowest = std::max_element(tasks.begin(), tasks.end(), [](Task &a, Task&b) {
                    if (a.self->finished)
                        return b.self->finished && a.self->size > b.self->size;
                    return false;
                });

            auto sz = snprintf(NULL, 0, "%5.2lf", (double)(fastest->self->size) / slowest->self->size) - 3;

            auto count = std::count_if(tasks.begin(), tasks.end(), [](Task &a) { return a.self->finished; });

            if (count > 1)
                printf("\e[%ldA", count-1);

            for (auto&& task : tasks) {
                if (!task.self->finished)
                    break;
                printf("%*s", (int)longest_name->self->name.size(), task.self->name.c_str());
                
                auto m = std::log(task.self->mean) / std::log(10);
                if (m < 3.0)
                    printf(" %6.2lf  (%6.2lfs )", task.self->mean, 1 / task.self->mean);
                else if (m < 6.0)
                    printf(" %6.2lfk (%6.2lfms)", task.self->mean/1'000, 1'000 / task.self->mean);
                else if (m < 9.0)
                    printf(" %6.2lfM (%6.2lfus)", task.self->mean/1'000'000, 1'000'000 / task.self->mean);
                else
                    printf(" %6.2lfG (%6.2lfns)", task.self->mean/1'000'000'000, 1'000'000 / task.self->mean);

                printf(" (±%5.2lf%%)", task.self->relative_stddev);

                // printf(" cmp: %10.2lf", task.self->average_count);
                
                if (std::addressof(*fastest) == std::addressof(task))
                    printf(" %*sfastest\n", sz + 3, "");
                else
                    printf(" %*.2lf× slower\n", sz, (double)fastest->self->size / task.self->size);
            }
        }
    };
};


int main() {
    using namespace std::chrono_literals;
    
    std::random_device rd;
    std::mt19937 gen(rd());

    constexpr size_t size = 1000000;
    std::array<int, size> arr;
    
    {
        printf("random integer array with the size %zu\n", size);

        Benchmark::IPS ips { 2s, 5s,
            [&arr, &gen]() {
                std::generate(arr.begin(), arr.end(), gen);
            },
            std::make_tuple(
                    "fib shell sort", 
                    [&arr](Benchmark::Task& cmp) { 
                    fib_shell_sort(arr.begin(), arr.end(), cmp);
                    }),
            std::make_tuple(
                    "fib shell sort2",
                    [&arr](Benchmark::Task& cmp) {
                    fib_shell_sort2(arr.begin(), arr.end(), cmp);
                    }),
            std::make_tuple(
                    "hyfib shell sort",
                    [&arr](Benchmark::Task& cmp) {
                    hyfib_shell_sort(arr.begin(), arr.end(), cmp);
                    }),
            std::make_tuple(
                    "std sort",
                    [&arr](Benchmark::Task& cmp) {
                    std::sort(arr.begin(), arr.end(), cmp);
                    }),
            std::make_tuple(
                    "std sort heap",
                    [&arr](Benchmark::Task& cmp) { 
                    std::make_heap(arr.begin(), arr.end(), cmp);
                    std::sort_heap(arr.begin(), arr.end(), cmp); 
                    })};

        ips.run();
    }
 
    {
        printf("\nascend integer array with the size %zu\n", size);

        Benchmark::IPS ips { 2s, 5s,
            [&arr](){
                for (int i = 0; i < size; i++)
                    arr[i] = i;
            },
            std::make_tuple(
                    "fib shell sort", 
                    [&arr](Benchmark::Task& cmp) { 
                    fib_shell_sort(arr.begin(), arr.end(), cmp);
                    }),
            std::make_tuple(
                    "fib shell sort2",
                    [&arr](Benchmark::Task& cmp) {
                    fib_shell_sort2(arr.begin(), arr.end(), cmp);
                    }),
            std::make_tuple(
                    "hyfib shell sort",
                    [&arr](Benchmark::Task& cmp) {
                    hyfib_shell_sort(arr.begin(), arr.end(), cmp);
                    }),
            std::make_tuple(
                    "std sort",
                    [&arr](Benchmark::Task& cmp) {
                    std::sort(arr.begin(), arr.end(), cmp);
                    }),
            std::make_tuple(
                    "std sort heap",
                    [&arr](Benchmark::Task& cmp) { 
                    std::make_heap(arr.begin(), arr.end(), cmp);
                    std::sort_heap(arr.begin(), arr.end(), cmp); 
                    })};

        ips.run();
    }
    
    {
        printf("\ndescend integer array with the size %zu\n", size);

        Benchmark::IPS ips { 2s, 5s,
            [&arr](){
                for (int i = 0; i < size; i++)
                    arr[i] = size - i;
            },
            std::make_tuple(
                    "fib shell sort", 
                    [&arr](Benchmark::Task& cmp) { 
                    fib_shell_sort(arr.begin(), arr.end(), cmp);
                    }),
            std::make_tuple(
                    "fib shell sort2",
                    [&arr](Benchmark::Task& cmp) {
                    fib_shell_sort2(arr.begin(), arr.end(), cmp);
                    }),
            std::make_tuple(
                    "hyfib shell sort",
                    [&arr](Benchmark::Task& cmp) {
                    hyfib_shell_sort(arr.begin(), arr.end(), cmp);
                    }),
            std::make_tuple(
                    "std sort",
                    [&arr](Benchmark::Task& cmp) {
                    std::sort(arr.begin(), arr.end(), cmp);
                    }),
            std::make_tuple(
                    "std sort heap",
                    [&arr](Benchmark::Task& cmp) { 
                    std::make_heap(arr.begin(), arr.end(), cmp);
                    std::sort_heap(arr.begin(), arr.end(), cmp); 
                    })};

        ips.run();
    }

    {
        printf("\nCubic skew-like integer array with the size %zu\n", size);

        Benchmark::IPS ips { 2s, 5s,
            [&arr, &gen](){
                for (int i = 0; i < size; i++) {
                    double x = (2.0 * double(i) / size) - 1.0;
                    double v = x * x * x;
                    double w = (v + 1.0) / 2.0 * size + 1;

                    arr[i] = int(w);
                }

                std::shuffle(arr.begin(), arr.end(), gen);
            },
            std::make_tuple(
                    "fib shell sort", 
                    [&arr](Benchmark::Task& cmp) { 
                    fib_shell_sort(arr.begin(), arr.end(), cmp);
                    }),
            std::make_tuple(
                    "fib shell sort2",
                    [&arr](Benchmark::Task& cmp) {
                    fib_shell_sort2(arr.begin(), arr.end(), cmp);
                    }),
            std::make_tuple(
                    "hyfib shell sort",
                    [&arr](Benchmark::Task& cmp) {
                    hyfib_shell_sort(arr.begin(), arr.end(), cmp);
                    }),
            std::make_tuple(
                    "std sort",
                    [&arr](Benchmark::Task& cmp) {
                    std::sort(arr.begin(), arr.end(), cmp);
                    }),
            std::make_tuple(
                    "std sort heap",
                    [&arr](Benchmark::Task& cmp) { 
                    std::make_heap(arr.begin(), arr.end(), cmp);
                    std::sort_heap(arr.begin(), arr.end(), cmp); 
                    })};

        ips.run();
    }

    {
        printf("\nQuinic skew-like integer array with the size %zu\n", size);

        Benchmark::IPS ips { 2s, 5s,
            [&arr, &gen](){
                for (int i = 0; i < size; i++) {
                    double x = (2.0 * double(i) / size) - 1.0;
                    double v = x * x * x * x * x;
                    double w = (v + 1.0) / 2.0 * size + 1;

                    arr[i] = int(w);
                }

                std::shuffle(arr.begin(), arr.end(), gen);
            },
            std::make_tuple(
                    "fib shell sort", 
                    [&arr](Benchmark::Task& cmp) { 
                    fib_shell_sort(arr.begin(), arr.end(), cmp);
                    }),
            std::make_tuple(
                    "fib shell sort2",
                    [&arr](Benchmark::Task& cmp) {
                    fib_shell_sort2(arr.begin(), arr.end(), cmp);
                    }),
            std::make_tuple(
                    "hyfib shell sort",
                    [&arr](Benchmark::Task& cmp) {
                    hyfib_shell_sort(arr.begin(), arr.end(), cmp);
                    }),
            std::make_tuple(
                    "std sort",
                    [&arr](Benchmark::Task& cmp) {
                    std::sort(arr.begin(), arr.end(), cmp);
                    }),
            std::make_tuple(
                    "std sort heap",
                    [&arr](Benchmark::Task& cmp) { 
                    std::make_heap(arr.begin(), arr.end(), cmp);
                    std::sort_heap(arr.begin(), arr.end(), cmp); 
                    })};

        ips.run();
    }


    return 0;
}
