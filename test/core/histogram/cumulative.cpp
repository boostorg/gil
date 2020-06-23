//
// Copyright 2020 Debabrata Mandal <mandaldebabrata123@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//

#include <boost/gil/histogram.hpp>

#include <boost/core/lightweight_test.hpp>

namespace gil = boost::gil;

void check_cumulative()
{
    gil::histogram<int> h1;
    for (int i = 0; i < 8; i++)
    {
        h1(i) = 1;
    }
    h1.cumulative();
    bool check1 = true;
    for (int i = 0; i < 8; i++)
    {
        if(h1(i) != i+1)
            check1 = false;
    }    
    BOOST_TEST(check1);

    gil::histogram<int , int> h2;
    h2(1, 3) = 1;
    h2(1, 4) = 2;
    h2(2, 1) = 3;
    h2(2, 2) = 1;
    h2(2, 5) = 2;
    h2(3, 2) = 3;
    h2(3, 9) = 1;
    h2.cumulative();
    BOOST_TEST(h2(1, 3) == 1);
    BOOST_TEST(h2(1, 4) == 3);
    BOOST_TEST(h2(2, 1) == 6);
    BOOST_TEST(h2(2, 2) == 7);
    BOOST_TEST(h2(2, 5) == 9);
    BOOST_TEST(h2(3, 2) == 12);
    BOOST_TEST(h2(3, 9) == 13);
}

void check_cumulative_histogram()
{
    gil::histogram<int , int> h2;
    h2(1, 3) = 1;
    h2(1, 4) = 2;
    h2(2, 1) = 3;
    h2(2, 2) = 1;
    h2(2, 5) = 2;
    h2(3, 2) = 3;
    h2(3, 9) = 1;
    gil::cumulative_histogram(h2);
    BOOST_TEST(h2(1, 3) == 1);
    BOOST_TEST(h2(1, 4) == 3);
    BOOST_TEST(h2(2, 1) == 6);
    BOOST_TEST(h2(2, 2) == 7);
    BOOST_TEST(h2(2, 5) == 9);
    BOOST_TEST(h2(3, 2) == 12);
    BOOST_TEST(h2(3, 9) == 13);
}

int main() {

    check_cumulative();
    check_cumulative_histogram();

    return boost::report_errors();
}   