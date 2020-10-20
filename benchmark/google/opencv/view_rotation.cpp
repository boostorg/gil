// Copyright (C) 2020 Samuel Debionne, ESRF.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <benchmark/benchmark.h>

#include <opencv2/core/core.hpp>

static void rotation_cc90(benchmark::State& state)
{
    using namespace cv;

    size_t dim = state.range(0);

    Mat1b in(dim, dim);
    Mat1b out(dim, dim);

    for (auto _ : state) {
        // The code to benchmark
        rotate(in,out,ROTATE_90_CLOCKWISE);
    }
}
BENCHMARK(rotation_cc90)->RangeMultiplier(2)->Range(256, 8 << 10);

static void rotation_ccw90(benchmark::State& state)
{
    using namespace cv;

    size_t dim = state.range(0);

    Mat1b in(dim, dim);
    Mat1b out(dim, dim);

    for (auto _ : state) {
        // The code to benchmark
        rotate(in,out,ROTATE_90_COUNTERCLOCKWISE);
    }
}
BENCHMARK(rotation_ccw90)->RangeMultiplier(2)->Range(256, 8 << 10);

static void rotation_180(benchmark::State& state)
{
    using namespace cv;

    size_t dim = state.range(0);

    Mat1b in(dim, dim);
    Mat1b out(dim, dim);

    for (auto _ : state) {
        // The code to benchmark
        rotate(in,out,ROTATE_180);
    }
}
BENCHMARK(rotation_180)->RangeMultiplier(2)->Range(256, 8 << 10);

BENCHMARK_MAIN();
