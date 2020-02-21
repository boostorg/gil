//
// Copyright 2019 Ayush Bansal <abansal1008@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/image_processing/morphology.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/gil.hpp>
#include <boost/multi_array.hpp>

const int struct_elem_size = 3;

using namespace boost::gil;
using namespace std;

int main()
{
    gray8_image_t img;
    read_image("test_adaptive.png", img, png_tag{});
    gray8_image_t img_dilate(img.dimensions());
    gray8_image_t img_erode(img.dimensions());

    int se_arr[struct_elem_size][struct_elem_size] = {{0,1,0},{1,1,1},{0,1,0}};

    using se_type = boost::multi_array<int, 2>;

    se_type se{boost::extents[struct_elem_size][struct_elem_size]};
    for (int i=0; i < struct_elem_size; ++i)
    {
        for (int j=0; j < struct_elem_size; ++j)
        {
            se[i][j] = se_arr[i][j];
        }
    }

    dilate(view(img), view(img_dilate), se);
    erode(view(img), view(img_erode), se);

    write_view("dilated_image.png", view(img_dilate), png_tag{});
    write_view("eroded_image.png", view(img_erode), png_tag{});
}
