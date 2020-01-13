//
// Copyright 2018 Mateusz Loskot <mateusz at loskot dot net>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#define BOOST_TEST_MODULE gil/test/core/image_view/planar_rgba_view
#include "unit_test.hpp"

#include <boost/gil.hpp>

#include <cstdint>

namespace boost { namespace gil {
namespace test { namespace fixture {

gil::point_t d{2, 2};
std::uint8_t r[] = { 1, 2, 3, 4 };
std::uint8_t g[] = { 10, 20, 30, 40 };
std::uint8_t b[] = { 110, 120, 130, 140 };
std::uint8_t a[] = { 251, 252, 253, 254 };

}}}}

namespace gil = boost::gil;
namespace fixture = boost::gil::test::fixture;

using dont_print_log_value_planar_pixel_reference = boost::gil::planar_pixel_reference<std::uint8_t&, boost::gil::rgba_t>;
BOOST_TEST_DONT_PRINT_LOG_VALUE(dont_print_log_value_planar_pixel_reference)

BOOST_AUTO_TEST_CASE(dimensions)
{
    auto v = gil::planar_rgba_view(fixture::d.x, fixture::d.y, fixture::r, fixture::g, fixture::b, fixture::a, sizeof(std::uint8_t) * 2);
    BOOST_TEST(!v.empty());
    BOOST_TEST(v.dimensions() == fixture::d);
    BOOST_TEST(v.num_channels() == 4u);
    BOOST_TEST(v.size() == static_cast<std::size_t>(fixture::d.x * fixture::d.y));
}

BOOST_AUTO_TEST_CASE(front)
{
    auto v = gil::planar_rgba_view(fixture::d.x, fixture::d.y, fixture::r, fixture::g, fixture::b, fixture::a, sizeof(std::uint8_t) * 2);
    gil::rgba8_pixel_t const pf{1, 10, 110, 251};
    BOOST_TEST(v.front() == pf);
}

BOOST_AUTO_TEST_CASE(back)
{
    auto v = gil::planar_rgba_view(fixture::d.x, fixture::d.y, fixture::r, fixture::g, fixture::b, fixture::a, sizeof(std::uint8_t) * 2);
    gil::rgba8_pixel_t const pb{4, 40, 140, 254};
    BOOST_TEST(v.back() == pb);
}

BOOST_AUTO_TEST_CASE(pixel_equal_to_operator)
{
    auto v = gil::planar_rgba_view(fixture::d.x, fixture::d.y, fixture::r, fixture::g, fixture::b, fixture::a, sizeof(std::uint8_t) * 2);
    for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(v.size()); i++)
    {
        gil::rgba8_pixel_t const p{fixture::r[i], fixture::g[i], fixture::b[i], fixture::a[i]};
        BOOST_TEST(v[i] == p);
    }
}
