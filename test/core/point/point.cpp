//
// Copyright 2018 Mateusz Loskot <mateusz at loskot dot net>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#define BOOST_TEST_MODULE gil/test/core/point/point
#include "unit_test.hpp"

#include <boost/gil/point.hpp>

#include <type_traits>

namespace gil = boost::gil;

BOOST_AUTO_TEST_CASE(default_constructor)
{
    gil::point<int> p;
    BOOST_TEST(p.x == 0);
    BOOST_TEST(p.y == 0);
}

BOOST_AUTO_TEST_CASE(user_constructor)
{
    gil::point<int> p{1, 2};
    BOOST_TEST(p.x == 1);
    BOOST_TEST(p.y == 2);
}

BOOST_AUTO_TEST_CASE(copy_constructor)
{
    gil::point<int> p1{1, 2};
    gil::point<int> p2{p1};
    BOOST_TEST(p1.x == p2.x);
    BOOST_TEST(p1.y == p2.y);
}

BOOST_AUTO_TEST_CASE(copy_assignment_operator)
{
    gil::point<int> p1{1, 2};
    gil::point<int> p2;
    p2 = p1;
    BOOST_TEST(p1.x == p2.x);
    BOOST_TEST(p1.y == p2.y);
}

BOOST_AUTO_TEST_CASE(index_operator)
{
    gil::point<int> p{1, 2};
    BOOST_TEST(p[0] == 1);
    BOOST_TEST(p[1] == 2);
}

BOOST_AUTO_TEST_CASE(addition_operator)
{
    gil::point<int> p1{1, 1};
    gil::point<int> const p2{2, 4};
    auto p3 = p1 + p2;
    BOOST_TEST(p3.x == 3);
    BOOST_TEST(p3.y == 5);
}

BOOST_AUTO_TEST_CASE(addition_assignment_operator)
{
    gil::point<int> p1{1, 1};
    gil::point<int> const p2{2, 4};
    p1 += p2;
    BOOST_TEST(p1.x == 3);
    BOOST_TEST(p1.y == 5);
}

BOOST_AUTO_TEST_CASE(subtraction_assignment_operator)
{
    gil::point<int> p1{2, 4};
    gil::point<int> const p2{1, 1};
    p1 -= p2;
    BOOST_TEST(p1.x == 1);
    BOOST_TEST(p1.y == 3);
}

BOOST_AUTO_TEST_CASE(subtraction_operator)
{
    gil::point<int> p1{2, 4};
    gil::point<int> const p2{1, 1};
    p1 = p1 - p2;
    BOOST_TEST(p1.x == 1);
    BOOST_TEST(p1.y == 3);
}

BOOST_AUTO_TEST_CASE(unary_minus_operator)
{
    gil::point<int> p1{2, 4};
    auto p2 = -p1;
    BOOST_TEST(p2.x == -2);
    BOOST_TEST(p2.y == -4);
    p2 = -p2;
    BOOST_TEST(p2.x == p1.x);
    BOOST_TEST(p2.y == p1.y);
}

BOOST_AUTO_TEST_CASE(division_assignment_operator)
{
    {
        gil::point<int> p1{2, 4};
        p1 /= 2;
        static_assert(std::is_same<decltype((p1 / short{}).x), int>::value, "!int");
        static_assert(std::is_same<decltype((p1 / int{}).x), int>::value, "!int");
        static_assert(std::is_same<decltype((p1 / float{}).x), float>::value, "!float");
        static_assert(std::is_same<decltype((p1 / double{}).x), double>::value, "!double");
        BOOST_TEST(p1.x == 1);
        BOOST_TEST(p1.y == 2);
    }
    // point / d
    {
        gil::point<int> p1{2, 4};
        auto p2 = p1 / float{2};
        static_assert(std::is_same<decltype((p2 / int{}).x), float>::value, "!float");
        static_assert(std::is_same<decltype((p2 / float{}).x), float>::value, "!float");
        static_assert(std::is_same<decltype((p2 / double{}).x), double>::value, "!double");
        BOOST_TEST(p2.x >= 1.0); // means == but >= avoids compiler warning
        BOOST_TEST(p2.y >= 2.0);
    }
}

BOOST_AUTO_TEST_CASE(multiplication_operator)
{
    gil::point<int> p1{2, 4};
    p1 *= 2;
    BOOST_TEST(p1.x == 4);
    BOOST_TEST(p1.y == 8);

    // point * m
    {
        auto p2 = p1 * int{2};
        static_assert(std::is_same<decltype(p2.x), int>::value, "!int");
        static_assert(std::is_same<decltype(p2.y), int>::value, "!int");
        BOOST_TEST(p2.x == 8);
        BOOST_TEST(p2.y == 16);
    }
    // m * point
    {
        auto p2 = double{2} *p1;
        static_assert(std::is_same<decltype(p2.x), double>::value, "!double");
        static_assert(std::is_same<decltype(p2.y), double>::value, "!double");
        BOOST_TEST(p2.x >= 8); // means == but >= avoids compiler warning
        BOOST_TEST(p2.y >= 16);
    }
}

BOOST_AUTO_TEST_CASE(multiplication_assignment_operator)
{
    gil::point<int> p1{2, 4};
    // point * m
    {
        auto p2 = p1 * int{2};
        static_assert(std::is_same<decltype(p2.x), int>::value, "!int");
        static_assert(std::is_same<decltype(p2.y), int>::value, "!int");
        BOOST_TEST(p2.x == 4);
        BOOST_TEST(p2.y == 8);
    }
    // m * point
    {
        auto p2 = double{2} * p1;
        static_assert(std::is_same<decltype(p2.x), double>::value, "!double");
        static_assert(std::is_same<decltype(p2.y), double>::value, "!double");
        BOOST_TEST(p2.x >= 4); // means == but >= avoids compiler warning
        BOOST_TEST(p2.y >= 8);
    }
}

BOOST_AUTO_TEST_CASE(bitwise_left_shift_operator)
{
    gil::point<unsigned int> p{2, 4};
    p = p << 1;
    BOOST_TEST(p.x == 4u);
    BOOST_TEST(p.y == 8u);
}

BOOST_AUTO_TEST_CASE(bitwise_right_shift_operator)
{
    gil::point<unsigned int> p{2, 4};
    p = p >> 1;
    BOOST_TEST(p.x == 2u / 2);
    BOOST_TEST(p.y == 4u / 2);
}

BOOST_AUTO_TEST_CASE(equal_to_operator)
{
    gil::point<int> p1{2, 4};
    gil::point<int> p2{2, 4};
    BOOST_TEST(p1 == p2);
}

BOOST_AUTO_TEST_CASE(not_equal_to_operator)
{
    gil::point<int> p1{1, 1};
    gil::point<int> p2{2, 4};
    BOOST_TEST(p1 != p2);
}

BOOST_AUTO_TEST_CASE(axis_value)
{
    gil::point<int> p1{1, 2};
    gil::point<int> const p2{1, 2};
    BOOST_TEST(gil::axis_value<0>(p1) == p1.x);
    BOOST_TEST(gil::axis_value<1>(p1) == p1.y);
    BOOST_TEST(gil::axis_value<0>(p2) == p2.x);
    BOOST_TEST(gil::axis_value<1>(p2) == p2.y);
}
