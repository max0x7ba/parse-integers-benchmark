/* -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
#include <algorithm>
#include <iostream>
#include <iterator>
#include <fstream>
#include <chrono>
#include <cstdio>

#if __cplusplus >= 201703L
#include <charconv>
#endif

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

// https://stackoverflow.com/a/43870188/412080
#define   LIKELY(condition) __builtin_expect(static_cast<bool>(condition), 1)
#define UNLIKELY(condition) __builtin_expect(static_cast<bool>(condition), 0)

char const random_ints_filename[] = "random_ints_1600000.txt";
constexpr int ELEMENTS = 1600000;

void method_istream(int(&a)[ELEMENTS]) {
    std::ios::sync_with_stdio(false);
    int n, m;
    std::cin >> n >> m;
    for(int i = 0; i < n; ++i)
        std::cin >> a[i];
}

void method_scanf(int(&a)[ELEMENTS]) {
    int n, m;
    if(UNLIKELY(2 != std::scanf("%d %d", &n, &m)))
        throw;
    for(int i = 0; i < n; ++i)
        if(UNLIKELY(1 != std::scanf("%d", a + i)))
            throw;
}

constexpr int step = 64;
char const fmt[step * 3] =
    "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d "
    "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d "
    "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d "
    "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d"
    ;

void method_scanf_multi(int(&a)[ELEMENTS]) {
    int n, m;
    if(UNLIKELY(2 != std::scanf("%d %d", &n, &m)))
        throw;

    for(int i = 0; i < n; i += step) {
        int expected = step < n - i ? step : n - i;
        int* b = a + i;
        int read = scanf(fmt + 3 * (step - expected),
                         b + 0x00, b + 0x01, b + 0x02, b + 0x03, b + 0x04, b + 0x05, b + 0x06, b + 0x07,
                         b + 0x08, b + 0x09, b + 0x0a, b + 0x0b, b + 0x0c, b + 0x0d, b + 0x0e, b + 0x0f,
                         b + 0x10, b + 0x11, b + 0x12, b + 0x13, b + 0x14, b + 0x15, b + 0x16, b + 0x17,
                         b + 0x18, b + 0x19, b + 0x1a, b + 0x1b, b + 0x1c, b + 0x1d, b + 0x1e, b + 0x1f,
                         b + 0x20, b + 0x21, b + 0x22, b + 0x23, b + 0x24, b + 0x25, b + 0x26, b + 0x27,
                         b + 0x28, b + 0x29, b + 0x2a, b + 0x2b, b + 0x2c, b + 0x2d, b + 0x2e, b + 0x2f,
                         b + 0x30, b + 0x31, b + 0x32, b + 0x33, b + 0x34, b + 0x35, b + 0x36, b + 0x37,
                         b + 0x38, b + 0x39, b + 0x3a, b + 0x3b, b + 0x3c, b + 0x3d, b + 0x3e, b + 0x3f);
        if(UNLIKELY(read != expected))
            throw;
    }
}

void method_getchar_inline(int(&a)[ELEMENTS]) {
    int n, m;
    if(UNLIKELY(2 != std::scanf("%d %d", &n, &m)))
        throw;

    for(int i = 0; i < n; ++i) {
        int r = std::getchar();
        while(std::isspace(r))
            r = std::getchar();
        bool neg = false;
        if('-' == r) {
            neg = true;
            r = std::getchar();
        }
        r -= '0';
        for(;;) {
            int s = std::getchar();
            if(!std::isdigit(s))
                break;
            r = r * 10 + (s - '0');
        }
        a[i] = neg ? -r : r;
    }
}

inline int int_parse_inline(char const*& begin, char const* end) {
    char const* p = begin;
    while(p != end && std::isspace(*p))
        ++p;
    if(UNLIKELY(p == end))
        throw;
    bool neg = *p == '-';
    p += neg;
    int r = 0;
    do {
        unsigned c = *p - '0';
        if(c >= 10)
            break;
        r = r * 10 + static_cast<int>(c);
    } while(++p != end);
    begin = p;
    return neg ? -r : r;
}

template<int(*parse_int)(char const*&, char const*)>
inline void method_mmap_(int(&a)[ELEMENTS]) {
    int fd = open(random_ints_filename, O_RDONLY);
    if(UNLIKELY(fd == -1))
        throw;
    struct stat s;
    if(UNLIKELY(fstat(fd, &s)))
        throw;
    void* pfd = mmap(nullptr, s.st_size, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);
    if(UNLIKELY(pfd == MAP_FAILED))
        throw;

    char const* begin = static_cast<char const*>(pfd);
    char const* end = begin + s.st_size;

    int n = parse_int(begin, end);
    int m = parse_int(begin, end);
    static_cast<void>(m);

    for(int i = 0; i < n; ++i)
        a[i] = parse_int(begin, end);
}

void method_mmap_parse_inline(int(&a)[ELEMENTS]) {
    method_mmap_<int_parse_inline>(a);
}

#if __cplusplus >= 201703L
inline int int_parse_charconv(char const*& begin, char const* end) {
    int value;
    while(begin != end && std::isspace(*begin))
        ++begin;
    auto r = std::from_chars(begin, end, value);
    if(UNLIKELY(static_cast<unsigned>(r.ec)))
        throw;
    begin = r.ptr;
    return value;
}

void method_mmap_parse_charconv(int(&a)[ELEMENTS]) {
    method_mmap_<int_parse_charconv>(a);
}
#endif

unsigned reverse_crc32(int const* begin, int const* end) {
    unsigned r = 0;
    while(begin != end)
        r = __builtin_ia32_crc32si(r, *--end);
    return r;
}

decltype(&method_istream) const methods[] = {
      &method_istream
    , &method_scanf
    , &method_scanf_multi
    , &method_getchar_inline
    , &method_mmap_parse_inline
#if __cplusplus >= 201703L
    , &method_mmap_parse_charconv
#endif
};
char const* names[] = {
      "iostream"
    , "scanf"
    , "scanf-multi"
    , "getchar-inline"
    , "mmap-parse-inline"
#if __cplusplus >= 201703L
    , "mmap-parse-charconv"
#endif
};
constexpr int method_count = sizeof methods / sizeof *methods;

int main() {
    constexpr int RUNS = 10;

    printf("method,checksum,seconds\n");
    double best_times[method_count];
    for(int method = 0; method < method_count; ++method) {
        for(int run = 0; run < RUNS; ++run) {
            std::ifstream random_ints(random_ints_filename);
            auto old_buf = std::cin.rdbuf(random_ints.rdbuf());
            if(UNLIKELY(!freopen(random_ints_filename, "rb", stdin)))
                throw;

            auto t0 = std::chrono::high_resolution_clock::now();
            int a[ELEMENTS];
            methods[method](a);
            auto t1 = std::chrono::high_resolution_clock::now();

            // Make sure the array and the computations are not optimized away.
            unsigned sum = reverse_crc32(a, a + sizeof a / sizeof *a);

            double time = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0).count();
            std::printf("%s,%u,%.9f\n", names[method], sum, time);
            if(!run || time < best_times[method])
                best_times[method] = time;

            std::cin.rdbuf(old_buf);
        }
    }

    std::printf("\n---- Best times ----\n");
    std::printf("seconds,    percent, method\n");
    int indexes[method_count];
    for(int i = 0; i < method_count; ++i)
        indexes[i] = i;
    std::sort(indexes, indexes + method_count, [&best_times](int a, int b) { return best_times[a] > best_times[b]; });
    int worst = indexes[0];
    for(int method = 0; method < method_count; ++method) {
        int i = indexes[method];
        std::printf("%.9f, %6.1f, %s\n", best_times[i], 1e2 * best_times[i] / best_times[worst], names[i]);
    }
}
