//
// Copyright 2021 Prathamesh Tagore <prathameshtagore@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/image_processing/morphology.hpp>
#include <vector>

//Default structuring element is SE = [1,1,1]
//                                    |1,1,1|
//                                    [1,1,1]
//SE(1,1)(center pixel) is the one which coincides with the currently considered pixel of the 
//image to be convolved. The structuring element can be easily changed by the user.

int main()
{
    using namespace boost::gil;
    gray8_image_t img;
    read_image("original.png", img, png_tag{});

    //Image can be converted to a binary format with high value as 255 and low value as 0
    //by uncommenting the following threshold operator.This can be used for binary morphological
    //operations.Convenient threshold for binary conversion should be chosen by the user.
    //threshold_binary(view(img), view(img),170, 255);

    std::vector<float>ker_vec(9,1.0f);//Structuring element
    detail::kernel_2d<float> ker_mat(ker_vec.begin(), ker_vec.size(), 1, 1);
    gray8_image_t img_out_dilation(img.dimensions()),img_out_erosion(img.dimensions()),img_out_opening(img.dimensions());
    gray8_image_t img_out_closing(img.dimensions()),img_out_mg(img.dimensions()),img_out_top_hat(img.dimensions());
    gray8_image_t img_out_black_hat(img.dimensions());
    //dilate(input_image_view,structuring_element,iterations)
    dilate(view(img),view(img_out_dilation),ker_mat,1);
    //erode(input_image_view,structuring_element,iterations)
    erode(view(img),view(img_out_erosion),ker_mat,1);
    //opening(input_image_view,structuring_element)
    opening(view(img),view(img_out_opening),ker_mat);
    //closing(input_image_view,structuring_element)
    closing(view(img),view(img_out_closing),ker_mat);
    //morphological_gradient(input_image_view,structuring_element)
    morphological_gradient(view(img),view(img_out_mg),ker_mat);
    //top_hat(input_image_view,structuring_element)
    top_hat(view(img),view(img_out_top_hat),ker_mat);
    //black_hat(input_image_view,structuring_element)
    black_hat(view(img),view(img_out_black_hat),ker_mat);

    //Saving results of above morphological transformations.
    write_view("out-morphological-example-dilation.png", view(img_out_dilation), png_tag{});
    write_view("out-morphological-example-erosion.png", view(img_out_erosion), png_tag{});
    write_view("out-morphological-example-opening.png", view(img_out_opening), png_tag{});
    write_view("out-morphological-example-closing.png", view(img_out_closing), png_tag{});
    write_view("out-morphological-example-morphological_gradient.png", view(img_out_mg), png_tag{});
    write_view("out-morphological-example-top_hat.png", view(img_out_top_hat), png_tag{});
    write_view("out-morphological-example-black_hat.png", view(img_out_black_hat), png_tag{});
}
