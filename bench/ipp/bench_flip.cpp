// Copyright (C) 2020 Samuel Debionne, ESRF.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <benchmark/benchmark.h>

#include <ipp.h>

#include <boost/gil.hpp>

static void ipp_flip_left_right(benchmark::State& state)
{
    using namespace boost::gil;

    size_t dim = state.range(0);

    gray8_image_t in(dim, dim);
    gray8_image_t out(dim, dim);

    IppiSize srcRoi = { dim, dim };

    for (auto _ : state) {
        // The code to benchmark
        ippiMirror_8u_C1R(
            boost::gil::interleaved_view_get_raw_data(const_view(in)), (int) in.width(),
            boost::gil::interleaved_view_get_raw_data(view(out)), (int) out.width(),
            srcRoi,
            ippAxsVertical);
    }
}
BENCHMARK(ipp_flip_left_right)->RangeMultiplier(2)->Range(256, 8 << 10);

static void ipp_flip_left_right_inplace(benchmark::State& state)
{
    using namespace boost::gil;

    size_t dim = state.range(0);

    gray8_image_t in(dim, dim);

    IppiSize srcRoi = { dim, dim };

    for (auto _ : state) {
        // The code to benchmark
        ippiMirror_8u_C1IR(
            boost::gil::interleaved_view_get_raw_data(view(in)), (int) in.width(),
            srcRoi,
            ippAxsVertical);
    }
}
BENCHMARK(ipp_flip_left_right_inplace)->RangeMultiplier(2)->Range(256, 8 << 10);

static void ipp_flip_up_down(benchmark::State& state)
{
    using namespace boost::gil;

    size_t dim = state.range(0);

    gray8_image_t in(dim, dim);
    gray8_image_t out(dim, dim);

    IppiSize srcRoi = { dim, dim };

    for (auto _ : state) {
        // The code to benchmark
        ippiMirror_8u_C1R(
            boost::gil::interleaved_view_get_raw_data(const_view(in)), (int) in.width(),
            boost::gil::interleaved_view_get_raw_data(view(out)), (int) out.width(),
            srcRoi,
            ippAxsHorizontal);
    }
}
BENCHMARK(ipp_flip_up_down)->RangeMultiplier(2)->Range(256, 8 << 10);

static void ipp_flip_up_down_inplace(benchmark::State& state)
{
    using namespace boost::gil;

    size_t dim = state.range(0);

    gray8_image_t in(dim, dim);

    IppiSize srcRoi = { dim, dim };

    for (auto _ : state) {
        // The code to benchmark
        ippiMirror_8u_C1IR(
            boost::gil::interleaved_view_get_raw_data(view(in)), (int) in.width(),
            srcRoi,
            ippAxsHorizontal);
    }
}
BENCHMARK(ipp_flip_up_down_inplace)->RangeMultiplier(2)->Range(256, 8 << 10);

BENCHMARK_MAIN();
