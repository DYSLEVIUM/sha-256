cmake_minimum_required(VERSION 3.10)

project(sha256)

# set C++ standard to C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add executable and source files
add_executable(sha256 src/main.cc src/sha256.cc)

# set include directories
target_include_directories(sha256 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(sha256 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# set compiler flags
if(MSVC)
    target_compile_options(sha256 PRIVATE /W4 /WX)
else()
    # target_compile_options(sha256 PRIVATE -Wall -Wextra -pedantic -Werror -fconstexpr-ops-limit=4294967296)
    target_compile_options(sha256 PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
