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

#include <boost/core/lightweight_test.hpp>

namespace gil = boost::gil;

int main()
{
    boost::function_requires<gil::RandomAccessNDImageViewConcept<gil::gray8_view_t>>();

     return boost::report_errors();
}
