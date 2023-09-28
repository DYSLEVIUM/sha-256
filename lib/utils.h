#pragma once

#include <array>
#include <bitset>
#include <ranges>
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
    size_t prime_count = 0;
    // for (size_t num = 2; prime_count < N; ++num) {
    for (size_t num : std::views::iota(2) | std::views::filter(is_prime)) {
        if (prime_count == N) {
            break;
        }
        primes[prime_count++] = num;
    }

    return primes;
}