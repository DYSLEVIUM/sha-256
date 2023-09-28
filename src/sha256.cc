// this file is also known as a translation unit
#include "SHA256/sha256.h"

#include <array>
#include <bitset>
#include <fstream>
#include <iostream>
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

std::string SHA256::hash(
    const std::vector<std::array<uint8_t, 64>>& message_blocks) {
    std::array<uint32_t, 8> hashes = SHA256::initial_hashes;

    for (const auto& message_block : message_blocks) {
        std::array<uint32_t, 64> message_schedule{};

        // Split data in 32 bit blocks for the 16 first words
        for (size_t i = 0, j = 0; i < 16; ++i, j += 4) {
            for (size_t k = 0; k < 4; ++k) {
                message_schedule[i] |=
                    message_block[j + k]
                    << (8 * (3 - k));  // shifting and doing logical-or
            }
        }

        for (size_t i = 16; i < 64; ++i) {
            message_schedule[i] = SHA256::l_sigma1(message_schedule[i - 2]) +
                                  message_schedule[i - 7] +
                                  SHA256::l_sigma0(message_schedule[i - 15]) +
                                  message_schedule[i - 16];
        }

        hashes = SHA256::compress(message_schedule, hashes);
    }

    std::ostringstream os;
    for (const auto& hash : hashes) {
        os << std::hex << hash;
    }

    return os.str();
}

void SHA256::add_size_to_final_block(std::array<uint8_t, 64>& final_block,
                                     const uint64_t& bit_len) {
    for (size_t i = 0; i < 8; ++i) {
        final_block[63 - i] = bit_len >> (i * 8);
    }
}

std::string SHA256::hash(const std::string& message) {
    size_t num_blocks = message.size() / 512 + 1;

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

                size_t te = 0;

                final_block[te++] = (1 << 7);  // append 1 to mark the end; 0x80

                // initial values is 0, the bottom loop is redundant
                // while (te < 56) {
                //     final_block[te++] = 0;  // pad with 0
                // }

                SHA256::add_size_to_final_block(final_block, bit_len);
                message_blocks.push_back(final_block);
            }
        } else {  // partially filled block
            size_t te = block_len;
            // size_t end = block_len < 56 ? 56 : 64;

            block[te++] = (1 << 7);  // append 1 to mark the end; 0x80

            // initial values is 0, the bottom loop is redundant
            // while (te < end) {
            //     block[te++] = 0;  // pad with 0
            // }

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
