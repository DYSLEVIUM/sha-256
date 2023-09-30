# not able to build for clang with make
# cmake . && make

# this one works for my machine, installed g++ using brew 🙃
/opt/homebrew/bin/g++-13 --std=c++23 -I ./include -I ./lib src/main.cc ./src/sha256.cc -o sha256

# dd if=/dev/urandom of=rand bs=10M count=10
# ./sha256 "pushpa" -f "rand"
# cat rand | xargs -0 ./sha256 # -0 tells input tokens are NUll-terminated
