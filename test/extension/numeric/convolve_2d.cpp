//
// Copyright 2019 Miral Shah <miralshah2211@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#define BOOST_TEST_MODULE gil/test/extension/numeric/convolve_2d
#include "unit_test.hpp"

#include <cstddef>

#include <boost/gil.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

namespace gil = boost::gil;

std::uint8_t img[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 255, 0, 0, 0, 255, 0, 0,
    0, 0, 0, 255, 0, 255, 0, 0, 0,
    0, 0, 0, 0, 255, 0, 0, 0, 0,
    0, 0, 0, 255, 0, 255, 0, 0, 0,
    0, 0, 255, 0, 0, 0, 255, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

std::uint8_t output[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 28, 28, 28, 0, 28, 28, 28, 0,
    0, 28, 56, 56, 56, 56, 56, 28, 0,
    0, 28, 56, 85, 85, 85, 56, 28, 0,
    0, 0, 56, 85, 141, 85, 56, 0, 0,
    0, 28, 56, 85, 85, 85, 56, 28, 0,
    0, 28, 56, 56, 56, 56, 56, 28, 0,
    0, 28, 28, 28, 0, 28, 28, 28, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

BOOST_AUTO_TEST_SUITE(convolve_2d)

BOOST_AUTO_TEST_CASE(convolve_2d_with_normalized_mean_filter)
{
    gil::gray8c_view_t src_view =
        gil::interleaved_view(9, 9, reinterpret_cast<const gil::gray8_pixel_t*>(img), 9);

    gil::image<gil::gray8_pixel_t> temp_img(src_view.width(), src_view.height());
    typename gil::image<gil::gray8_pixel_t>::view_t temp_view = view(temp_img);
    gil::gray8_view_t dst_view(temp_view);

    std::vector<float> v(9, 1.0f / 9.0f);
    gil::detail::kernel_2d<float> kernel(v.begin(), v.size(), 1, 1);

    gil::detail::convolve_2d(src_view, kernel, dst_view);

    gil::gray8c_view_t out_view =
        gil::interleaved_view(9, 9, reinterpret_cast<const gil::gray8_pixel_t*>(output), 9);

    BOOST_TEST(gil::equal_pixels(out_view, dst_view));
}

BOOST_AUTO_TEST_SUITE_END()
