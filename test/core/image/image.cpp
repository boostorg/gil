//
// Copyright 2019 Mateusz Loskot <mateusz at loskot dot net>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#define BOOST_TEST_MODULE gil/test/core/image/image
#include "unit_test.hpp"

#include <boost/gil.hpp>

#include "test_fixture.hpp"
#include "core/pixel/test_fixture.hpp"

namespace gil = boost::gil;
namespace fixture = boost::gil::test::fixture;

BOOST_AUTO_TEST_CASE_TEMPLATE(default_constructor, Image, fixture::image_types)
{
    {
        Image image;
        BOOST_TEST(image.width() == 0);
        BOOST_TEST(image.height() == 0);
    }
    {
        std::size_t alignment = 0;
        Image image(alignment);
        BOOST_TEST(image.width() == 0);
        BOOST_TEST(image.height() == 0);
    }
    {
        std::size_t alignment = 0;
        std::allocator<unsigned char> allocator;
        Image image(alignment, allocator);
        BOOST_TEST(image.width() == 0);
        BOOST_TEST(image.height() == 0);
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(constructor_with_dimensions, Image, fixture::image_types)
{
    gil::point_t const dimensions{256, 128};
    {
        Image image(dimensions);
        BOOST_TEST(image.width() == dimensions.x);
        BOOST_TEST(image.height() == dimensions.y);
    }
    {
        std::size_t alignment = 0;
        Image image(dimensions, alignment);
        BOOST_TEST(image.width() == dimensions.x);
        BOOST_TEST(image.height() == dimensions.y);
    }
    {
        std::size_t alignment = 0;
        std::allocator<unsigned char> allocator;
        Image image(dimensions, alignment, allocator);
        BOOST_TEST(image.width() == dimensions.x);
        BOOST_TEST(image.height() == dimensions.y);
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(constructor_with_width_height, Image, fixture::image_types)
{
    std::ptrdiff_t const width{32};
    std::ptrdiff_t const height{64};
    {
        Image image(width, height);
        BOOST_TEST(image.width() == width);
        BOOST_TEST(image.height() == height);
    }
    {
        std::size_t alignment = 0;
        Image image(width, height, alignment);
        BOOST_TEST(image.width() == width);
        BOOST_TEST(image.height() == height);
    }
    {
        std::size_t alignment = 0;
        std::allocator<unsigned char> allocator;
        Image image(width, height, alignment, allocator);
        BOOST_TEST(image.width() == width);
        BOOST_TEST(image.height() == height);
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(constructor_with_dimensions_pixel, Image, fixture::image_types)
{
    gil::point_t const dimensions{256, 128};
    {
        using pixel_t = typename Image::view_t::value_type;
        pixel_t const rnd_pixel = fixture::pixel_generator<pixel_t>::random();

        Image image(dimensions, rnd_pixel);
        BOOST_TEST(image.width() == dimensions.x);
        BOOST_TEST(image.height() == dimensions.y);

        for (pixel_t const& p : gil::view(image))
        {
            BOOST_TEST(p == rnd_pixel);
        }
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(constructor_with_dimensions_pixel_alignment, Image, fixture::image_types)
{
    gil::point_t const dimensions{3, 3};
    {
        using pixel_t = typename Image::view_t::value_type;
        pixel_t const rnd_pixel = fixture::pixel_generator<pixel_t>::random();

        Image image(dimensions, rnd_pixel, 32u);
        BOOST_TEST(image.width() == dimensions.x);
        BOOST_TEST(image.height() == dimensions.y);

        for (pixel_t const& p : gil::view(image))
        {
            BOOST_TEST(p == rnd_pixel);
        }
    }
}

// TODO:
//BOOST_AUTO_TEST_CASE(constructor_with_dimensions_bit_aligned_pixel) {}
//BOOST_AUTO_TEST_CASE(constructor_with_width_height_bit_aligned_pixel) {}
//BOOST_AUTO_TEST_CASE(constructor_with_dimensions_packed_pixel) {}
//BOOST_AUTO_TEST_CASE(constructor_with_width_height_packed_pixel) {}
