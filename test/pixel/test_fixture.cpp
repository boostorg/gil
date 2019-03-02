//
// Copyright 2019 Mateusz Loskot <mateusz at loskot dot net>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include <boost/gil/channel.hpp>

#include <limits>
#include <ostream>

#define BOOST_TEST_MODULE test_pixel_test_fixture
#include "unit_test.hpp"
#include "test_fixture.hpp"

namespace gil = boost::gil;
namespace fixture = boost::gil::test::fixture;

BOOST_AUTO_TEST_CASE_TEMPLATE(pixel_value_default_constructor, Pixel, fixture::pixel_types)
{
    fixture::pixel_value<Pixel> fix;
    Pixel const default_value{};
    // FIXME: Default value of pixel/homogeneous_color_base is undermined
    //BOOST_TEST(fix.pixel_ == default_value);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(pixel_value_parameterized_constructor, Pixel, fixture::pixel_types)
{
    using channel_t = typename gil::channel_type<Pixel>::type;
    Pixel sample_pixel;
    gil::static_fill(sample_pixel, std::numeric_limits<channel_t>::max());
    fixture::pixel_value<Pixel> fix{sample_pixel};
    // FIXME: Default value of pixel/homogeneous_color_base is undermined
    // Despite initialising first channel, rest of channels remain undetermined.
    BOOST_TEST(fix.pixel_ == sample_pixel);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(pixel_reference_default_constructor, Pixel, fixture::pixel_types)
{
    fixture::pixel_reference<Pixel&> fix;
    Pixel const default_value{};
    // FIXME: Default value of pixel/homogeneous_color_base is undermined
    //BOOST_TEST(fix.pixel_ == Pixel{});
}

BOOST_AUTO_TEST_CASE_TEMPLATE(pixel_reference_parameterized_constructor, Pixel, fixture::pixel_types)
{
    using channel_t = typename gil::channel_type<Pixel>::type;
    Pixel sample_pixel;
    gil::static_fill(sample_pixel, std::numeric_limits<channel_t>::max());
    fixture::pixel_reference<Pixel&> fix{sample_pixel};
    //BOOST_TEST(fix.pixel_ == sample_pixel);
}
