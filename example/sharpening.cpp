//
// Copyright 2021 Harsit Pant <harshitpant83@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <iostream>
#include <boost/gil.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/image_processing/sharpening.hpp>

using namespace boost::gil;
using namespace std;
int main()
{
    gray8_image_t img_in;
    read_image("test_adaptive.png", img_in, png_tag{});
    gray8_image_t img_out(img_in.dimensions());

    sharpen(view(img_in), view(img_out), 1, 3);
    write_view("sharpened image.png", view(img_out), png_tag{});

    cout << "done";
    cin.get();
    return 0;
}
