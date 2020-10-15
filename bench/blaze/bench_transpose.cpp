// Copyright (C) 2020 Samuel Debionne, ESRF.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <benchmark/benchmark.h>

#include <blaze/Math.h>

#include <boost/gil.hpp>

template <blaze::AlignmentFlag IsAligned = blaze::unaligned,
          blaze::PaddingFlag IsPadded = blaze::unpadded, bool StorageOrder = blaze::rowMajor,
          typename GrayView>
auto as_matrix(GrayView const& source)
{
    using channel_t = typename boost::gil::channel_type<GrayView>::type;

    return blaze::CustomMatrix<channel_t, IsAligned, IsPadded, StorageOrder>(
        boost::gil::interleaved_view_get_raw_data(source),
        source.height(),
        source.width());
}

static void blaze_transpose(benchmark::State& state)
{
    using namespace boost::gil;

    size_t dim = state.range(0);

    gray8_image_t in(dim, dim);
    gray8_image_t out(dim, dim);

    auto mat_in = as_matrix(view(in));
    auto mat_out = as_matrix(view(in));

    for (auto _ : state) {
        // The code to benchmark
        mat_out = blaze::trans(mat_in);
    }
}
BENCHMARK(blaze_transpose)->RangeMultiplier(2)->Range(256, 8 << 10);

static void blaze_transpose_inplace(benchmark::State& state)
{
    using namespace boost::gil;

    size_t dim = state.range(0);

    gray8_image_t in(dim, dim);

    auto mat_in_out = as_matrix(view(in));

    for (auto _ : state) {
        // The code to benchmark
        blaze::transpose(mat_in_out);
    }
}
BENCHMARK(blaze_transpose_inplace)->RangeMultiplier(2)->Range(256, 8 << 10);

BENCHMARK_MAIN();
