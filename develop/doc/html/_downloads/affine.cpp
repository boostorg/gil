/*
    Copyright 2005-2007 Adobe Systems Incorporated

    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).

    See http://opensource.adobe.com/gil for most recent version including documentation.
*/

/*************************************************************************************************/

///////////////////////
////  NOTE: This sample file uses the numeric extension, which does not come with the Boost distribution.
////  You may download it from http://opensource.adobe.com/gil
///////////////////////

/// \file
/// \brief Test file for resample_pixels() in the numeric extension
/// \author Lubomir Bourdev and Hailin Jin
/// \date February 27, 2007

#include <boost/gil.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

int main()
{
    namespace gil = boost::gil;

    gil::rgb8_image_t img;
    gil::read_image("test.jpg", img, gil::jpeg_tag());

    // test resample_pixels
    // Transform the image by an arbitrary affine transformation using nearest-neighbor resampling
    gil::rgb8_image_t transf(gil::rgb8_image_t::point_t(gil::view(img).dimensions() * 2));
    gil::fill_pixels(gil::view(transf), gil::rgb8_pixel_t(255, 0, 0)); // the background is red

    gil::matrix3x2<double> mat =
        gil::matrix3x2<double>::get_translate(-gil::point2<double>(200,250)) *
        gil::matrix3x2<double>::get_rotate(-15*3.14/180.0);
    gil::resample_pixels(const_view(img), gil::view(transf), mat, gil::nearest_neighbor_sampler());
    gil::write_view("out-affine.jpg", gil::view(transf), gil::jpeg_tag());

    return 0;
}
