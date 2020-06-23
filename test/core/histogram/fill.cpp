//
// Copyright 2020 Debabrata Mandal <mandaldebabrata123@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//

#include <boost/gil/histogram.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>

#include <boost/core/lightweight_test.hpp>

namespace gil = boost::gil;
namespace mp11 = boost::mp11;

gil::gray8_image_t img1(4, 4, gil::gray8_pixel_t(1));
gil::gray8_view_t v1 = view(img1);

gil::rgb8_image_t img2(4, 4, gil::rgb8_pixel_t(1));
gil::rgb8_view_t v2 = view(img2);

gil::gray16_image_t img3(4, 4, gil::gray16_pixel_t(1));
gil::gray16_view_t v3 = view(img3);

gil::gray8_image_t big_gray_image(8, 8);

gil::rgb8_image_t big_rgb_image(8, 8);

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

void fill_gray_image(std::ptrdiff_t width, std::ptrdiff_t height, std::uint8_t* matrix, gil::gray8_view_t const& view)
{
    for (std::ptrdiff_t i = 0; i < height; ++i)
    {
        for (std::ptrdiff_t j = 0; j < width; ++j)
        {
            view(i, j) = matrix[i * height + j];
        }
    }
}

void fill_rgb_image(std::ptrdiff_t width, std::ptrdiff_t height, std::uint8_t* matrix, gil::rgb8_view_t const& view)
{
    for (std::ptrdiff_t i = 0; i < height; ++i)
    {
        for (std::ptrdiff_t j = 0; j < width; ++j)
        {
            for(std::size_t k = 0; k < 3; ++k)
                view(i, j)[k] = matrix[i * height + j] + k;
        }
    }
}

void check_histogram_fill() 
{
    fill_gray_image(8, 8, big_matrix, view(big_gray_image));
    fill_rgb_image(8, 8, big_matrix, view(big_rgb_image));

    gil::histogram<int> h1, h2;
    gil::histogram<int, int, int> h3;

    h1.fill(view(big_gray_image));

    bool check_gray_fill = true;
    for (std::size_t i = 1; i <= 8; ++i)
    {
        if(h1(i) != 8)
        {
            check_gray_fill = false;
        }
    }
    BOOST_TEST(check_gray_fill);

    h3.fill(view(big_rgb_image));

    bool check_rgb_fill = true;
    for (std::size_t i = 1; i <= 8; ++i)
    {
        if(h3(i, i+1, i+2) != 8)
        {
            check_rgb_fill = false;
        }
    }
    BOOST_TEST(check_rgb_fill);

    h2.fill<1>(view(big_rgb_image));
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
    fill_rgb_image(8, 8, big_matrix, view(big_rgb_image));
    gil::histogram<int> h1;

    gil::fill_histogram<1>(const_view(big_rgb_image), h1);
    
    bool check_1d = true;
    for (std::size_t i = 1; i <= 8; ++i)
    {
        if(h1(i+1) != 8)
        {
            check_1d = false;
        }
    }
    BOOST_TEST(check_1d);

    fill_rgb_image(8, 8, big_matrix, view(big_rgb_image));
    gil::histogram<int, int> h2;

    gil::fill_histogram<2, 1>(const_view(big_rgb_image), h2);
    
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

int main() {

    check_histogram_fill();
    check_histogram_fill_algorithm();

    return boost::report_errors();
}   
