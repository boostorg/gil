/*
    Copyright 2005-2007 Adobe Systems Incorporated
   
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).

    See http://opensource.adobe.com/gil for most recent version including documentation.
*/

/*************************************************************************************************/

/// \file
/// \brief Test file for using dynamic images
/// \author Lubomir Bourdev and Hailin Jin
/// \date February 27, 2007

#include <boost/gil.hpp>
#include <boost/gil/extension/dynamic_image/any_image.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/mpl/vector.hpp>

int main()
{
    namespace bg = boost::gil;

    using my_images_t = boost::mpl::vector<bg::gray8_image_t, bg::rgb8_image_t, bg::gray16_image_t, bg::rgb16_image_t>;
    bg::any_image<my_images_t> dynamic_image;
    bg::read_image("test.jpg", dynamic_image, bg::jpeg_tag());
    // Save the image upside down, preserving its native color space and channel depth
    auto view = bg::flipped_up_down_view(bg::const_view(dynamic_image));
    bg::write_view("out-dynamic_image.jpg", view, bg::jpeg_tag());
}
