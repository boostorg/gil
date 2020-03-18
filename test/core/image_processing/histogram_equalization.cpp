//
// Copyright 2019 Debabrata Mandal <mandaldebabrata123@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/gil/image_processing/histogram_equalization.hpp>
#include <boost/core/lightweight_test.hpp>

#include <vector>

namespace gil = boost::gil;

int a=5;
boost::gil::gray8_image_t original(a, a);
boost::gil::gray8_image_t processed(a, a), expected(a, a);
std::vector<std::vector<int> > test1_random{
    {  1,   10,  10,  10,  10},
    {  20,  25,  25,  55,  20},
    {  0,   55,  55,  55,  20},
    {  20,  255, 255, 255, 0},
    {  100, 100, 100, 10,  0}};
std::vector<std::vector<int> > expected_test1{
    {  40,  91,  91,  91,  91},
    {  132, 153, 153, 193, 132},
    {  30,  193, 193, 193, 132},
    {  132, 255, 255, 255, 30},
    {  224, 224, 224, 91,  30}};

std::vector<std::vector<int> > test2_uniform{
    {  0,   10,  20,  30,  40},
    {  50,  60,  70,  80,  90},
    {  100, 110, 120, 130, 140},
    {  150, 160, 170, 180, 190},
    {  200, 210, 220, 230, 240}};
std::vector<std::vector<int> > expected_test2{
    {  10,  20,  30,  40,  51},
    {  61,  71,  81,  91,  102},
    {  112, 122, 132, 142, 153},
    {  163, 173, 183, 193, 204},
    {  214, 224, 234, 244, 255}};

std::vector<std::vector<int> > test3_2peaks{
    {  0,   0,   0,   0,   10},
    {  40,  43,  44,  46,  50},
    {  55,  56,  44,  46,  44},
    {  200, 201, 202, 203, 200},
    {  201, 202, 201, 201, 22}};
std::vector<std::vector<int> > expected_test3{
    {  40,  40,  40,  40,  51},
    {  71,  81,  112, 132, 142},
    {  153, 163, 112, 132, 112},
    {  183, 224, 244, 255, 183},
    {  224, 244, 224, 224, 61}};

void vector_to_gray_image(boost::gil::gray8_image_t& img,
    std::vector<std::vector<int> >& grid)
{
    for(std::ptrdiff_t y=0; y<grid.size(); ++y)
    {
        for(std::ptrdiff_t x=0; x<grid[0].size(); ++x)
        {
            boost::gil::view(img)(x,y) = boost::gil::gray8_pixel_t(grid[y][x]);      
        }
    }
}

void test_random_image()
{
    vector_to_gray_image(original,test1_random);
    vector_to_gray_image(expected,expected_test1);
    histogram_equalization(boost::gil::const_view(original),boost::gil::view(processed));
    BOOST_TEST(boost::gil::equal_pixels(boost::gil::view(processed), boost::gil::view(expected)));
}

void test_uniform_image()
{
    vector_to_gray_image(original,test2_uniform);
    vector_to_gray_image(expected,expected_test2);
    histogram_equalization(boost::gil::const_view(original),boost::gil::view(processed));
    BOOST_TEST(boost::gil::equal_pixels(boost::gil::view(processed), boost::gil::view(expected)));
}

void test_double_peaked_image()
{
    vector_to_gray_image(original,test3_2peaks);
    vector_to_gray_image(expected,expected_test3);
    histogram_equalization(boost::gil::const_view(original),boost::gil::view(processed));
    BOOST_TEST(boost::gil::equal_pixels(boost::gil::view(processed), boost::gil::view(expected)));
}

int main()
{
    //Basic tests for grayscale histogram_equalization
    test_random_image();
    test_uniform_image();
    test_double_peaked_image();

    return boost::report_errors();
}