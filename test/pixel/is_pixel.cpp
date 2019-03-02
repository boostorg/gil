//
// Copyright 2019 Mateusz Loskot <mateusz at loskot dot net>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include <boost/gil/pixel.hpp>
#include <boost/gil/concepts/pixel.hpp>
#include <boost/gil/typedefs.hpp>

#include <boost/mp11.hpp>

#include "test_fixture.hpp"

namespace gil = boost::gil;
using namespace boost::mp11;

int main()
{
    static_assert(std::is_same
        <
            mp_all_of<gil::test::fixture::pixel_typedefs, gil::is_pixel>,
            std::true_type
        >::value,
        "is_pixel does not yield true for all core pixel types");

    static_assert(std::is_same
        <
            mp_all_of
            <
                mp_transform
                <
                    gil::test::fixture::nested_pixel_type,
                    gil::test::fixture::representative_pixel_types
                >,
                gil::is_pixel
            >,
            std::true_type
        >::value,
        "is_pixel does not yield true for representative pixel types");


    static_assert(std::is_same
        <
            mp_all_of<gil::test::fixture::non_pixels, gil::is_pixel>,
            std::false_type
        >::value,
        "is_pixel yields true for non-pixel type");

    static_assert(std::is_same
        <
            mp_none_of<gil::test::fixture::non_pixels, gil::is_pixel>,
            std::true_type
        >::value,
        "is_pixel yields true for non-pixel type");
}
