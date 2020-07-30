//
// Copyright 2020 Debabrata Mandal <mandaldebabrata123@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//

#include <boost/gil/histogram.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/typedefs.hpp>

#include <boost/core/lightweight_test.hpp>
#include <iostream>

namespace gil = boost::gil;
namespace mp11 = boost::mp11;

std::uint8_t big_matrix[] = 
{
    1, 2, 3, 4, 5, 6, 7, 8,
    1, 2, 1, 2, 1, 2, 1, 2,
    1, 2, 3, 4, 5, 6, 7, 8,
    3, 4, 3, 4, 3, 4, 3, 4,
    1, 2, 3, 4, 5, 6, 7, 8,
    5, 6, 5, 6, 5, 6, 5, 6,
    1, 2, 3, 4, 5, 6, 7, 8,
    7, 8, 7, 8, 7, 8, 7, 8
};

void check_normalize()
{
    auto epsilon = 1e-6;
    // 1D histogram
    double expected[64];
    gil::histogram<int> h1;
    int sum = 0;
    for (std::size_t i = 0; i < 64; i++)
    {
        h1(i) = big_matrix[i];
        sum += big_matrix[i];
    }
    for (std::size_t i = 0; i < 64; i++)
    {
        expected[i] = double(big_matrix[i]) / sum;
    }
    h1.normalize();

    bool check = true;
    for (std::size_t i = 0; i < 64; i++)
    {
        check = check & (abs(expected[i] - h1(i)) < epsilon); 
    }
    BOOST_TEST(check);

    // 2D histogram
    double expected2[8][8];
    gil::histogram<int, int> h2;
    int sum2 = 0;
    for (std::size_t i = 0; i < 64; i++)
    {
        h2(i/8, i%8) = big_matrix[i];
        sum2 += big_matrix[i];
    }
    for (std::size_t i = 0; i < 64; i++)
    {
        expected2[i/8][i%8] = double(big_matrix[i]) / sum2;
    }
    h2.normalize();

    bool check2 = true;
    for (std::size_t i = 0; i < 64; i++)
    {
        check2 = check2 & (abs(expected2[i/8][i%8] - h2(i/8,i%8)) < epsilon); 
    }
    BOOST_TEST(check2);
}

void check_nearest_key()
{
    {
        gil::histogram<int> h1;
        h1(1) = 1;
        h1(3) = 4;
        h1(4) = 4;
        h1(6) = 1;
        std::tuple<int> k1{2}, k2{3}, k3{5};
        std::tuple<int> k1_expected{1}, k2_expected{3}, k3_expected{4};
        BOOST_TEST(k1_expected == h1.nearest_key(k1));
        BOOST_TEST(k2_expected == h1.nearest_key(k2));
        BOOST_TEST(k3_expected == h1.nearest_key(k3));
    }

    {
        gil::histogram<int, int> h2;
        h2(1, 1) = 1;
        h2(1, 4) = 1;
        h2(2, 4) = 1;
        h2(4, 4) = 1;
        std::tuple<int, int> k1(1, 1), k2(1, 3), k3(2, 1), k4(2, 7), k5(4, 4);
        std::tuple<int, int> k1_exp(1, 1), k2_exp(1, 1), k3_exp(1, 4), k4_exp(2, 4), k5_exp(4, 4);
        BOOST_TEST(k1_exp == h2.nearest_key(k1));
        BOOST_TEST(k2_exp == h2.nearest_key(k2));
        BOOST_TEST(k3_exp == h2.nearest_key(k3));
        BOOST_TEST(k4_exp == h2.nearest_key(k4));
        BOOST_TEST(k5_exp == h2.nearest_key(k5));
    }

}

int main() {

    check_normalize();
    check_nearest_key();

    return boost::report_errors();
}   
