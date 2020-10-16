# Copyright (C) 2020 Samuel Debionne, ESRF.

# Use, modification and distribution is subject to the Boost Software
# License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

add_executable(bench_ipp_transpose
    bench_transpose.cpp
)

target_link_libraries(bench_ipp_transpose
    benchmark::benchmark
    ipp::ipp
)


add_executable(test_ipp_transpose
    test_transpose.cpp
)

target_link_libraries(test_ipp_transpose
    ipp::ipp
)