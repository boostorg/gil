//
// Copyright 2018 Mateusz Loskot <mateusz at loskot dot net>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include <boost/gil/point.hpp>

#include <boost/core/lightweight_test.hpp>

namespace gil = boost::gil;

int main()
{
    // default constructor
    {
        gil::point<int> p;
        BOOST_TEST(p.x == 0);
        BOOST_TEST(p.y == 0);
    }
    // user-defined constructor
    {
        gil::point<int> p{1, 2};
        BOOST_TEST(p.x == 1);
        BOOST_TEST(p.y == 2);
    }
    // index operator
    {
        gil::point<int> p{1, 2};
        BOOST_TEST(p[0] == 1);
        BOOST_TEST(p[1] == 2);
    }
    // arithmetic: add
    {
        gil::point<int> p1{1, 1};
        gil::point<int> const p2{2, 4};
        p1 += p2;
        BOOST_TEST(p1.x == 3);
        BOOST_TEST(p1.y == 5);
    }
    // arithmetic: subtract
    {
        gil::point<int> p1{2, 4};
        gil::point<int> const p2{1, 1};
        p1 = p1 - p2;
        BOOST_TEST(p1.x == 1);
        BOOST_TEST(p1.y == 3);
        p1 -= p2;
        BOOST_TEST(p1.x == 0);
        BOOST_TEST(p1.y == 2);
    }
    // arithmetic: opposite
    {
        gil::point<int> p1{2, 4};
        auto p2 = -p1;
        BOOST_TEST(p2.x == -2);
        BOOST_TEST(p2.y == -4);
        p2 = -p2;
        BOOST_TEST(p2.x == p1.x);
        BOOST_TEST(p2.y == p1.y);

    }
    {
    }
    // arithmetic: divide by scalar
    {
        gil::point<int> p1{2, 4};
        p1 /= 2;
        BOOST_TEST(p1.x == 1);
        BOOST_TEST(p1.y == 2);
    }
    // bitwise left-shift
    {
        gil::point<unsigned int> p{2, 4};
        p = p << 1;
        BOOST_TEST(p.x == 2 * 2);
        BOOST_TEST(p.y == 2 * 4);
    }
    // right-shift
    {
        gil::point<unsigned int> p{2, 4};
        p = p >> 1;
        BOOST_TEST(p.x == 2 / 2);
        BOOST_TEST(p.y == 4 / 2);
    }
    // compare: equal
    {
        gil::point<int> p1{ 2, 4 };
        gil::point<int> p2{ 2, 4 };
        BOOST_TEST(p1 == p2);
    }
    // compare: not equal
    {
        gil::point<int> p1{ 1, 1 };
        gil::point<int> p2{ 2, 4 };
        BOOST_TEST(p1 != p2);
    }
    return boost::report_errors();
}
