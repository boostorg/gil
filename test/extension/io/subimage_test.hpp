//
// Copyright 2013 Christian Henning
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_IO_TEST_SUBIMAGE_TEST_HPP
#define BOOST_GIL_IO_TEST_SUBIMAGE_TEST_HPP

#include <boost/gil.hpp>

#include <boost/core/lightweight_test.hpp>

#include <string>

namespace gil = boost::gil;

template <typename Image, typename Format>
void run_subimage_test(
    std::string const& filename, gil::point_t const& top_left, gil::point_t const& dimension)
{
    Image original, subimage;
    gil::read_image(filename, original, Format{});
    gil::image_read_settings<Format> settings(top_left, dimension);
    gil::read_image(filename, subimage, settings);
    BOOST_TEST(gil::equal_pixels(
        gil::const_view(subimage),
        gil::subimage_view(gil::const_view(original), top_left, dimension)));
}

#endif // BOOST_GIL_IO_TEST_SUBIMAGE_TEST_HPP
