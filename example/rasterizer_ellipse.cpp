//
// Copyright 2021 Prathamesh Tagore <prathameshtagore@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil.hpp>

namespace gil = boost::gil;

int main()
{
    // Syntax for usage :- 
    // auto rasterizer = gil::midpoint_elliptical_rasterizer{};
    // rasterizer(img_view, colour, center, semi-axes_length);
    // Where
    // img_view : gil view of the image on which ellipse is to be drawn.
    // colour : Vector containing channel intensity values for img_view. Number of colours 
    // provided must be equal to the number of channels present in img_view.
    // center : Array containing x co-ordinate and y co-ordinate of the center respectively.
    // semi-axes_length : Array containing lengths of horizontal semi-axis and vertical semi-axis
    // respectively.

    gil::gray8_image_t gray_buffer_image(256, 256);
    auto gray_elliptical_rasterizer = gil::midpoint_elliptical_rasterizer{};
    gray_elliptical_rasterizer(view(gray_buffer_image), {128}, {128, 128}, {100, 50});

    gil::rgb8_image_t rgb_buffer_image(256, 256);
    auto rgb_elliptical_rasterizer = gil::midpoint_elliptical_rasterizer{};
    rgb_elliptical_rasterizer(view(rgb_buffer_image), {0, 0, 255}, {128, 128}, {50, 100});

    gil::rgb8_image_t rgb_buffer_image_out_of_bound(256, 256);
    auto rgb_elliptical_rasterizer_out_of_bound = gil::midpoint_elliptical_rasterizer{};
    rgb_elliptical_rasterizer_out_of_bound(view(rgb_buffer_image_out_of_bound), {255, 0, 0},
        {100, 100}, {125, 125});

    gil::write_view("rasterized_ellipse_gray.jpg", view(gray_buffer_image), gil::jpeg_tag{});
    gil::write_view("rasterized_ellipse_rgb.jpg", view(rgb_buffer_image), gil::jpeg_tag{});
    gil::write_view("rasterized_ellipse_rgb_out_of_bound.jpg", view(rgb_buffer_image_out_of_bound),
        gil::jpeg_tag{});
}