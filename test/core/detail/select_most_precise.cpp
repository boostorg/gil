//
// Copyright 2022 Marco Langer
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include <boost/gil/detail/select_most_precise.hpp>
#include <boost/core/lightweight_test.hpp>

#include <type_traits>

namespace gil = boost::gil;

struct my_non_fundamental{};

void test_select_most_precise()
{
    BOOST_TEST((std::is_same
        <
            gil::detail::select_most_precise<double, double>::type,
            double
        >::value));

    BOOST_TEST((std::is_same
        <
            gil::detail::select_most_precise<float, float>::type,
            float
        >::value));

    // float, double
    BOOST_TEST((std::is_same
        <
            gil::detail::select_most_precise<float, double>::type,
            double
        >::value));
    BOOST_TEST((std::is_same
        <
            gil::detail::select_most_precise<double, float>::type,
            double
        >::value));
    BOOST_TEST((std::is_same
        <
            gil::detail::select_most_precise<double, double>::type,
            double
        >::value));
    BOOST_TEST((std::is_same
        <
            gil::detail::select_most_precise<float, float>::type,
            float
        >::value));

    // int, double
    BOOST_TEST((std::is_same
        <
            gil::detail::select_most_precise<int, double>::type,
            double
        >::value));
    BOOST_TEST((std::is_same
        <
            gil::detail::select_most_precise<double, int>::type,
            double
        >::value));

    // double, my_non_fundamental
    BOOST_TEST((std::is_same
        <
            gil::detail::select_most_precise<double, my_non_fundamental>::type,
            my_non_fundamental
        >::value));
    BOOST_TEST((std::is_same
        <
            gil::detail::select_most_precise<my_non_fundamental, double>::type,
            my_non_fundamental
        >::value));
}

int main()
{
    test_select_most_precise();

    return ::boost::report_errors();
}