//
// Copyright 2018 Mateusz Loskot <mateusz at loskot dot net>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include <boost/gil/gil_concept.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/typedefs.hpp>

#define BOOST_TEST_MODULE test_image_view_concepts
#include <gil_test_common.hpp>

namespace gil = boost::gil;

BOOST_AUTO_TEST_CASE(RandomAccessND)
{
    boost::function_requires<gil::RandomAccessNDImageViewConcept<gil::gray8_view_t>>();
}
