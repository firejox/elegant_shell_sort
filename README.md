# Elegant Shell Sort

Elegant Shell Sort is [Shell Sort](https://en.wikipedia.org/wiki/Shellsort) 
with the gaps of Fibonacci sequences. ~~The worst case performance is O(n(lg n)^2)~~  
(unclarified) and the lower bound of the average performance is **Omega(nlg n)** according to Jiang-Li-Vitányi's formula.
Although the upper bound of the average complexity is open question, it may be an option for effiecient sort.

---

## Visualization in sound of sort
<a href="http://www.youtube.com/watch?feature=player_embedded&v=3mi2YKuYX9M
" target="_blank"><img src="http://img.youtube.com/vi/3mi2YKuYX9M/0.jpg" 
alt="hyber fibonacci shell sort" width="240" height="180" border="10" /></a>

---

## Performance compare with std::sort and std::sort\_heap

```sh
[firejox@myhostname shellsort]$ clang++ -o FibShellSort FibShellSort.cpp --std=c++17
[firejox@myhostname shellsort]$ ./FibShellSort
random integer array with the size 1000000
  fib shell sort   1.44  (  0.69s ) (± 0.87%) 2.25× slower
 fib shell sort2   1.37  (  0.73s ) (± 0.10%) 2.25× slower
hyfib shell sort   1.31  (  0.76s ) (± 0.05%) 2.25× slower
        std sort   0.58  (  1.71s ) (± 2.94%)      fastest
   std sort heap   1.06  (  0.94s ) (± 0.12%) 1.80× slower

ascend integer array with the size 1000000
  fib shell sort   0.31  (  3.27s ) (± 0.18%) 3.06× slower
 fib shell sort2   0.16  (  6.26s ) (± 1.00%) 1.62× slower
hyfib shell sort   0.10  ( 10.39s ) (± 1.12%)      fastest
        std sort   0.46  (  2.15s ) (± 0.79%) 4.73× slower
   std sort heap   0.89  (  1.12s ) (± 0.36%) 8.67× slower

descend integer array with the size 1000000
  fib shell sort   0.41  (  2.47s ) (± 0.35%) 1.31× slower
 fib shell sort2   0.30  (  3.38s ) (± 0.24%)      fastest
hyfib shell sort   0.32  (  3.08s ) (± 0.90%) 1.06× slower
        std sort   0.37  (  2.72s ) (± 1.15%) 1.21× slower
   std sort heap   0.93  (  1.08s ) (± 0.47%) 2.83× slower

Cubic skew-like integer array with the size 1000000
  fib shell sort   1.48  (  0.68s ) (± 2.98%) 2.25× slower
 fib shell sort2   1.39  (  0.72s ) (± 2.16%) 2.25× slower
hyfib shell sort   1.32  (  0.76s ) (± 1.14%) 2.25× slower
        std sort   0.56  (  1.78s ) (± 0.83%)      fastest
   std sort heap   1.06  (  0.94s ) (± 0.25%) 1.80× slower

Quinic skew-like integer array with the size 1000000
  fib shell sort   1.33  (  0.75s ) (± 1.02%) 2.50× slower
 fib shell sort2   1.26  (  0.79s ) (± 1.27%) 2.50× slower
hyfib shell sort   1.17  (  0.86s ) (± 1.11%) 2.00× slower
        std sort   0.55  (  1.81s ) (± 1.15%)      fastest
   std sort heap   1.08  (  0.93s ) (± 2.05%) 2.00× slower

```
