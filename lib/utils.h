#pragma once

#include <array>
#include <bitset>
#include <string>

template <typename T>
inline consteval T bin_pow(T x, T n) {
    uint64_t res = 1;
    while (n) {
        if (n & 1) res *= x;
        x *= x;
        n >>= 1;
    }
    return res;
}

template <typename T>
inline T mod_add(const T& a, const T& b, const uint64_t& mod) {
    return uint64_t(a + b) > mod ? a + b - mod : (a + b);
}

template <typename T, uint16_t N = 64>
consteval std::array<T, N> get_primes() {
    auto is_prime = [](const T& num) {
        for (size_t i = 2; i * i <= num; ++i) {
            if (num % i == 0) {
                return false;
            }
        }

        return true;
    };

    std::array<T, N> primes;
    for (size_t num = 2, prime_count = 0; prime_count < N; ++num) {
        if (is_prime(num)) {
            primes[prime_count++] = num;
        }
    }

    return primes;
}