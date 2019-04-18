# parse-integers-benchmark
Benchmarking reading and parsing integers from a file in C++.

## Building and running
```
cd parse-integers-benchmark
make -r run_parse_integers_fast
```

## Example output
```
---- Best times ----
seconds,     method
0.167762524, getchar
0.147397999, scanf
0.136970085, iostream
0.118754395, scanf-multi
0.033481586, mmap-parse-faster
```