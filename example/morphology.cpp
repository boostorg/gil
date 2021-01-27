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
#include<iostream>
#include<map>
#include<string>
// Default structuring element is SE = [1,1,1]
//                                     |1,1,1|
//                                     [1,1,1]
// SE(1,1)(center pixel) is the one which coincides with the currently considered pixel of the 
// image to be convolved. The structuring element can be easily changed by the user.

int main(int argc,char **argv)
{
    std::map<std::string,bool>operations;
    if(argc < 4 || argc > 11)
    {
        throw std::invalid_argument("Wrong format of command line arguments.\n"
        "Correct format is <Input_image.png> <Output_image_template>"
        " <operation1> <operation2> <operation3> <operation4> <operation5> <operation6>"
        " <operation7> <operation8>\n");
        // User has to enter atleast one operation and they can enter maximum 8 operations
        // considering binary conversion to be an operation.Output_image_template argument is the 
        // common component which will be added in all output images followed by a hyphen and the 
        // operation name.
        // Example : 
        // ./example_morphology original.png out black_hat top_hat morphological_gradient dilation
        //  erosion opening closing
        // Order of arguments entered will not matter with the exception of binary operation used
        // for binary morphological operations.If binary is entered through the command line,
        // it will always be the first operation to be applied.
        return -1;
    }
    else
    {
        for(int i=3;i<argc;++i)
            operations[argv[i]] = true;
    }
    using namespace boost::gil;
    gray8_image_t img;
    read_image(argv[1], img, png_tag{});

    // Image can be converted to a binary format with high value as 255 and low value as 0
    // by using the threshold operator . This can be used for binary morphological operations .
    // Convenient threshold for binary conversion may be chosen by the user.
    if(operations["binary"])
    {
        threshold_binary(view(img), view(img),170, 255);
        std::string name = argv[2];
        name += "-binary.png";
        write_view(name, view(img), png_tag{});
    }

    std::vector<float>ker_vec(9,1.0f);// Structuring element
    detail::kernel_2d<float> ker_mat(ker_vec.begin(), ker_vec.size(), 1, 1);
    gray8_image_t img_out_dilation(img.dimensions()),img_out_erosion(img.dimensions()),img_out_opening(img.dimensions());
    gray8_image_t img_out_closing(img.dimensions()),img_out_mg(img.dimensions()),img_out_top_hat(img.dimensions());
    gray8_image_t img_out_black_hat(img.dimensions());
    
    // Do not pass empty input image views in functions defined below for morphological operations 
    // to avoid errors.
    if(operations["dilation"])
    {
        // dilate(input_image_view,output_image_view,structuring_element,iterations)
        dilate(view(img),view(img_out_dilation),ker_mat,1);
        std::string name = argv[2];
        name += "-dilation.png";
        write_view( name, view(img_out_dilation), png_tag{});
    }
    
    if(operations["erosion"])
    {
        // erode(input_image_view,output_image_view,structuring_element,iterations)
        erode(view(img),view(img_out_erosion),ker_mat,1);
        std::string name = argv[2];
        name += "-erosion.png";
        write_view( name, view(img_out_erosion), png_tag{});
    }
    
    if(operations["opening"])
    {
        // opening(input_image_view,output_image_view,structuring_element)
        opening(view(img),view(img_out_opening),ker_mat);
        std::string name = argv[2];
        name += "-opening.png";
        write_view( name, view(img_out_opening), png_tag{});
    }
    
    if(operations["closing"])
    {
        // closing(input_image_view,output_image_view,structuring_element)
        closing(view(img),view(img_out_closing),ker_mat);
        std::string name = argv[2];
        name += "-closing.png";
        write_view( name, view(img_out_closing), png_tag{});
    }
    
    if(operations["morphological_gradient"])
    {
        // morphological_gradient(input_image_view,output_image_view,structuring_element)
        morphological_gradient(view(img),view(img_out_mg),ker_mat);
        std::string name = argv[2];
        name += "-morphological_gradient.png";
        write_view(name, view(img_out_mg), png_tag{});
    }
    
    if(operations["top_hat"])
    {
        // top_hat(input_image_view,output_image_view,structuring_element)
        top_hat(view(img),view(img_out_top_hat),ker_mat);
        std::string name = argv[2];
        name += "-top_hat.png";
        write_view(name, view(img_out_top_hat), png_tag{});
    }
    
    if(operations["black_hat"])
    {
        // black_hat(input_image_view,output_image_view,structuring_element)
        black_hat(view(img),view(img_out_black_hat),ker_mat);
        std::string name = argv[2];
        name += "-black_hat.png";
        write_view( name, view(img_out_black_hat), png_tag{});
    }
}
