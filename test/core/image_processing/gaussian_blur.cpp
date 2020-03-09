//
// Copyright 2020 Laxmikant Suryavanshi <laxmikantsuryavanshi@hotmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#define BOOST_TEST_MODULE gil/test/core/image_processing/gaussian_filter
#include "unit_test.hpp"

#include <boost/gil/algorithm.hpp>
#include <boost/gil/gray.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_processing/filter.hpp>

namespace gil = boost::gil;

const float kernel[] =
{
  0.241971, 0.398942, 0.241971
};

std::uint8_t img[] =
{
  0, 0,   0,   0,   0,
  0, 100, 100, 100, 0,
  0, 100, 100, 100, 0,
  0, 100, 100, 100, 0,
  0, 0,   0,   0,   0
};

std::uint8 output[] =
{
  5, 15, 21, 15, 5,
  15, 41, 56, 41, 15,
  21, 56, 77, 56, 21,
  15, 41, 56, 41, 15,
  5, 15, 21, 15, 5
};

BOOST_AUTO_TEST_SUITE(filter)

BOOST_AUTO_TEST_CASE(gaussian_filter_with_default_parameters)
{
    gil::gray8c_view_t src_view =
        gil::interleaved_view(5, 5, reinterpret_cast<const gil::gray8_pixel_t*>(img), 5);

    gil::image<gil::gray8_pixel_t> temp_img(src_view.width(), src_view.height());
    typename gil::image<gil::gray8_pixel_t>::view_t temp_view = view(temp_img);
    gil::gray8_view_t dst_view(temp_view);

    gil::gaussian_filter(src_view, dst_view, 3, 1.0f);

    gil::gray8c_view_t out_view =
        gil::interleaved_view(5, 5, reinterpret_cast<const gil::gray8_pixel_t*>(output), 5);

    BOOST_TEST(gil::equal_pixels(out_view, dst_view));
}

BOOST_AUTO_TEST_SUITE_END()
