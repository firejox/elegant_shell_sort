# Elegant Shell Sort

Elegant Shell Sort is [Shell Sort](https://en.wikipedia.org/wiki/Shellsort) 
with the gaps of Fibonacci sequences and its variant yields amazing performance. 
the lower bound of the average performance is **Omega(nlg n)** according to Jiang-Li-Vitányi's formula.
Although the upper bound of the average complexity is open question, it may be an option for effiecient sort.

---

## Visualization in sound of sort
<a href="http://www.youtube.com/watch?feature=player_embedded&v=3mi2YKuYX9M
" target="_blank"><img src="http://img.youtube.com/vi/3mi2YKuYX9M/0.jpg" 
alt="hyber fibonacci shell sort" width="240" height="180" border="10" /></a>

---

## Performance compare with std::sort and std::sort\_heap

```sh
[firejox@myhostname shellsort]$ clang++ -o FibShellSort FibShellSort.cpp --std=c++17 -O3
[firejox@myhostname shellsort]$ ./FibShellSort
random integer array with the size 10000000
       fib shell sort   3.10  (  0.32s ) (± 0.01%) cmp:  1337115240.00 3.50× slower
 fib fuzzy shell sort   1.85  (  0.54s ) (± 2.73%) cmp:   578302716.00 2.33× slowert
             std sort   0.73  (  1.36s ) (± 3.93%) cmp:   276433631.00       fastest
        std sort heap   3.89  (  0.26s ) (± 0.47%) cmp:   236298999.00 3.50× slower
            std qsort   1.69  (  0.59s ) (± 0.31%) cmp:           0.00 2.33× slower

ascend integer array with the size 10000000
       fib shell sort   0.58  (  1.72s ) (± 4.05%) cmp:   315842185.00 2.22× slower
 fib fuzzy shell sort   0.49  (  2.03s ) (± 1.39%) cmp:   295842191.00 1.82× slowert
             std sort   0.26  (  3.83s ) (± 0.42%) cmp:   316531837.00       fastest
        std sort heap   0.65  (  1.55s ) (± 0.24%) cmp:   238288048.00 2.50× slower
            std qsort   0.69  (  1.44s ) (± 1.45%) cmp:           0.00 2.50× slower

descend integer array with the size 10000000
       fib shell sort   0.66  (  1.52s ) (± 1.17%) cmp:   353359827.00 3.38× slower
 fib fuzzy shell sort   0.56  (  1.79s ) (± 0.59%) cmp:   339869035.00 3.00× slowert
             std sort   0.19  (  5.27s ) (± 2.52%) cmp:   222097162.00       fastest
        std sort heap   0.73  (  1.36s ) (± 0.19%) cmp:   240430389.00 3.86× slower
            std qsort   1.12  (  0.90s ) (± 2.57%) cmp:           0.00 5.40× slower

Cubic skew-like integer array with the size 10000000
       fib shell sort   3.11  (  0.32s ) (± 0.19%) cmp:  1230233729.00 4.00× slower
 fib fuzzy shell sort   1.73  (  0.58s ) (± 0.21%) cmp:   560553610.67 2.67× slowert
             std sort   0.69  (  1.44s ) (± 1.09%) cmp:   273028994.25       fastest
        std sort heap   3.83  (  0.26s ) (± 0.01%) cmp:   236299615.50 4.00× slower
            std qsort   1.74  (  0.58s ) (± 0.35%) cmp:           0.00 2.67× slower

Quinic skew-like integer array with the size 10000000
       fib shell sort   3.87  (  0.26s ) (±21.14%) cmp:  1186601595.00 4.00× slower
 fib fuzzy shell sort   1.75  (  0.57s ) (± 3.54%) cmp:   533883723.67 2.67× slowert
             std sort   0.66  (  1.52s ) (± 0.36%) cmp:   264888812.38       fastest
        std sort heap   3.67  (  0.27s ) (± 0.45%) cmp:   236283666.50 4.00× slower
            std qsort   1.72  (  0.58s ) (± 0.72%) cmp:           0.00 2.67× slower
```
