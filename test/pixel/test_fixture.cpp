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

#define BOOST_TEST_MODULE test_channel_test_fixture
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
    using channel_t = gil::channel_type<Pixel>::type;
    auto const channel1_max = std::numeric_limits<channel_t>::max();
    Pixel const pixel{channel1_max};
    fixture::pixel_value<Pixel> fix{pixel};
    BOOST_TEST(fix.pixel_ == pixel);
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
    using channel_t = gil::channel_type<Pixel>::type;
    auto const channel1_max = std::numeric_limits<channel_t>::max();
    Pixel const pixel{channel1_max};
    fixture::pixel_reference<Pixel&> fix{pixel};
    BOOST_TEST(fix.pixel_ == pixel);
}
