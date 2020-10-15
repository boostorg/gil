# Copyright (C) 2020 Samuel Debionne, ESRF.

# Use, modification and distribution is subject to the Boost Software
# License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

add_executable(bench_rotation
    bench_rotation.cpp
)

target_link_libraries(bench_rotation
    benchmark::benchmark
)

add_executable(bench_flip
  bench_flip.cpp
)

target_link_libraries(bench_flip
    benchmark::benchmark
)

add_executable(bench_transpose
  bench_transpose.cpp
)

target_link_libraries(bench_transpose
    benchmark::benchmark
)

add_executable(bench_transpose_impl
  bench_transpose_impl.cpp
)

target_link_libraries(bench_transpose_impl
    benchmark::benchmark
)

if(BUILD_BENCHMARK_OPENCV)
    find_package(OpenCV REQUIRED)
    add_subdirectory(opencv)
endif()

if(BUILD_BENCHMARK_BLAZE)
    find_package(blaze REQUIRED)
    add_subdirectory(blaze)
endif()