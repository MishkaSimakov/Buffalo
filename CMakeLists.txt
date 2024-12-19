cmake_minimum_required(VERSION 3.22)
project(Buffalo)

if (POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif ()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-fsanitize=address")

add_subdirectory(src)

include(FetchContent)

# install fmt
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
FetchContent_MakeAvailable(fmt)

# install GTEST
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_subdirectory(tests)

# main solution
add_executable(Solution main.cpp)
target_link_libraries(Solution PUBLIC Buffalo)

# grammar building executable
add_executable(BuildGrammar build_grammar.cpp)
target_compile_definitions(BuildGrammar PUBLIC COMPILING_GRAMMAR)
target_link_libraries(BuildGrammar PUBLIC Buffalo)
