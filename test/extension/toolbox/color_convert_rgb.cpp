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
#include <tuple>
#include <vector>

#include "test_utility_output_stream.hpp"
#include "test_utility_with_tolerance.hpp"

namespace gil = boost::gil;

void test_basic_rgb_to_hsl()
{
    gil::rgb8_pixel_t p{128, 0, 128};
    gil::hsl32f_pixel_t h;
    gil::color_convert(p, h);

    BOOST_TEST_GT(gil::get_color(h, gil::hsl_color_space::hue_t()), 0.8);         // 0.83333331
    BOOST_TEST_EQ(gil::get_color(h, gil::hsl_color_space::saturation_t()), 1.0);  // 1.00000000
    BOOST_TEST_GT(gil::get_color(h, gil::hsl_color_space::lightness_t()), 0.25);  // 0.25098040
}

void test_colors_rgb_to_hsl()
{
    using color_t = std::tuple<std::uint8_t, std::uint8_t, std::uint8_t, float, float, float>;
    std::vector<color_t> colors = {
        {0, 0, 0, 0, 0, 0},               // black
        {255, 255, 255, 0, 0, 1},         // white
        {255, 0, 0, 0, 1, 0.5},           // red
        {0, 255, 0, 2 / 6.f, 1, 0.5},     // lime
        {0, 0, 255, 4 / 6.f, 1, 0.5},     // blue
        {255, 255, 0, 1 / 6.f, 1, 0.5},   // yellow
        {0, 255, 255, 3 / 6.f, 1, 0.5},   // cyan
        {255, 0, 255, 5 / 6.f, 1, 0.5},   // magenta
        {191, 191, 191, 0, 0, 0.75},      // silver
        {128, 128, 128, 0, 0, 0.5},       // gray
        {128, 0, 0, 0, 1, 0.25},          // maroon
        {128, 128, 0, 1 / 6.f, 1, 0.25},  // olive
        {0, 128, 0, 2 / 6.f, 1, 0.25},    // green
        {128, 0, 128, 5 / 6.f, 1, 0.25},  // purple
        {0, 128, 128, 3 / 6.f, 1, 0.25},  // teal
        {0, 0, 128, 4 / 6.f, 1, 0.25},    // navy
    };

    for (auto&& color : colors)
    {
        std::uint8_t r{0}, g{0}, b{0};
        float h{0}, s{0}, l{0};
        std::tie(r, g, b, h, s, l) = color;
        gil::rgb8_pixel_t rgb(r, g, b);
        gil::hsl32f_pixel_t hsl;
        gil::color_convert(rgb, hsl);

        float const abs_error = 0.002;
        BOOST_TEST_WITH(
            h, get_color(hsl, gil::hsl_color_space::hue_t()),
            gil::test::utility::with_tolerance<double>(abs_error));
        BOOST_TEST_WITH(
            s, get_color(hsl, gil::hsl_color_space::saturation_t()),
            gil::test::utility::with_tolerance<double>(abs_error));
        BOOST_TEST_WITH(
            l, get_color(hsl, gil::hsl_color_space::lightness_t()),
            gil::test::utility::with_tolerance<double>(abs_error));
    }
}

void test_copy_pixels_rgb_to_hsl()
{
    gil::rgb8_image_t rgb_img(320, 240);
    gil::rgb8_pixel_t rgb_pix(64, 32, 64);
    gil::fill_pixels(view(rgb_img), rgb_pix);
    gil::hsl32f_image_t hsl_img(view(rgb_img).dimensions());
    gil::copy_pixels(gil::color_converted_view<gil::hsl32f_pixel_t>(view(rgb_img)), view(hsl_img));

    auto view = gil::view(hsl_img);
    for (auto it = view.begin(), end = view.end(); it != end; ++it)
    {
        gil::rgb8_pixel_t p;
        gil::color_convert(*it, p);
        BOOST_TEST_EQ(p, rgb_pix);
    }
}

int main()
{
    test_basic_rgb_to_hsl();
    test_colors_rgb_to_hsl();
    test_copy_pixels_rgb_to_hsl();

    return ::boost::report_errors();
}
