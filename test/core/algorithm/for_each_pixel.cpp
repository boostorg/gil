//
// Copyright 2018 Mateusz Loskot <mateusz at loskot dot net>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#define BOOST_TEST_MODULE gil/test/core/algorithm/for_each_pixel
#include "unit_test.hpp"

#include <boost/gil/algorithm.hpp>
#include <boost/gil/image.hpp>

namespace gil = boost::gil;

BOOST_AUTO_TEST_CASE(lambda_expression)
{
    gil::gray8_pixel_t const gray128(128);
    gil::gray8_image_t image(2, 2, gray128);

    int sum{0};
    gil::for_each_pixel(gil::view(image), [&sum](gil::gray8_pixel_t& p) {
        sum += gil::at_c<0>(p);
    });
    BOOST_TEST(sum == 2 * 2 * 128);
}
