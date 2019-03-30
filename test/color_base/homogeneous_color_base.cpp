//
// Copyright 2019 Mateusz Loskot <mateusz at loskot dot net>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include <boost/gil/color_base.hpp>
#include <boost/gil/gray.hpp>
#include <boost/gil/rgb.hpp>
#include <boost/gil/rgba.hpp>
#include <boost/mpl/int.hpp>
#include <boost/core/typeinfo.hpp>

#define BOOST_TEST_MODULE test_channel_traits
#include "unit_test.hpp"

namespace gil = boost::gil;
namespace mpl = boost::mpl;

namespace {

template <int N>
using color_base = gil::detail::homogeneous_color_base<std::uint8_t, gil::gray_layout_t, N>;

mpl::int_<0> e0;
mpl::int_<1> e1;
mpl::int_<2> e2;
mpl::int_<3> e3;
mpl::int_<4> e4;

} // unnamed namespace

BOOST_AUTO_TEST_CASE(homogeneous_color_base_1_default_constructor)
{
    using fixture = color_base<1>;
    fixture f;
    BOOST_TEST(std::uint8_t{f} == std::uint8_t{0});
    BOOST_TEST(f.at(e0) == std::uint8_t{0});
}

BOOST_AUTO_TEST_CASE(homogeneous_color_base_1_value_constructor)
{
    using fixture = color_base<1>;
    fixture f{1};
    BOOST_TEST(std::uint8_t{f} == std::uint8_t{1});
    BOOST_TEST(f.at(e0) == std::uint8_t{1});
}

BOOST_AUTO_TEST_CASE(homogeneous_color_base_2_default_constructor)
{
    using fixture = color_base<2>;
    fixture f;
    BOOST_TEST(f.at(e0) == std::uint8_t{0});
    BOOST_TEST(f.at(e1) == std::uint8_t{0});
}

BOOST_AUTO_TEST_CASE(homogeneous_color_base_2_value_constructor)
{
    using fixture = color_base<2>;
    fixture f{2};
    BOOST_TEST(f.at(e0) == std::uint8_t{2});
    BOOST_TEST(f.at(e0) == f.at(e1));
}

BOOST_AUTO_TEST_CASE(homogeneous_color_base_3_default_constructor)
{
    using fixture = color_base<3>;
    fixture f;
    BOOST_TEST(f.at(e0) == std::uint8_t{0});
    BOOST_TEST(f.at(e1) == std::uint8_t{0});
    BOOST_TEST(f.at(e2) == std::uint8_t{0});
}


BOOST_AUTO_TEST_CASE(homogeneous_color_base_3_value_constructor)
{
    using fixture = color_base<3>;
    fixture f{3};
    BOOST_TEST(f.at(e0) == std::uint8_t{3});
    BOOST_TEST(f.at(e0) == f.at(e1));
    BOOST_TEST(f.at(e0) == f.at(e2));
}

BOOST_AUTO_TEST_CASE(homogeneous_color_base_4_default_constructor)
{
    using fixture = color_base<4>;
    fixture f;
    BOOST_TEST(f.at(e0) == std::uint8_t{0});
    BOOST_TEST(f.at(e1) == std::uint8_t{0});
    BOOST_TEST(f.at(e2) == std::uint8_t{0});
    BOOST_TEST(f.at(e3) == std::uint8_t{0});
}

BOOST_AUTO_TEST_CASE(homogeneous_color_base_4_value_constructor)
{
    using fixture = color_base<4>;
    fixture f{4};
    BOOST_TEST(f.at(e0) == std::uint8_t{4});
    BOOST_TEST(f.at(e0) == f.at(e1));
    BOOST_TEST(f.at(e0) == f.at(e2));
    BOOST_TEST(f.at(e0) == f.at(e3));
}

BOOST_AUTO_TEST_CASE(homogeneous_color_base_5_default_constructor)
{
    using fixture = color_base<5>;
    fixture f;
    BOOST_TEST(f.at(e0) == std::uint8_t{0});
    BOOST_TEST(f.at(e1) == std::uint8_t{0});
    BOOST_TEST(f.at(e2) == std::uint8_t{0});
    BOOST_TEST(f.at(e3) == std::uint8_t{0});
    BOOST_TEST(f.at(e4) == std::uint8_t{0});
}

BOOST_AUTO_TEST_CASE(homogeneous_color_base_5_value_constructor)
{
    using fixture = color_base<5>;
    fixture f{5};
    BOOST_TEST(f.at(e0) == f.at(e1));
    BOOST_TEST(f.at(e0) == f.at(e2));
    BOOST_TEST(f.at(e0) == f.at(e3));
    BOOST_TEST(f.at(e0) == f.at(e4));
}
