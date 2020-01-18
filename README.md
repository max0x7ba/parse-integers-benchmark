# parse-integers-benchmark
Benchmarking reading and parsing integers from a file in C++.

## Building and running
```
cd parse-integers-benchmark
make -r run_parse_integers_fast
```

## Example outputs
##### CentOS release 6.10, g++-6.3.0, Intel Core i7-4790 CPU @ 3.6GHz
```
---- Best times ----
seconds,    percent, method
0.167985515,  100.0, getchar
0.147258495,   87.7, scanf
0.137161991,   81.7, iostream
0.118859546,   70.8, scanf-multi
0.034033769,   20.3, mmap-parse-faster
```
##### Ubuntu 18.04.2 LTS, g++-8.2.0, Intel Core i7-7700K CPU @ 5GHz
```
---- Best times ----
seconds,    percent, method
0.133155952,  100.0, iostream
0.102128208,   76.7, scanf
0.082469185,   61.9, scanf-multi
0.048661004,   36.5, getchar
0.025320109,   19.0, mmap-parse-faster
```
##### Ubuntu 18.04.3 LTS, g++-8.3.0, Intel Core i9-9900K CPU @ 5GHz
```
---- Best times ----
seconds,    percent, method
0.129434717,  100.0, iostream
0.126812376,   98.0, scanf
0.106872285,   82.6, scanf-multi
0.054889884,   42.4, getchar
0.025161949,   19.4, mmap-parse-faster
```
