//
// Copyright 2022 Marco Langer
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include <boost/gil/extension/dynamic_image/any_image.hpp>
#include <boost/gil/extension/dynamic_image/algorithm.hpp>

#include <boost/core/lightweight_test.hpp>

#include <vector>

namespace gil = boost::gil;

struct accumulator
{
    template <typename Pixel> 
    void operator()(Pixel const& p)
    {
        sum += gil::at_c<0>(p);
    }

    int sum{};
};

void test_accumulate()
{
    using any_image_t = typename gil::any_image<gil::rgb16_image_t, gil::gray8_image_t>;
 
    gil::gray8_pixel_t const gray128(128);
    gil::rgb16_pixel_t const rgb16(128);

    std::vector<any_image_t> images;
    images.emplace_back(gil::gray8_image_t(2, 2, gray128));
    images.emplace_back(gil::rgb16_image_t(2, 2, rgb16));

    for (any_image_t const& image : images)
    {
        accumulator acc = gil::for_each_pixel(gil::const_view(image), accumulator());
        BOOST_TEST_EQ(acc.sum, 2 * 2 * 128);
    }
}

int main()
{
    test_accumulate();

    return ::boost::report_errors();
}
