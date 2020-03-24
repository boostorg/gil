//
// Copyright 2013 Christian Henning
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include <boost/gil.hpp>
#include <boost/gil/extension/io/bmp/old.hpp>

#include <boost/mp11.hpp>
#include <boost/core/lightweight_test.hpp>

#include "mandel_view.hpp"
#include "paths.hpp"

namespace gil = boost::gil;
namespace mp11 = boost::mp11;

#ifdef BOOST_GIL_IO_TEST_ALLOW_READING_IMAGES

void test_old_read_dimensions()
{
    boost::gil::point_t dim = gil::bmp_read_dimensions(bmp_filename);
    BOOST_TEST_EQ(dim.x, 1000);
    BOOST_TEST_EQ(dim.y, 600);
}

void test_old_read_image()
{
    gil::rgb8_image_t img;
    gil::bmp_read_image(bmp_filename, img);

    BOOST_TEST_EQ(img.width(), 1000);
    BOOST_TEST_EQ(img.height(), 600);
}

void test_old_read_and_convert_image()
{
    gil::rgb8_image_t img;
    gil::bmp_read_and_convert_image(bmp_filename, img);

    BOOST_TEST_EQ(img.width(), 1000);
    BOOST_TEST_EQ(img.height(), 600);
}

void test_old_read_view()
{
    gil::rgb8_image_t img(1000, 600);
    gil::bmp_read_view(bmp_filename, view(img));
}

void test_old_read_and_convert_view()
{
    gil::rgb8_image_t img(1000, 600);
    gil::bmp_read_and_convert_view(bmp_filename, view(img));
}

void test_old_write_view()
{
#ifdef BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
    auto b = gil::rgb8_pixel_t(0, 0, 255);
    auto g = gil::rgb8_pixel_t(0, 255, 0);
    gil::bmp_write_view(bmp_out + "old_write_view_test.bmp", create_mandel_view(1000, 600, b, g));
#endif // BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
}

void test_old_dynamic_image()
{
    using my_img_types = mp11::mp_list
        <
            gil::gray8_image_t,
            gil::gray16_image_t,
            gil::rgb8_image_t,
            gil::rgba8_image_t
        >;

    gil::any_image<my_img_types> image;
    gil::bmp_read_image(bmp_filename.c_str(), image);

#ifdef BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
    gil::bmp_write_view(bmp_out + "old_dynamic_image_test.bmp", gil::view(image));
#endif // BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
}

int main()
{
    test_old_read_dimensions();
    test_old_read_image();
    test_old_read_and_convert_image();
    test_old_read_view();
    test_old_read_and_convert_view();
    test_old_write_view();
    test_old_dynamic_image();

    return boost::report_errors();
}

#else
int main() {}
#endif // BOOST_GIL_IO_TEST_ALLOW_READING_IMAGES
