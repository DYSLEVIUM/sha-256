// Resources
// https://codeforces.com/blog/entry/79941
// https://www.youtube.com/watch?v=UdwdJWQ5o78
// https://www.cppstories.com/2022/const-options-cpp20/
// https://news.ycombinator.com/item?id=28504635

#pragma GCC optimize("Ofast,fast-math,unroll-loops")

#include <bitset>
#include <cstdint>
#include <fstream>
#include <iostream>

#include "SHA256/sha256.h"

typedef std::bitset<32> bits32;

int main() {
    std::cout << SHA256::hash("abc") << '\n';
    std::cout << SHA256::hash(std::ifstream("./test.txt")) << '\n';
    return 0;
}
