//
// Copyright 2018 Mateusz Loskot <mateusz at loskot dot net>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#define BOOST_TEST_MODULE gil/test/core/image_view/collection
#include "unit_test.hpp"

#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/typedefs.hpp>

#include <iterator>

namespace gil = boost::gil;

namespace
{
gil::gray8_pixel_t const gray128(128);
gil::gray8_pixel_t const gray255(255);
} // unnamed namespace

BOOST_TEST_DONT_PRINT_LOG_VALUE(std::reverse_iterator<boost::gil::iterator_from_2d<boost::gil::gray8_loc_t>>)
BOOST_TEST_DONT_PRINT_LOG_VALUE(boost::gil::iterator_from_2d<boost::gil::gray8_loc_t>)

// Collection

BOOST_AUTO_TEST_CASE(begin)
{
    gil::gray8_image_t image(2, 2, gray255);
    auto view = gil::view(image);
    BOOST_TEST(*view.begin() == gray255);
}

BOOST_AUTO_TEST_CASE(end)
{
    gil::gray8_image_t::view_t view;
    BOOST_TEST(view.begin() == view.end());
}

BOOST_AUTO_TEST_CASE(empty)
{
    gil::gray8_image_t::view_t view;
    BOOST_TEST(view.empty());

    gil::gray8_image_t image(2, 2);
    view = gil::view(image);
    BOOST_TEST(!view.empty());
}

BOOST_AUTO_TEST_CASE(size)
{
    gil::gray8_image_t::view_t view;
    BOOST_TEST(view.size() == 0);

    gil::gray8_image_t image(2, 2);
    view = gil::view(image);
    BOOST_TEST(view.size() == 4);
}

BOOST_AUTO_TEST_CASE(swap)
{
    gil::gray8_image_t::view_t view1;
    gil::gray8_image_t::view_t view2;

    gil::gray8_image_t image(2, 2);
    view1 = gil::view(image);
    view1.swap(view2);

    BOOST_TEST(view1.empty());
    BOOST_TEST(!view2.empty());
}

// ForwardCollection

BOOST_AUTO_TEST_CASE(back)
{
    gil::gray8_image_t image(2, 2, gray255);
    auto view = gil::view(image);
    BOOST_TEST(view.back() == gray255);
}

BOOST_AUTO_TEST_CASE(front)
{
    gil::gray8_image_t image(2, 2, gray255);
    auto view = gil::view(image);
    BOOST_TEST(view.front() == gray255);
}

// ReversibleCollection

BOOST_AUTO_TEST_CASE(rbegin)
{
    gil::gray8_image_t image(2, 2, gray255);
    auto view = gil::view(image);
    view(1,1) = gray128;
    BOOST_TEST(*view.rbegin() == gray128);
}

BOOST_AUTO_TEST_CASE(rend)
{
    gil::gray8_image_t::view_t view;
    BOOST_TEST(view.rbegin() == view.rend());
}
