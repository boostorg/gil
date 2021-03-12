//
// Copyright 2021 Prathamesh Tagore <prathameshtagore@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

// Reference for test case was taken from
// https://github.com/tuttleofx/TuttleOFX/blob/develop/libraries/boostHack/boost/gil/extension/toolbox/shrink.tests.cpp

#include <boost/gil/extension/toolbox/metafunctions.hpp>
#include <boost/core/lightweight_test.hpp>

namespace gil = boost::gil;

int main()
{
    gil::rgb8_image_t original_img(20, 20), expected_img(12, 8);
    gil::rgb8_view_t original_img_view = gil::view(original_img), obtained_img_view;

    gil::fill_pixels(original_img_view, gil::rgb8_pixel_t(255, 255, 255));
    gil::fill_pixels(gil::view(expected_img), gil::rgb8_pixel_t(255, 255, 255));

    gil::shrink<gil::rgb8_view_t> shrink(0.20, 0.30, 0.20, 0.30);
    shrink(original_img_view, obtained_img_view);

    BOOST_TEST(gil::equal_pixels(obtained_img_view, gil::view(expected_img)));

    return boost::report_errors();
}