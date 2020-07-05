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

namespace gil = boost::gil;
namespace mp11 = boost::mp11;

gil::gray8_image_t img1(4, 4, gil::gray8_pixel_t(1));
gil::gray8_view_t v1 = view(img1);

gil::rgb8_image_t img2(4, 4, gil::rgb8_pixel_t(1));
gil::rgb8_view_t v2 = view(img2);

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

int main() {

    check_histogram_fill();
    check_histogram_fill_algorithm();
    check_normalize();

    return boost::report_errors();
}   
