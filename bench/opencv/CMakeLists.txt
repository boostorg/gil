# Copyright (C) 2020 Samuel Debionne, ESRF.

# Use, modification and distribution is subject to the Boost Software
# License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

add_executable(bench_rotation
  bench_rotation.cpp
)

target_link_libraries(bench_rotation
    benchmark::benchmark
    ${OpenCV_LIBS}
)

target_include_directories(bench_rotation
    ${OpenCV_INCLUDE_DIRS}
)

add_executable(bench_flip
  bench_flip.cpp
)

target_link_libraries(bench_flip
    benchmark::benchmark
    ${OpenCV_LIBS}
)

target_include_directories(bench_flip
    ${OpenCV_INCLUDE_DIRS}
)
