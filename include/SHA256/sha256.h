/*
The keyword static has several different meanings in C++, and the code you've
written above uses them in two different ways.

In the context of member functions, static means "this member function does not
have a receiver object. It's basically a normal function that's nested inside of
the scope of the class."

In the context of function declarations, static means "this function is scoped
only to this file and can't be called from other places."
*/

/*
Note: In most cases you want the definitions of the template member functions
accessible in the header. If the definitions are in the header, remember to mark
them as inline (or define them inside the template class definition), if they
are not, think twice and make sure that you do not need the definitions in the
header (you do not need the definitions in a header if you explicitly
instantiate in the translation unit that contains the definition for all types
with which you want to use the template).
*/

/*

Initializing a variable with static storage duration might result in two
outcomes¹:

    The variable is initialized at compile-time (constant-initialization);

    The variable is initialized the first time control passes through its
declaration.

Case (2) is problematic because it can lead to the static initialization order
fiasco, which is a source of dangerous bugs related to global objects.

The constinit keyword can only be applied on variables with static storage
duration. If the decorated variable is not initialized at compile-time, the
program is ill-formed (i.e. does not compile).

Using constinit ensures that the variable is initialized at compile-time, and
that the static initialization order fiasco cannot take place.
*/

// https://stackoverflow.com/questions/45183324/whats-the-difference-between-static-constexpr-and-static-inline-variables-in-c

#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <functional>
#include <string>

#include "utils.h"

class SHA256 {
    // static makes the function available for the current translation unit
    static inline uint32_t shift_right(const uint32_t&, const uint8_t& = 1);
    static inline uint32_t right_rotate(const uint32_t&, const uint8_t& = 1);

    static inline uint32_t Xor(const uint32_t&);

    template <typename... Args>
    static inline uint32_t Xor(const uint32_t&, const Args&...);

    static inline uint32_t add(const uint32_t&);

    template <typename... Args>
    static inline uint32_t add(const uint32_t&, const Args&...);

    // compound functions
    static inline uint32_t sigma(
        const uint32_t&,
        const std::array<
            std::pair<std::function<uint32_t(const uint32_t&, const uint8_t&)>,
                      uint8_t>,
            3>&);

    // lower sigma
    static inline uint32_t l_sigma0(const uint32_t&);  // σ0
    static inline uint32_t l_sigma1(const uint32_t&);  // σ1

    // upper sigma
    static inline uint32_t u_sigma0(const uint32_t&);  // Σ0
    static inline uint32_t u_sigma1(const uint32_t&);  // Σ1

    static inline uint32_t choice(const uint32_t&, const uint32_t&,
                                  const uint32_t&);
    static inline uint32_t majority(const uint32_t&, const uint32_t&,
                                    const uint32_t&);

    // get 64 prime numbers at compile time
    static constexpr auto primes = get_primes<uint64_t>();

    // constants -> first 32 bits of the cube_root of prime numbers
    // make constants at compile-time
    static constexpr auto const constants = []() {
        std::array<uint32_t, 64> results;
        for (size_t i = 0; auto& result : results) {
            double cube_root = std::cbrt(primes[i++]);
            uint32_t constant = uint64_t(cube_root * bin_pow<uint64_t>(2, 32)) %
                                bin_pow<uint64_t>(2, 32);
            result = constant;
        }

        return results;
    }();

    // initial hashes -> first 32 bits of the square_root of prime numbers
    static constexpr auto const initial_hashes = []() {
        std::array<uint32_t, 8> results;
        for (size_t i = 0; auto& result : results) {
            double square_root = std::sqrt(primes[i++]);
            uint32_t hash = uint64_t(square_root * bin_pow<uint64_t>(2, 32)) %
                            bin_pow<uint64_t>(2, 32);
            result = hash;
        }

        return results;
    }();

    // static constexpr auto const states = SHA256::hashes;

    static const uint8_t BLOCK_SIZE = (512 / 8);
    static const uint8_t DIGEST_SIZE = (256 / 8);

    static void add_size_to_final_block(std::array<uint8_t, 64>&,
                                        const uint64_t&);

    static std::vector<std::array<uint8_t, 64>> get_message_blocks(
        const std::string&);

    static std::string hash(const std::vector<std::array<uint8_t, 64>>&);

    static std::array<uint32_t, 8> compress(const std::array<uint32_t, 64>&,
                                            const std::array<uint32_t, 8>&);

   public:
    static std ::string hash(const std::string&);
    static std ::string hash(const std::ifstream&);
};

// would have been better to put all the functions in a helper file, but
// this was done for learning how to use translation unit with header files
// when i keep the following functions in another file, i need to remove the
// inline for them to be static also static definition in class is different
// from the static in the header

inline uint32_t SHA256::shift_right(const uint32_t& value,
                                    const uint8_t& shift) {
    return (value >> shift);
}

inline uint32_t SHA256::right_rotate(const uint32_t& value,
                                     const uint8_t& shift) {
    return ((value >> shift) | (value << (32 - shift)));
}

inline uint32_t SHA256::Xor(const uint32_t& identity) { return identity; }

template <typename... Args>
inline uint32_t SHA256::Xor(const uint32_t& value, const Args&... args) {
    return value ^ Xor(args...);
}

inline uint32_t SHA256::add(const uint32_t& identity) { return identity; }

template <typename... Args>
inline uint32_t SHA256::add(const uint32_t& value, const Args&... args) {
    return mod_add(value, add(args...), bin_pow<uint32_t>(2, 32));
}

inline uint32_t SHA256::sigma(
    const uint32_t& value,
    const std::array<
        std::pair<std::function<uint32_t(const uint32_t&, const uint8_t&)>,
                  uint8_t>,
        3>& funcs_params) {
    std::array<uint32_t, 3> res;
    for (size_t i = 0; auto& [func, param] : funcs_params) {
        res[i++] = func(value, param);
    }

    //! find a way to unpack the array to the variadic function
    return SHA256::Xor(res[0], res[1], res[2]);
}

inline uint32_t SHA256::l_sigma0(const uint32_t& value) {
    std::array<
        std::pair<std::function<uint32_t(const uint32_t&, const uint8_t&)>,
                  uint8_t>,
        3>
        funcs_params{
            std::make_pair(
                std::function<uint32_t(const uint32_t&, const uint8_t&)>(
                    SHA256::right_rotate),
                7),
            std::make_pair(
                std::function<uint32_t(const uint32_t&, const uint8_t&)>(
                    SHA256::right_rotate),
                18),
            std::make_pair(
                std::function<uint32_t(const uint32_t&, const uint8_t&)>(
                    SHA256::shift_right),
                3),
        };

    return SHA256::sigma(value, funcs_params);
}

inline uint32_t SHA256::l_sigma1(const uint32_t& value) {
    std::array<
        std::pair<std::function<uint32_t(const uint32_t&, const uint8_t&)>,
                  uint8_t>,
        3>
        funcs_params{
            std::make_pair(
                std::function<uint32_t(const uint32_t&, const uint8_t&)>(
                    SHA256::right_rotate),
                17),
            std::make_pair(
                std::function<uint32_t(const uint32_t&, const uint8_t&)>(
                    SHA256::right_rotate),
                19),
            std::make_pair(
                std::function<uint32_t(const uint32_t&, const uint8_t&)>(
                    SHA256::shift_right),
                10),
        };

    return SHA256::sigma(value, funcs_params);
}

inline uint32_t SHA256::u_sigma0(const uint32_t& value) {
    std::array<
        std::pair<std::function<uint32_t(const uint32_t&, const uint8_t&)>,
                  uint8_t>,
        3>
        funcs_params{
            std::make_pair(
                std::function<uint32_t(const uint32_t&, const uint8_t&)>(
                    SHA256::right_rotate),
                2),
            std::make_pair(
                std::function<uint32_t(const uint32_t&, const uint8_t&)>(
                    SHA256::right_rotate),
                13),
            std::make_pair(
                std::function<uint32_t(const uint32_t&, const uint8_t&)>(
                    SHA256::right_rotate),
                22),
        };

    return SHA256::sigma(value, funcs_params);
}

inline uint32_t SHA256::u_sigma1(const uint32_t& value) {
    std::array<
        std::pair<std::function<uint32_t(const uint32_t&, const uint8_t&)>,
                  uint8_t>,
        3>
        funcs_params{
            std::make_pair(
                std::function<uint32_t(const uint32_t&, const uint8_t&)>(
                    SHA256::right_rotate),
                6),
            std::make_pair(
                std::function<uint32_t(const uint32_t&, const uint8_t&)>(
                    SHA256::right_rotate),
                11),
            std::make_pair(
                std::function<uint32_t(const uint32_t&, const uint8_t&)>(
                    SHA256::right_rotate),
                25),
        };

    return SHA256::sigma(value, funcs_params);
}

// based on value_1 bit; either take the value_2 bit or the value_3 bit
inline uint32_t SHA256::choice(const uint32_t& value_1, const uint32_t& value_2,
                               const uint32_t& value_3) {
    return SHA256::Xor(value_1 & value_2, (~value_1) & value_3);
}

inline uint32_t SHA256::majority(const uint32_t& value_1,
                                 const uint32_t& value_2,
                                 const uint32_t& value_3) {
    return SHA256::Xor(value_1 & value_2, value_1 & value_3, value_2 & value_3);
}
