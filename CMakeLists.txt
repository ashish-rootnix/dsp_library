cmake_minimum_required(VERSION 3.25)
project(dsp_lib
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "DSP C++ Library"
)

# -- C++23 Standard --
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #for clangd / IDE support

# -- Sanitizer Options --

# Sanitizer is a special instrument added by the compiler that
# Injects extra checking code
# Detects runtime bugs
# Creashes program immedietly when bug occurs
# Prints detailed stack trace
# Supported by GCC Clang

# ASAN - Address sanitizer => -fsanitizer=address => 
# Use after free, buffer overflow, Heap Overflow, Stack Overflow, 
# Double delete, Memory Leaks

# TSAN - Thread Sanitizer => -fsanitizer=thread => Data races, Unsafe concurrent access

# UBSAN - Undefined Behaviour Sanitizer => -fsanitizer=undefined
# Single integer overflow, Null pointer derefrence, Invalid casts, Misaligned pointer, shift overflow

set(SANITIZER "" CACHE STRING "Sanetizer: asan, tsan, or empty")

if(SANITIZER STREQUAL "asan")
    message(STATUS "Building with ASan + UBsan")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g)
    add_link_options(-fsanitize=address,undefined)
elseif(SANITIZER STREQUAL "tsan")    
    message(STATUS "Building with TSan")
    add_compile_options(-fsanitize=thread -g)
    add_link_options(-fsanitize=thread)
endif()

add_compile_options(
    -Wall -Wextra 
    -Wpedantic          # Non ISO standard C++ constructs
    -Werror             # forces to fix evry single warning
    -Wshadow            # Variable declaration hides another variable in outer scope
    -Wnon-virtual-dtor
    -Wold-style-cast    # C-style casts are used instead C++ standard casts
    -Wcast-align        # cast increases alignment requirements
    -Wunused            # Unused variables
    -Woverloaded-virtual# Derived class hides a base class function instead overriding 
    -Wconversion        # catches silent narrow conversions - impicit conversions
    -Wsign-conversion   # catches silent narrow conversions - impicit conversions 
    -Wnull-dereference # null derefrencing is detected at compile time
    -Wdouble-promotion  # float is implicitly promoted to double
    -Wformat=2          # Enable strict format string checking for printf/scanf
)

# -- Core Library --
add_subdirectory(src)

# -- Testing --
enable_testing()

include(FetchContent)
FetchContent_Declare(
        GoogleTest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
)
FetchContent_MakeAvailable(GoogleTest)

# -- Test's helper functions --
function(add_dsp_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME}
        PRIVATE
            dsp_core
            GTest::gtest_main # provide main(), just write TEST()
    )
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

add_subdirectory(tests)
