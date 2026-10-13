// Copyright (C) 2020 Samuel Debionne, ESRF.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <benchmark/benchmark.h>

#include <boost/gil.hpp>

#define GIL_ENABLE_UNROLLED 1

namespace boost {
    namespace gil {

        template <typename SrcView, typename DstView>
        static void transpose(const SrcView& src, const DstView& dst)
        {
            int i, j;
            auto m = src.width();
            auto n = src.height();

            static const int block_size = 4;

            std::array<gray8c_view_t::xy_locator::cached_location_t, block_size* block_size> l;
            std::generate(l.begin(), l.end(), [loc = src.xy_at(0, 0), i = 0]() mutable {
                auto res = loc.cache_location(i % block_size, i / block_size);
                i++;
                return res;
            });

#if GIL_ENABLE_UNROLLED
            for (i = 0; i <= m - block_size; i += block_size)
            {
                auto d0 = dst.row_begin(i);
                auto d1 = dst.row_begin(i + 1);
                auto d2 = dst.row_begin(i + 2);
                auto d3 = dst.row_begin(i + 3);

                for (j = 0; j <= n - block_size; j += block_size)
                {
                    auto s = src.xy_at(i, j);

                    d0[j] = s[l[0]]; d0[j + 1] = s[l[4]]; d0[j + 2] = s[l[8]]; d0[j + 3] = s[l[12]];
                    d1[j] = s[l[1]]; d1[j + 1] = s[l[5]]; d1[j + 2] = s[l[9]]; d1[j + 3] = s[l[13]];
                    d2[j] = s[l[2]]; d2[j + 1] = s[l[6]]; d2[j + 2] = s[l[10]]; d2[j + 3] = s[l[14]];
                    d3[j] = s[l[3]]; d3[j + 1] = s[l[7]]; d3[j + 2] = s[l[11]]; d3[j + 3] = s[l[15]];
                }

                for (; j < n; j++)
                {
                    auto s = src.xy_at(i, j);

                    d0[j] = s[l[0]]; d1[j] = s[l[1]]; d2[j] = s[l[2]]; d3[j] = s[l[3]];
                }
            }
#endif
            for (; i < m; i++)
            {
                auto d0 = dst.row_begin(i);
#if GIL_ENABLE_UNROLLED
                for (j = 0; j <= n - block_size; j += block_size)
                {
                    auto s = src.xy_at(i, j);

                    d0[j] = s[l[0]]; d0[j + 1] = s[l[4]]; d0[j + 2] = s[l[8]]; d0[j + 3] = s[l[12]];
                }
#endif
                for (; j < n; j++)
                    d0[j] = src(i, j);
            }
        }

    }
} //namespace boost::gil

static void transpose(benchmark::State& state)
{
    using namespace boost::gil;

    size_t dim = state.range(0);

    gray8_image_t in(dim, dim);
    gray8_image_t out(dim, dim);

    for (auto _ : state) {
        // The code to benchmark
        transpose(const_view(in), view(out));
    }
}
BENCHMARK(transpose)->RangeMultiplier(2)->Range(256, 8 << 10);

BENCHMARK_MAIN();