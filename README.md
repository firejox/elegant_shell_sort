# Elegant Shell Sort

Elegant Shell Sort is [Shell Sort](https://en.wikipedia.org/wiki/Shellsort) 
with the gaps of Fibonacci sequences. The worst case performance is **O(n(lg n)^2)** 
and the lower bound of the average performance is **Omega(nlg n)** according to Jiang-Li-Vitányi's formula.
Although the upper bound of the average complexity is open question, it may be an option for effiecient sort.

---

## Visualization in sound of sort
<a href="http://www.youtube.com/watch?feature=player_embedded&v=3mi2YKuYX9M
" target="_blank"><img src="http://img.youtube.com/vi/3mi2YKuYX9M/0.jpg" 
alt="hyber fibonacci shell sort" width="240" height="180" border="10" /></a>

---

## Performance compare with std::sort and std::sort\_heap

```
clang verion 7.0.0 -O3

  fib shell sort   0.19  (  5.33s ) (± 0.38%) 2.96× slower
 fib shell sort2   0.15  (  6.70s ) (± 0.14%) 2.35× slower
hyfib shell sort   0.15  (  6.58s ) (± 0.30%) 2.42× slower
        std sort   0.06  ( 15.82s ) (± 0.39%)      fastest
   std sort heap   0.21  (  4.82s ) (± 0.23%) 3.20× slower

```
