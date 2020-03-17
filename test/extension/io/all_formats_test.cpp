//
// Copyright 2013 Christian Henning
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/extension/io/bmp.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/io/pnm.hpp>
#include <boost/gil/extension/io/targa.hpp>
#include <boost/gil/extension/io/tiff.hpp>

#include <boost/core/lightweight_test.hpp>

#include "paths.hpp"

namespace gil = boost::gil;
namespace fs = boost::filesystem;

// Test will include all format's headers and load and write some images.
// This test is more of a compilation test.
#ifdef BOOST_GIL_IO_TEST_ALLOW_READING_IMAGES

void test_bmp()
{
    gil::rgb8_image_t img;
    gil::read_image(bmp_filename, img, gil::bmp_tag());
#ifdef BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
    fs::create_directories(fs::path(bmp_out));
    gil::write_view(bmp_out + "all_formats_test.bmp", gil::view(img), gil::bmp_tag());
#endif // BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
}

void test_jpeg()
{
    gil::rgb8_image_t img;
    gil::read_image(jpeg_filename, img, gil::jpeg_tag());
#ifdef BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
    fs::create_directories(fs::path(jpeg_out));
    gil::write_view(jpeg_out + "all_formats_test.jpg", gil::view(img), gil::jpeg_tag());
#endif // BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
}

void test_png()
{
    gil::rgba8_image_t img;
    gil::read_image(png_filename, img, gil::png_tag());
#ifdef BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
    fs::create_directories(fs::path(png_out));
    gil::write_view(png_out + "all_formats_test.png", gil::view(img), gil::png_tag());
#endif // BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
}

void test_pnm()
{
    gil::rgb8_image_t img;
    gil::read_image(pnm_filename, img, gil::pnm_tag());
#ifdef BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
    fs::create_directories(fs::path(pnm_out));
    gil::write_view(pnm_out + "all_formats_test.pnm", gil::view(img), gil::pnm_tag());
#endif // BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
}

void test_targa()
{
    gil::rgb8_image_t img;
    gil::read_image(targa_filename, img, gil::targa_tag());
#ifdef BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
    fs::create_directories(fs::path(targa_out));
    gil::write_view(targa_out + "all_formats_test.tga", gil::view(img), gil::targa_tag());
#endif // BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
}

void test_tiff()
{
    gil::rgba8_image_t img;
    gil::read_image(tiff_filename, img, gil::tiff_tag());
#ifdef BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
    fs::create_directories(fs::path(tiff_out));
    gil::write_view(tiff_out + "all_formats_test.tif", gil::view(img), gil::tiff_tag());
#endif // BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
}

int main()
{
    test_bmp();
    test_jpeg();
    test_png();
    test_pnm();
    test_targa();
    test_tiff();

    return boost::report_errors();
}

#else
int main() {}
#endif // BOOST_GIL_IO_TEST_ALLOW_READING_IMAGES
