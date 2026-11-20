// Copyright (C) 2020 Samuel Debionne, ESRF.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <benchmark/benchmark.h>

#include <ipp.h>

#include <boost/gil.hpp>

inline bool is_odd(benchmark::IterationCount cnt) { return (cnt % 2); }

static void ipp_flip_left_right(benchmark::State& state)
{
    using namespace boost::gil;

    size_t dim = state.range(0);

    gray8_image_t in(dim, dim);
    generate_pixels(view(in), [i = 0]() mutable -> std::uint8_t { return ++i; });
    gray8_image_t out(dim, dim);

    IppiSize srcRoi = { dim, dim };

    for (auto _ : state) {
        // The code to benchmark
        auto res = ippiMirror_8u_C1R(
            boost::gil::interleaved_view_get_raw_data(const_view(in)), (int)const_view(in).pixels().row_size(),
            boost::gil::interleaved_view_get_raw_data(view(out)), (int)view(in).pixels().row_size(),
            srcRoi,
            ippAxsVertical);

        if (res != ippStsNoErr)
            state.SkipWithError("ipp_flip_left_right failed");
    }
    
    if (!equal_pixels(flipped_left_right_view(const_view(in)), const_view(out)))
        state.SkipWithError("ipp_flip_left_right wrong result");
}
BENCHMARK(ipp_flip_left_right)->RangeMultiplier(2)->Range(256, 8 << 10);

static void ipp_flip_left_right_inplace(benchmark::State& state)
{
    using namespace boost::gil;

    size_t dim = state.range(0);

    gray8_image_t in(dim, dim);
    generate_pixels(view(in), [i = 0]() mutable -> std::uint8_t { return ++i; });
    gray8_image_t in_ref(in);

    IppiSize srcRoi = { dim, dim };

    for (auto _ : state) {
        // The code to benchmark
        auto res = ippiMirror_8u_C1IR(
            boost::gil::interleaved_view_get_raw_data(view(in)), (int)view(in).pixels().row_size(),
            srcRoi,
            ippAxsVertical);

        if (res != ippStsNoErr)
            state.SkipWithError("ipp_flip_left_right_inplace failed");
    }

    if (is_odd(state.iterations())) { 
        if (!equal_pixels(flipped_left_right_view(const_view(in_ref)), const_view(in)))
            state.SkipWithError("ipp_flip_left_right_inplace wrong result");
    }
    else {
        if (!equal_pixels(const_view(in_ref), const_view(in)))
            state.SkipWithError("ipp_flip_left_right_inplace wrong result");
    }
}
BENCHMARK(ipp_flip_left_right_inplace)->RangeMultiplier(2)->Range(256, 8 << 10);

static void ipp_flip_up_down(benchmark::State& state)
{
    using namespace boost::gil;

    size_t dim = state.range(0);

    gray8_image_t in(dim, dim);
    generate_pixels(view(in), [i = 0]() mutable -> std::uint8_t { return ++i; });
    gray8_image_t out(dim, dim);

    IppiSize srcRoi = { dim, dim };

    for (auto _ : state) {
        // The code to benchmark
        auto res = ippiMirror_8u_C1R(
            boost::gil::interleaved_view_get_raw_data(const_view(in)), (int)const_view(in).pixels().row_size(),
            boost::gil::interleaved_view_get_raw_data(view(out)), (int)view(out).pixels().row_size(),
            srcRoi,
            ippAxsHorizontal);

        if (res != ippStsNoErr)
            state.SkipWithError("ipp_flip_up_down failed");
    }

    if (is_odd(state.iterations())) {
        if (!equal_pixels(flipped_up_down_view(const_view(in)), const_view(out)))
            state.SkipWithError("ipp_flip_up_down wrong result");
    }
    else {
        if (!equal_pixels(const_view(in), const_view(out)))
            state.SkipWithError("ipp_flip_up_down wrong result");
    }
}
BENCHMARK(ipp_flip_up_down)->RangeMultiplier(2)->Range(256, 8 << 10);

static void ipp_flip_up_down_inplace(benchmark::State& state)
{
    using namespace boost::gil;

    size_t dim = state.range(0);

    gray8_image_t in(dim, dim);
    generate_pixels(view(in), [i = 0]() mutable -> std::uint8_t { return ++i; });
    gray8_image_t in_ref(in);

    IppiSize srcRoi = { dim, dim };

    for (auto _ : state) {
        // The code to benchmark
        auto res = ippiMirror_8u_C1IR(
            boost::gil::interleaved_view_get_raw_data(view(in)), (int)view(in).pixels().row_size(),
            srcRoi,
            ippAxsHorizontal);

        if (res != ippStsNoErr)
            state.SkipWithError("ipp_flip_up_down_inplace failed");
    }
    
    if (!equal_pixels(flipped_up_down_view(const_view(in_ref)), const_view(in)))
        state.SkipWithError("ipp_flip_up_down_inplace wrong result");
}
BENCHMARK(ipp_flip_up_down_inplace)->RangeMultiplier(2)->Range(256, 8 << 10);

BENCHMARK_MAIN();
