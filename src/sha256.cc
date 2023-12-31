// this file is also known as a translation unit
#include "SHA256/sha256.h"

#include <array>
#include <bitset>
/* #include <format> */
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

std::array<uint32_t, 8> SHA256::compress(
    const std::array<uint32_t, 64>& message_schedule,
    const std::array<uint32_t, 8>& current_hashes) {
    std::array<uint32_t, 8> new_hashes = current_hashes;

    for (size_t i = 0; const auto& word : message_schedule) {
        int32_t t1 =
            SHA256::u_sigma1(new_hashes[4]) +
            SHA256::choice(new_hashes[4], new_hashes[5], new_hashes[6]) +
            new_hashes[7] + SHA256::constants[i++] + word;
        int32_t t2 =
            SHA256::u_sigma0(new_hashes[0]) +
            SHA256::majority(new_hashes[0], new_hashes[1], new_hashes[2]);

        // shift the state registers
        for (size_t j = 7; j > 0; --j) {
            new_hashes[j] = new_hashes[j - 1];
        }

        new_hashes[4] += t1;
        new_hashes[0] = t1 + t2;
    }

    for (size_t i = 0; auto& hash : new_hashes) {
        hash += current_hashes[i++];
    }

    return new_hashes;
}

std::array<uint32_t, 64> SHA256::get_message_schedule(
    const std::array<uint8_t, 64>& message_block) {
    std::array<uint32_t, 64> message_schedule{};

    // Split data in 32 bit blocks for the 16 first words
    for (size_t i = 0, j = 0; i < 16; ++i, j += 4) {
        for (size_t k = 0; k < 4; ++k) {
            message_schedule[i] |=
                message_block[j + k]
                << (8 * (3 - k));  // shifting and doing logical-or
        }
    }

    for (size_t i : std::views::iota(16, 64)) {
        message_schedule[i] = SHA256::l_sigma1(message_schedule[i - 2]) +
                              message_schedule[i - 7] +
                              SHA256::l_sigma0(message_schedule[i - 15]) +
                              message_schedule[i - 16];
    }

    return message_schedule;
}

std::string SHA256::hash(
    const std::vector<std::array<uint8_t, 64>>& message_blocks) {
    std::array<uint32_t, 8> hashes = SHA256::initial_hashes;

    for (size_t i = 0; const auto& message_block : message_blocks) {
        std::cout << "\rCompressing for message_block " << ++i
                  << "; Percentage Done: "
                  << uint16_t(100 * i / message_blocks.size()) << char('%')
                  << std::flush;

        auto message_schedule = SHA256::get_message_schedule(message_block);
        hashes = SHA256::compress(message_schedule, hashes);
    }

    std::cout << '\n';

    std::ostringstream os;
    for (const auto& hash : hashes) {
        /* std::cout << std::format("{:#08x}\n", hash) << '\n'; */
        os << std::setw(8) << std::setfill('0') << std::right << std::hex
           << hash;
    }

    return os.str();
}

void SHA256::add_size_to_final_block(std::array<uint8_t, 64>& final_block,
                                     const uint64_t& bit_len) {
    for (size_t i : std::views::iota(0, 8)) {
        final_block[63 - i] = bit_len >> (i * 8);
    }
}

std::vector<std::array<uint8_t, 64>> SHA256::get_message_blocks(
    const std::string& message) {
    size_t num_blocks = message.size() / 64 + 1;

    std::vector<std::array<uint8_t, 64>>
        message_blocks;  // vector of 64 word blocks which is of type 8-bit
                         // array
    message_blocks.reserve(num_blocks);

    uint16_t block_len = 0;
    uint64_t bit_len = 0;

    for (size_t i = 0; i < message.size(); ++i) {
        std::array<uint8_t, 64> block{};

        size_t k = i;
        for (; k < message.size() && block_len < 64; ++k) {
            block[block_len++] = std::bitset<8>(message[k]).to_ulong();
        }

        if (block_len == 64) {  // full block
            message_blocks.push_back(block);

            bit_len += 512;
            block_len = 0;

            // our block is entirely completely filled, we need another block to
            // add the size
            if (k == message.size()) {
                std::array<uint8_t, 64> final_block{};

                final_block[0] = (1 << 7);  // append 1 to mark the end; 0x80

                SHA256::add_size_to_final_block(final_block, bit_len);
                message_blocks.push_back(final_block);
            }
        } else {  // partially filled block
            size_t te = block_len;

            block[te++] = (1 << 7);  // append 1 to mark the end; 0x80

            std::array<uint8_t, 64> final_block{};

            if (block_len >= 56) {
                message_blocks.push_back(block);
                final_block = {};
            } else {
                final_block = block;
            }

            bit_len += block_len * 8;

            SHA256::add_size_to_final_block(final_block, bit_len);
            message_blocks.push_back(final_block);
        }
        i = k - 1;
    }

    std::cout << "Generated " << message_blocks.size() << " message blocks\n";

    return message_blocks;
}

std::string SHA256::hash(const std::string& message) {
    std::cout << "Converting to message_blocks\n";

    auto message_blocks = SHA256::get_message_blocks(message);
    return SHA256::hash(message_blocks);
}

std::string SHA256::hash(const std::ifstream& fin) {
    if (!fin.is_open()) {
        std::cerr << "Error opening file.";
        return {};
    }

    std::ostringstream os;
    os << fin.rdbuf();

    return SHA256::hash(os.str());
}
