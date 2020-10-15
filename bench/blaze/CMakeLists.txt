# Copyright (C) 2020 Samuel Debionne, ESRF.

# Use, modification and distribution is subject to the Boost Software
# License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

add_executable(bench_blaze_transpose
    bench_transpose.cpp
)

target_link_libraries(bench_blaze_transpose
    benchmark::benchmark
    blaze::blaze
)

target_compile_definitions(bench_blaze_transpose
    PRIVATE BLAZE_USE_SHARED_MEMORY_PARALLELIZATION=0
)

add_executable(bench_blaze_transpose_mt
    bench_transpose.cpp
)

target_link_libraries(bench_blaze_transpose_mt
    benchmark::benchmark
    blaze::blaze
)
