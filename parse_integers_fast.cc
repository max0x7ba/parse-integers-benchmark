/* -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
#include <algorithm>
#include <iostream>
#include <iterator>
#include <fstream>
#include <chrono>
#include <cstdio>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

char const random_ints_filename[] = "random_ints_1600000.txt";

void method_istream(int(&a)[1600000]) {
    std::ios::sync_with_stdio(false);
    int n, m;
    std::cin >> n >> m;
    for(int i = 0; i < n; ++i)
        std::cin >> a[i];
}

void method_scanf(int(&a)[1600000]) {
    int n, m;
    if(2 != std::scanf("%d %d", &n, &m))
        throw;
    for(int i = 0; i < n; ++i)
        if(1 != std::scanf("%d", a + i))
            throw;
}

constexpr int step = 64;
char const fmt[step * 3] =
    "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d "
    "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d "
    "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d "
    "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d"
    ;

void method_scanf_multi(int(&a)[1600000]) {
    int n, m;
    if(2 != std::scanf("%d %d", &n, &m))
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
        if(read != expected)
            throw;
    }
}

void method_getchar(int(&a)[1600000]) {
    int n, m;
    if(2 != std::scanf("%d %d", &n, &m))
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

inline int find_and_parse_int(char const*& begin, char const* end) {
    char const* p = begin;
    while(p != end && std::isspace(*p))
        ++p;
    if(p == end)
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

void method_mmap_parse_faster(int(&a)[1600000]) {
    int fd = open(random_ints_filename, O_RDONLY);
    if(fd == -1)
        throw;
    struct stat s;
    if(fstat(fd, &s))
        throw;
    void* pfd = mmap(nullptr, s.st_size, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);
    if(pfd == MAP_FAILED)
        throw;

    char const* begin = static_cast<char const*>(pfd);
    char const* end = begin + s.st_size;

    int n = find_and_parse_int(begin, end);
    int m = find_and_parse_int(begin, end);
    static_cast<void>(m);

    for(int i = 0; i < n; ++i)
        a[i] = find_and_parse_int(begin, end);
}

unsigned reverse_crc32(int const* begin, int const* end) {
    unsigned r = 0;
    while(begin != end)
        r = __builtin_ia32_crc32si(r, *--end);
    return r;
}

decltype(&method_istream) const methods[] = {&method_istream, &method_scanf, &method_scanf_multi, &method_getchar, &method_mmap_parse_faster};
char const* names[] = {"iostream", "scanf", "scanf-multi", "getchar", "mmap-parse-faster"};
constexpr int method_count = sizeof methods / sizeof *methods;

int main() {
    constexpr int RUNS = 10;

    printf("method,checksum,seconds\n");
    double best_times[method_count];
    for(int method = 0; method < method_count; ++method) {
        for(int run = 0; run < RUNS; ++run) {
            std::ifstream random_ints(random_ints_filename);
            auto old_buf = std::cin.rdbuf(random_ints.rdbuf());
            freopen(random_ints_filename, "rb", stdin);

            auto t0 = std::chrono::high_resolution_clock::now();
            int a[1600000];
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
    std::printf("seconds,     method\n");
    int indexes[method_count];
    for(int i = 0; i < method_count; ++i)
        indexes[i] = i;
    std::sort(indexes, indexes + method_count, [&best_times](int a, int b) { return best_times[a] > best_times[b]; });
    for(int method = 0; method < method_count; ++method)
        std::printf("%.9f, %s\n", best_times[indexes[method]], names[indexes[method]]);
}
