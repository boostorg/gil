// Copyright (C) 2020 Samuel Debionne, ESRF.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <benchmark/benchmark.h>

#include <boost/gil.hpp>

static void rotation_cc90(benchmark::State& state)
{
    using namespace boost::gil;

    size_t dim = state.range(0);

    gray8_image_t in(dim, dim);
    gray8_image_t out(dim, dim);

    for (auto _ : state) {
        // The code to benchmark
        auto rotated = rotated90cw_view(const_view(in));
        copy_pixels(rotated, view(out));
    }
}
BENCHMARK(rotation_cc90)->RangeMultiplier(2)->Range(256, 8 << 10);

static void rotation_cc90_v2(benchmark::State& state)
{
    using namespace boost::gil;

    size_t dim = state.range(0);

    gray8_image_t in(dim, dim);
    gray8_image_t out(dim, dim);

    for (auto _ : state) {
        // The code to benchmark
        auto transposed = transposed_view(const_view(in));
        copy_pixels(transposed, view(out));
        auto flipped = flipped_left_right_view(const_view(out));
        copy_pixels(flipped, view(out));
    }
}
BENCHMARK(rotation_cc90_v2)->RangeMultiplier(2)->Range(256, 8 << 10);

static void rotation_ccw90(benchmark::State& state)
{
    using namespace boost::gil;

    size_t dim = state.range(0);

    gray8_image_t in(dim, dim);
    gray8_image_t out(dim, dim);

    for (auto _ : state) {
        // The code to benchmark
        auto rotated = rotated90ccw_view(const_view(in));
        copy_pixels(rotated, view(out));
    }
}
BENCHMARK(rotation_ccw90)->RangeMultiplier(2)->Range(256, 8 << 10);

static void rotation_ccw90_v2(benchmark::State& state)
{
    using namespace boost::gil;

    size_t dim = state.range(0);

    gray8_image_t in(dim, dim);
    gray8_image_t out(dim, dim);

    for (auto _ : state) {
        // The code to benchmark
        auto transposed = transposed_view(const_view(in));
        copy_pixels(transposed, view(out));
        auto flipped = flipped_up_down_view(const_view(out));
        copy_pixels(flipped, view(out));
    }
}
BENCHMARK(rotation_ccw90_v2)->RangeMultiplier(2)->Range(256, 8 << 10);

static void rotation_180(benchmark::State& state)
{
    using namespace boost::gil;

    size_t dim = state.range(0);

    gray8_image_t in(dim, dim);
    gray8_image_t out(dim, dim);

    for (auto _ : state) {
        // The code to benchmark
        auto rotated = rotated180_view(const_view(in));
        copy_pixels(rotated, view(out));
    }
}
BENCHMARK(rotation_180)->RangeMultiplier(2)->Range(256, 8 << 10);

BENCHMARK_MAIN();
