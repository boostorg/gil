//
// Copyright 2013 Christian Henning
// Copyright 2020 Mateusz Loskot <mateusz@loskot.net>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include <boost/gil.hpp>
#include <boost/gil/extension/toolbox/color_spaces/hsl.hpp>

#include <boost/core/lightweight_test.hpp>

#include <cstdint>

#include "test_utility_output_stream.hpp"

namespace gil = boost::gil;

void test_basic_hsl_to_rgb()
{
    gil::rgb8_pixel_t p(64, 0, 64);
    gil::hsl32f_pixel_t h;
    gil::color_convert(p, h);

    gil::rgb8_pixel_t b;
    gil::color_convert(h, b);
    BOOST_TEST_EQ(b, p);
}

void test_image_assign_hsl()
{
    std::ptrdiff_t const w = 320;
    std::ptrdiff_t const h = 240;
    gil::hsl32f_image_t hsl_img(w, h);

    for (std::ptrdiff_t y = 0; y < h; y++)
    {
        gil::hsl32f_view_t::x_iterator hsl_x_it = view(hsl_img).row_begin(y);
        float v = static_cast<float>(h - y) / h;
        for (std::ptrdiff_t x = 0; x < w; x++)
        {
            float const hue = (x + 1.f) / w;
            gil::hsl32f_pixel_t const p(hue, 1.0, v);
            hsl_x_it[x] = p;
            BOOST_TEST_EQ(gil::view(hsl_img)(x, y), p);
        }
    }
}

int main()
{
    test_basic_hsl_to_rgb();
    test_image_assign_hsl();

    return ::boost::report_errors();
}
