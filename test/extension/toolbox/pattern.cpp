//
// Copyright 2021 Prathamesh Tagore <prathameshtagore@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

// Reference for test case was taken from 
// https://github.com/tuttleofx/TuttleOFX/blob/develop/libraries/boostHack/boost/gil/extension/toolbox/pattern.tests.cpp

#include <boost/gil/extension/toolbox/metafunctions.hpp>
#include <boost/core/lightweight_test.hpp>
#include <vector>

namespace gil = boost::gil;

// This function helps us fill pixels of a rgb view given as 2nd argument with
// elements of the vector given as 1st argument.
void pixel_fill_rgb(std::vector<std::vector<std::vector<unsigned int>>>& vec,
                gil::rgb8_image_t& img)
{
    for (std::ptrdiff_t view_row = 0; view_row < view(img).height(); ++view_row)
    {
        for (std::ptrdiff_t view_col = 0; view_col < view(img).width(); ++view_col)
        {
            gil::view(img)(view_col, view_row) = gil::rgb8_pixel_t(
                static_cast<unsigned char>(vec[view_row][view_col][0]),
                static_cast<unsigned char>(vec[view_row][view_col][1]),
                static_cast<unsigned char>(vec[view_row][view_col][2]));
        }
    }
}

int main()
{
    std::vector<std::vector<std::vector<unsigned int>>> original_img_vector {
    {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}},
    {{  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}},
    {{  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}},
    {{  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}},
    {{  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}},
    {{  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}},
    {{  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}},
    {{  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}},
    {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}},
    {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}}, 
    {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}},
    {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}},
    {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}},
    {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}},
    {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}},
    {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}}
};

    std::vector<std::vector<std::vector<unsigned int>>> expected_img_vector {
    {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}},
    {{  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}},
    {{  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}},
    {{  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0} ,{  0,   0,   0}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}},
    {{  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}},
    {{  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}},
    {{  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}},
    {{  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}},
    {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}},
    {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}},
    {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}},
    {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}},
    {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}},
    {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}},
    {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0},
    {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}},
    {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}},
    {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}}
};

    gil::rgb8_image_t original_img(16, 16), expected_img(33, 17), obtained_img(33, 17);
    gil::rgb8_view_t original_img_view = gil::view(original_img);
    gil::rgb8_view_t obtained_img_view = gil::view(obtained_img);

    pixel_fill_rgb(original_img_vector, original_img);
    pixel_fill_rgb(expected_img_vector, expected_img);

    gil::pattern<gil::rgb8_view_t> pattern(original_img_view);
    pattern(obtained_img_view);

    BOOST_TEST(gil::equal_pixels(obtained_img_view, gil::view(expected_img)));

    return boost::report_errors();
}
