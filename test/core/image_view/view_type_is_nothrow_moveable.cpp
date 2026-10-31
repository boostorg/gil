//
// Copyright 2022 Marco Langer <langer.m86 at gmail dot com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include "core/image/test_fixture.hpp"

#include <boost/gil.hpp>

#include <boost/core/lightweight_test.hpp>
#include <boost/mp11.hpp>

#include <type_traits>

namespace gil = boost::gil;
namespace mp11 = boost::mp11;
namespace fixture = boost::gil::test::fixture;

struct test_move_nothrow
{
    template <typename Image>
    void operator()(Image const&)
    {
        using view_t = typename Image::view_t;

        BOOST_ASSERT(std::is_nothrow_move_constructible<view_t>::value);
        BOOST_ASSERT(std::is_nothrow_move_assignable<view_t>::value);
    }

    static void run()
    {
        mp11::mp_for_each<fixture::image_types>(test_move_nothrow{});
        mp11::mp_for_each<fixture::rgb_planar_image_types>(test_move_nothrow{});
        mp11::mp_for_each<fixture::rgb_interleaved_image_types>(test_move_nothrow{});
    }
};

int main()
{
    test_move_nothrow::run();

    return ::boost::report_errors();
}
