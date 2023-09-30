// Resources
// https://codeforces.com/blog/entry/79941
// https://www.youtube.com/watch?v=UdwdJWQ5o78
// https://www.cppstories.com/2022/const-options-cpp20/
// https://news.ycombinator.com/item?id=28504635

#pragma GCC optimize("Ofast,fast-math,unroll-loops")

#include <cstring>
#include <fstream>
#include <iostream>

#include "SHA256/sha256.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Number of arguments is invalid!";
        return -1;
    }

    size_t file_number = 0;
    for (size_t i = 1; (int16_t)i < argc; ++i) {
        std::string hash;
        if (!strcmp(argv[i], "-f")) {
            if ((int16_t)i + 1 == argc) {
                std::cerr << "No file was provided";
                return -1;
            }

            std::cout << "Starting Hash for file: " << argv[++i] << '\n';

            std::ifstream fin(argv[i]);
            hash = SHA256::hash(fin);
            fin.close();

        } else {
            std::cout << "Starting Hash for string: " << argv[i] << '\n';

            hash = SHA256::hash(argv[i]);
        }

        std::cout << "Hash for " << file_number << " is: " << hash << '\n';
        std::cout << '\n';
        ++file_number;
    }

    return 0;
}
