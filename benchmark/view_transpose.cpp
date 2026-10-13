// Copyright (C) 2020 Samuel Debionne, ESRF.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <benchmark/benchmark.h>

#include <boost/gil.hpp>

static void transpose(benchmark::State& state)
{
    using namespace boost::gil;

    size_t dim = state.range(0);

    gray8_image_t in(dim, dim);
    gray8_image_t out(dim, dim);

    for (auto _ : state) {
        // The code to benchmark
        auto transposed = transposed_view(const_view(in));
        copy_pixels(transposed, view(out));
    }
}
BENCHMARK(transpose)->RangeMultiplier(2)->Range(256, 8 << 10);

BENCHMARK_MAIN();
