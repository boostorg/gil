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
