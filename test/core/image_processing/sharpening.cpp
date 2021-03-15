//
// Copyright 2021 Harsit Pant <harshitpant83@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/gil/image_processing/sharpening.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/algorithm.hpp>

#include <boost/core/lightweight_test.hpp>

using namespace boost;
using namespace gil;
using namespace std;

int const width = 9;
int const height = 9;

gray8_image_t gray_img_in(width, height);
gray8_image_t gray_img_out(width, height);
gray8_image_t gray_img_expected(width, height);

rgb8_image_t rgb_img_in(width, height);
rgb8_image_t rgb_img_out(width, height);
rgb8_image_t rgb_img_expected(width, height);

void no_edges()
{
    //all pixels same, no edge detected, thus nothing to sharpen.
    fill_pixels(view(gray_img_in), gray8_pixel_t(50));
    fill_pixels(view(gray_img_expected), gray8_pixel_t(50));

    sharpen(const_view(gray_img_in), view(gray_img_out), 2, 1, 0.5);

    BOOST_TEST(equal_pixels(const_view(gray_img_out), const_view(gray_img_expected)));
}

void no_edges_rgb()
{
    //all pixels same, no edge detected, thus nothing to sharpen.
    fill_pixels(view(rgb_img_in), rgb8_pixel_t(50, 50, 50));
    fill_pixels(view(rgb_img_expected), rgb8_pixel_t(50, 50, 50));

    sharpen(const_view(rgb_img_in), view(rgb_img_out), 1, 3, 0.1);

    BOOST_TEST(equal_pixels(const_view(rgb_img_out), const_view(rgb_img_expected)));
}

void amount_zero()
{
    // image remains unchanged if specified ammount is zero.
    uint8_t in[] =
    {
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53
    };

    uint8_t out[] =
    {
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53
    };
     
    gray8c_view_t in_view = interleaved_view(9, 9, reinterpret_cast<const gray8_pixel_t*>(in), 9);
    
    gray8c_view_t exp_view = interleaved_view(9, 9, reinterpret_cast<const gray8_pixel_t*>(out), 9);

    sharpen(in_view, view(gray_img_out), 1, 0);
    BOOST_TEST(equal_pixels(const_view(gray_img_out), exp_view));
}
int main()
{
    no_edges();
    no_edges_rgb();
    amount_zero();
    return ::boost::report_errors();
}
