//
// Copyright 2019 Miral Shah <miralshah2211@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/image_processing/threshold.hpp>

using namespace boost::gil;

int main()
{
    gray8_image_t img;
    read_image("test_adaptive.png", img, png_tag{});
    gray8_image_t img_out(img.dimensions());

    //    performing binary threshold on each channel of the image
    //    if the pixel value is more than 150 than it will be set to 255 else to 0
    boost::gil::threshold_adaptive(const_view(img), view(img_out), 11);
    write_view("out-threshold-adaptive.png", view(img_out), png_tag{});

    //    if the pixel value is more than 150 than it will be set to 150 else no change
    boost::gil::threshold_adaptive(const_view(img), view(img_out), 11, threshold_adaptive_method::mean, threshold_direction::inverse);
    write_view("out-threshold-adaptive-inv.png", view(img_out), png_tag{});

    return 0;
}
