# parse-integers-benchmark
Benchmarking reading and parsing integers from a file in C++. The methods benchmarked are:

* `getchar-inline` - call `std::getchar` and parse each `int` char-by-char.
* `scanf` - call `std::scanf` to parse each `int`.
* `scanf-multi` - call `std::scanf` to parse 64 `int`s in one call.
* `iostream` - call `std::istream::operator>>(int&)` to parse each `int`. No `multi` version is possible.
* `mmap-inline` - `mmap` the entire file into memory and parse each `int` char-by-char.
* `mmap-charconv` - `mmap` the entire file into memory and parse each `int` using `std::from_chars`.

## Building and running
```
git clone https://github.com/max0x7ba/parse-integers-benchmark.git
cd parse-integers-benchmark
make -r run_parse_integers_fast
```

The benchmark compiles in C++17 mode by default. You can search-replace `gnu++17` to `gnu++11` in `Makefile` to make it work with C++11.

## Example outputs
##### Ubuntu 18.04.4 LTS, g++-8.3.0, Intel Core i9-9900KS CPU @ 5.1GHz, C++17 `charconv`.
```
---- Best times ----
seconds,    percent, method
0.123534698,  100.0, scanf
0.121618955,   98.4, iostream
0.104887812,   84.9, scanf-multi
0.047167165,   38.2, getchar-inline
0.030817239,   24.9, mmap-charconv
0.025470340,   20.6, mmap-inline
```
##### Ubuntu 18.04.2 LTS, g++-8.2.0, Intel Core i7-7700K CPU @ 5GHz
```
---- Best times ----
seconds,    percent, method
0.133155952,  100.0, iostream
0.102128208,   76.7, scanf
0.082469185,   61.9, scanf-multi
0.048661004,   36.5, getchar-inline
0.025320109,   19.0, mmap-inline
```
##### CentOS release 6.10, g++-6.3.0, Intel Core i7-4790 CPU @ 3.6GHz
```
---- Best times ----
seconds,    percent, method
0.167985515,  100.0, getchar-inline
0.147258495,   87.7, scanf
0.137161991,   81.7, iostream
0.118859546,   70.8, scanf-multi
0.034033769,   20.3, mmap-inline
```

## Notes
* `mmap` is a relatively expensive operation so that it may only be faster for relatively large files. Benchmark on your file size.
* `getchar-inline` and `mmap-inline` use a similar inline method of parsing an `int` from a stream of `char`, so that the time difference can be attributed to the latency of accessing the next `char`. `mmap` method maps the entire file into memory and reads it sequentially, which is the best case scenario for the CPU memory prefetcher.
