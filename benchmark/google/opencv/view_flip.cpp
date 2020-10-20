// Copyright (C) 2020 Samuel Debionne, ESRF.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <benchmark/benchmark.h>

#include <opencv2/core/core.hpp>

static void flip_up_down(benchmark::State& state)
{
    using namespace cv;

    int dim = state.range(0);

    Mat1b in(dim, dim);
    Mat1b out(dim, dim);

    for (auto _ : state) {
        // The code to benchmark
        flip(in,out,0);
    }
}
BENCHMARK(flip_up_down)->RangeMultiplier(2)->Range(256, 8 << 10);

static void flip_left_right(benchmark::State& state)
{
    using namespace cv;

    int dim = state.range(0);

    Mat1b in(dim, dim);
    Mat1b out(dim, dim);

    for (auto _ : state) {
        // The code to benchmark
        flip(in,out,1);
    }
}
BENCHMARK(flip_left_right)->RangeMultiplier(2)->Range(256, 8 << 10);

BENCHMARK_MAIN();
