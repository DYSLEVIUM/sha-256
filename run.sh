# not able to build for clang with make
# cmake . && make && ./sha256

# this one works for my machine, installed g++ using brew 🙃
/opt/homebrew/bin/g++-13 --std=c++23 -I ./include -I ./lib src/main.cc ./src/sha256.cc -o sha256
