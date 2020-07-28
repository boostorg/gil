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

gil::gray8_image_t img1(4, 4, gil::gray8_pixel_t(1));
gil::gray8_view_t v1 = view(img1);

gil::rgb8_image_t img2(4, 4, gil::rgb8_pixel_t(1));
gil::rgb8_view_t v2 = view(img2);

std::uint8_t sparse_matrix[] = 
{
    1, 1, 1, 1, 
    3, 3, 3, 3, 
    5, 5, 5, 5,
    7, 7, 7, 7
};

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

std::uint8_t big_rgb_matrix[] = 
{
    1, 2, 3, 2, 3, 4, 3, 4, 5, 4, 5, 6, 5, 6, 7, 6, 7, 8, 7, 8, 9, 8, 9, 10,
    1, 2, 3, 2, 3, 4, 1, 2, 3, 2, 3, 4, 1, 2, 3, 2, 3, 4, 1, 2, 3, 2, 3, 4,
    1, 2, 3, 2, 3, 4, 3, 4, 5, 4, 5, 6, 5, 6, 7, 6, 7, 8, 7, 8, 9, 8, 9, 10,
    3, 4, 5, 4, 5, 6, 3, 4, 5, 4, 5, 6, 3, 4, 5, 4, 5, 6, 3, 4, 5, 4, 5, 6,
    1, 2, 3, 2, 3, 4, 3, 4, 5, 4, 5, 6, 5, 6, 7, 6, 7, 8, 7, 8, 9, 8, 9, 10,
    5, 6, 7, 6, 7, 8, 5, 6, 7, 6, 7, 8, 5, 6, 7, 6, 7, 8, 5, 6, 7, 6, 7, 8,
    1, 2, 3, 2, 3, 4, 3, 4, 5, 4, 5, 6, 5, 6, 7, 6, 7, 8, 7, 8, 9, 8, 9, 10,
    7, 8, 9, 8, 9, 10, 7, 8, 9, 8, 9, 10, 7, 8, 9, 8, 9, 10, 7, 8, 9, 8, 9, 10,
};

std::vector<std::vector<bool>> mask =
{
    {1, 0, 0, 1},
    {0, 0, 1, 1},
    {0, 1, 0, 1},
    {1, 1, 0, 0},
};

gil::gray8c_view_t sparse_gray_view = gil::interleaved_view(4, 4, reinterpret_cast<gil::gray8c_pixel_t*>(sparse_matrix), 4);

gil::gray8c_view_t big_gray_view = gil::interleaved_view(8, 8, reinterpret_cast<gil::gray8c_pixel_t*>(big_matrix), 8);

gil::rgb8c_view_t big_rgb_view = gil::interleaved_view(8, 8, reinterpret_cast<gil::rgb8c_pixel_t*>(big_rgb_matrix), 24);

void check_histogram_fill() 
{
    gil::histogram<int> h1, h2;
    gil::histogram<int, int, int> h3;

    h1.fill(big_gray_view);

    bool check_gray_fill = true;
    for (std::size_t i = 1; i <= 8; ++i)
    {
        if(h1(i) != 8)
        {
            check_gray_fill = false;
        }
    }
    BOOST_TEST(check_gray_fill);

    h3.fill(big_rgb_view);

    bool check_rgb_fill = true;
    for (std::size_t i = 1; i <= 8; ++i)
    {
        if(h3(i, i+1, i+2) != 8)
        {
            check_rgb_fill = false;
        }
    }
    BOOST_TEST(check_rgb_fill);

    h2.fill<1>(big_rgb_view);
    bool check_gray_fill2 = true;
    for (std::size_t i = 1; i <= 8; ++i)
    {
        if(h2(i+1) != 8)
        {
            check_gray_fill2 = false;
        }
    }
    BOOST_TEST(check_gray_fill2);

    // Check with limits
    std::tuple<int> lower{2}, higher{6};
    h1.clear();
    h1.fill(big_gray_view, false, {{}}, lower, higher, true);
    check_gray_fill = true;
    for (std::size_t i = 1; i <= 8; ++i)
    {
        if(i<2 || i>6)
        {
            check_gray_fill = check_gray_fill & (h1(i)==0);continue;
        }
        if(h1(i) != 8)
        {
            check_gray_fill = false;
        }
    }
    BOOST_TEST(check_gray_fill);

    std::tuple<int, int ,int> lower1{2,2,2}, higher1{6,6,6};
    h3.clear();
    h3.fill(big_rgb_view, false, {{}}, lower1, higher1, true);

    check_rgb_fill = true;
    for (std::size_t i = 1; i <= 8; ++i)
    {
        if(!(i >= 2 && (i+2) <= 6))
        {
            check_rgb_fill = check_rgb_fill & (h3(i, i+1, i+2)==0);continue;
        }
        if(h3(i, i+1, i+2) != 8)
        {
            check_rgb_fill = false;
        }
    }
    BOOST_TEST(check_rgb_fill);

    h2.clear();
    // std::tuple<int> lower{2}, higher{6};
    h2.fill<1>(big_rgb_view, false, {{}}, lower, higher, true);
    check_gray_fill2 = true;
    for (std::size_t i = 1; i <= 8; ++i)
    {
        if(i+1 < 2 || i+1 > 6)
        {
            check_gray_fill = check_gray_fill & (h2(i+1)==0);continue;
        }
        if(h2(i+1) != 8)
        {
            check_gray_fill2 = false;
        }
    }
    BOOST_TEST(check_gray_fill2);

    //Check mask
    gil::histogram<int> h4;

    gil::fill_histogram(sparse_gray_view, h4, false, false, true, mask);
    
    bool check_1d = true;
    for (std::size_t i = 1; i <= 8; ++i)
    {
        if(i%2==1)
        {
            check_1d = check_1d & (h4(i)==2);
        }
    }
    BOOST_TEST(check_1d);
}

void check_histogram_fill_algorithm() 
{
    gil::histogram<int> h1;

    gil::fill_histogram<1>(big_rgb_view, h1);
    
    bool check_1d = true;
    for (std::size_t i = 1; i <= 8; ++i)
    {
        if(h1(i+1) != 8)
        {
            check_1d = false;
        }
    }
    BOOST_TEST(check_1d);

    gil::histogram<int, int> h2;

    gil::fill_histogram<2, 1>(big_rgb_view, h2);
    
    bool check_2d = true;
    for (std::size_t i = 1; i <= 8; ++i)
    {
        if(h2(i+2, i+1) != 8)
        {
            check_2d = false;
        }
    }
    BOOST_TEST(check_2d);

    gil::histogram<int> h3;

    std::tuple<int> low(1), high(8);
    gil::fill_histogram(sparse_gray_view, h3, false, false, false, {{}}, low, high, true);
    
    check_1d = true;
    for (std::size_t i = 1; i <= 8; ++i)
    {
        if(h3.find(i) == h3.end())
        {
            check_1d = false;
        }
        else
        {
            check_1d = check_1d & (i % 2 == 1 ? (h3(i) == 4) : (h3(i) == 0));
        }
    }
    BOOST_TEST(check_1d);
}

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

    check_histogram_fill();
    check_histogram_fill_algorithm();
    check_normalize();
    check_nearest_key();

    return boost::report_errors();
}   
