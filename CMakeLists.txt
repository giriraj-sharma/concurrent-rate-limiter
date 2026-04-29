cmake_minimum_required(VERSION 3.14)
project(RateLimiter VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMMON_SOURCES
    src/algorithms/fixed_window.cpp
    src/algorithms/sliding_window.cpp
    src/core/rate_limiter_service.cpp
    src/core/config.cpp
)

add_executable(rate_limiter src/main.cpp ${COMMON_SOURCES})
target_include_directories(rate_limiter PRIVATE src include)
target_link_libraries(rate_limiter PRIVATE pthread)

add_executable(stress_test tests/stress_test.cpp ${COMMON_SOURCES})
target_include_directories(stress_test PRIVATE src include)
target_link_libraries(stress_test PRIVATE pthread)
