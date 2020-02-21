//
// Copyright 2019 Ayush Bansal <abansal1008@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/gil/image_processing/morphology.hpp>
#include <boost/core/lightweight_test.hpp>

#include <vector>

namespace gil = boost::gil;

const int width = 5, height = 5;
const int big_width = 50, big_height = 50;

gil::gray8_image_t original_gray(width, height), processed_gray(width, height), expected_gray(width, height);
gil::gray8_image_t original_gray_big(big_width, big_height), processed_gray_big(big_width, big_height), expected_gray_big(big_width, big_height);

auto original_gray_view = gil::const_view(original_gray);
auto processed_gray_view = gil::view(processed_gray);

std::vector<std::vector<int> > original_gray_dilation_values {
        {  0,   0,   0,   0,   0},
        {  0, 255, 255,   0,   0},
        {  0, 255,   0,   0,   0},
        {  0, 255,   0,   0,   0},
        {  0,   0,   0,   0,   0}};

std::vector<std::vector<int> > original_gray_erosion_values{
        {  0,   0,   0,   0,   0},
        {  0, 255, 255, 255,   0},
        {  0, 255, 255, 255,   0},
        {  0, 255, 255, 255,   0},
        {  0,   0,   0,   0,   0}};

std::vector<std::vector<int> > big_image_values{
{255,255,255,255,255,255,255,250,255,1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{255,255,255,255,255,255,255,255,254,0,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{255,255,255,255,255,253,255,253,255,253,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{255,255,255,255,255,255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,4,0,1,0,0,0,0,0},
{255,255,255,255,255,255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,2,0,3,1,3,0,3,1,2},
{255,255,255,255,255,255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,3,0,0,0,2,254,253,255},
{255,255,255,255,255,255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,255,254,255,255,255,252},
{255,255,255,255,255,255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,253,255,248,255,247,255},
{255,255,255,255,255,255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,251,255,246,252,255,255,255,253,255,255},
{255,255,255,255,255,255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,255,255,251,255,255,255,255,252,255,255,253},
{255,255,255,255,255,255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,250,255,255,255,255,255,253,253,255,255,250,255},
{255,255,255,255,255,255,251,255,254,255,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,255,255,255,255,255,255,255,255,255,255,255,255,255},
{255,255,255,255,255,254,255,255,255,255,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,3,1,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
{255,255,255,255,255,252,254,255,251,250,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,255,250,255,255,255,255,255,255,255,255,255,255,255,255,255},
{255,255,255,255,255,255,254,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,7,0,255,252,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
{255,255,255,255,255,254,254,252,252,253,0,2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,253,255,253,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
{255,255,255,255,255,255,255,255,255,0,1,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,0,255,255,254,254,253,255,255,255,255,255,255,255,255,255,255,255,255,255},
{255,255,255,255,255,253,255,254,0,6,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,255,248,253,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
{255,255,255,255,255,255,253,255,0,0,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,255,255,255,255,255,251,255,255,255,255,255,255,255,255,255,255,255,255,255},
{255,254,255,255,251,255,255,0,3,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,253,255,255,255,252,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
{250,255,251,255,255,255,250,4,0,0,1,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,253,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
{252,255,255,249,255,255,3,0,0,4,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,255,254,253,253,253,255,255,255,255,255,255,255,255,255,255,255,255,255},
{255,255,255,254,255,3,0,5,2,1,0,5,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,254,255,255,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
{254,253,251,255,253,0,1,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,255,251,254,255,254,255,255,255,255,255,255,255,255,255,255,255,255,255},
{251,255,255,252,0,1,7,0,0,3,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,254,255,255,250,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
{255,255,251,5,0,0,0,4,0,5,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,1,252,252,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
{255,252,255,0,0,4,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
{255,252,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,254,255,255,254,255,255,255,255,255,255,255,255,255,255,255,255,255},
{255,1,1,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,2,0,255,255,254,253,255,255,255,255,255,255,255,255,255,255,255,255,255},
{0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,255,246,255,253,255,255,255,255,255,255,255,255,255,255,255,255,255},
{0,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,2,0,5,252,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
{9,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,254,255,255,255,255,255,255,255,255,255,255,255,255,255},
{0,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,5,5,0,251,254,253,255,255,255,255,255,255,255,255,255,255,255,255,255},
{0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,3,5,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
{1,3,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,2,2,0,252,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,7,0,0,1,255,255,251,255,255,253,255,255,255,255,255,255,255,255},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,3,0,1,2,0,255,255,255,252,255,255,255,248,255,255,255,255,255,255},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,0,5,0,0,7,0,254,255,255,255,249,255,251,255,255,255,255,255,255},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,0,0,1,0,247,255,251,255,253,255,252,255,255,255,255,255},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,5,3,0,0,0,0,0,3,4,252,255,255,254,255,255,255,255,255,255,255},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,2,255,255,254,255,255,255,255,255,255,255},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,3,0,0,0,3,4,2,0,0,3,253,255,254,255,255,255,255,255},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,3,0,0,3,1,0,0,0,2,3,0,2,255,252,255,255,255,255,255},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2,0,3,254,255,253,255,250,255},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,2,1,2,0,2,253,0,0,3,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,2,0,7,0,0,2,1,0,4},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,2,0,0,0,0,3,1,1,2,1},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,7,0,1,5,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,0,0,4,0,0,0,3,4,0,1,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,5,0,0,0,1,4}};

std::vector<std::vector<int> > struct_element_3x3{{0,1,0}, {1,1,1}, {0,1,0}};
std::vector<std::vector<int> > struct_element_2x2{{0,1}, {1,1}};
std::vector<std::vector<int> > struct_element_big{
{0,0,0,0,0,1,0,0,0,0,0},
{0,0,0,0,0,1,0,0,0,0,0},
{0,0,0,0,0,1,0,0,0,0,0},
{0,0,0,0,0,1,0,0,0,0,0},
{0,0,0,0,0,1,0,0,0,0,0},
{1,1,1,1,1,1,1,1,1,1,1},
{0,0,0,0,0,1,0,0,0,0,0},
{0,0,0,0,0,1,0,0,0,0,0},
{0,0,0,0,0,1,0,0,0,0,0},
{0,0,0,0,0,1,0,0,0,0,0},
{0,0,0,0,0,1,0,0,0,0,0}};

// Utility function to fill values from a vector into an image view.
void fill_values(std::vector<std::vector<int> > &values, gil::gray8_image_t &img)
{
    for (std::ptrdiff_t y=0; y < (std::ptrdiff_t) values.size(); ++y)
    {
        for (std::ptrdiff_t x=0; x < (std::ptrdiff_t) values[0].size(); ++x)
        {
            gil::view(img)(x,y) = gil::gray8_pixel_t(values[y][x]);
        }
    }
}

// Test Dilation using 3x3 Structuring Element.
void test_dilation_3x3()
{
    std::vector<std::vector<int> > expected_values{
            { 0,   255, 255,   0,   0},
            { 255, 255, 255, 255,   0},
            { 255, 255, 255,   0,   0},
            { 255, 255, 255,   0,   0},
            { 0,   255,   0,   0,   0}};
    fill_values(expected_values, expected_gray);

    gil::dilate(original_gray_view,processed_gray_view,struct_element_3x3);
    BOOST_TEST(gil::equal_pixels(gil::view(processed_gray), gil::view(expected_gray)));
}

// Test Dilation using 3x3 Structuring Element. Checks boundary cases for image.
void test_dilation_3x3_boundary_case()
{
    gil::view(original_gray)(0,0) = (unsigned char) 255, gil::view(original_gray)(1,0) = (unsigned char) 255; // Make the boundary values as 255
    std::vector<std::vector<int> > expected_values{
            {255,	255,	255,	0,	0},
            {255,	255,	255,	255,	0},
            {255,	255,	255,	0,	0},
            {255,	255,	255,	0,	0},
            {0,	255,	0,	0,	0}};
    fill_values(expected_values, expected_gray);

    gil::dilate(original_gray_view,processed_gray_view,struct_element_3x3);
    BOOST_TEST(gil::equal_pixels(gil::view(processed_gray), gil::view(expected_gray)));

    // RESET Image to original values
    fill_values(original_gray_dilation_values, original_gray);
}

// Test Dilation using 2x2 Structuring Element.
void test_dilation_2x2()
{
    std::vector<std::vector<int> > expected_values{
            {0,	0,	0,	0,	0},
            {0,	255,	255,	255,	0},
            {0,	255,	255,	0,	0},
            {0,	255,	255,	0,	0},
            {0,	255,	0,	0,	0}};
    fill_values(expected_values, expected_gray);

    gil::dilate(original_gray_view,processed_gray_view,struct_element_2x2);
    BOOST_TEST(gil::equal_pixels(gil::view(processed_gray), gil::view(expected_gray)));
}

// Test Erosion using 3x3 Structuring Element.
void test_erosion_3x3()
{
    std::vector<std::vector<int> > expected_values(width, std::vector<int>(height,0));
    expected_values[width/2][height/2] = 255;
    fill_values(expected_values, expected_gray);

    gil::erode(original_gray_view,processed_gray_view,struct_element_3x3);
    BOOST_TEST(gil::equal_pixels(gil::view(processed_gray), gil::view(expected_gray)));
}

// Test Erosion using 3x3 Structuring Element. Checks boundary cases for image.
void test_erosion_3x3_boundary_case()
{
    gil::view(original_gray)(0,0) = (unsigned char) 255,
    gil::view(original_gray)(1,0) = (unsigned char) 255,
    gil::view(original_gray)(0,1) = (unsigned char) 255; // Make the boundary values as 255
    std::vector<std::vector<int> > expected_values{
            {255,	0,	0,	0,	0},
            {0,	255,	0,	0,	0},
            {0,	0,	255,	0,	0},
            {0,	0,	0,	0,	0},
            {0,	0,	0,	0,	0}};
    fill_values(expected_values, expected_gray);

    gil::erode(original_gray_view,processed_gray_view,struct_element_3x3);
    BOOST_TEST(gil::equal_pixels(gil::view(processed_gray), gil::view(expected_gray)));

    // RESET Image to original values
    fill_values(original_gray_dilation_values, original_gray);
}

// Test Erosion using 2x2 Structuring Element.
void test_erosion_2x2()
{
    std::vector<std::vector<int> > expected_values{
            {0,	0,	0,	0,	0},
            {0,	0,	0,	0,	0},
            {0,	0,	255,	255,	0},
            {0,	0,	255,	255,	0},
            {0,	0,	0,	0,	0}};
    fill_values(expected_values, expected_gray);

    gil::erode(original_gray_view,processed_gray_view,struct_element_2x2);
    BOOST_TEST(gil::equal_pixels(gil::view(processed_gray), gil::view(expected_gray)));
}

// Big Tests

void test_dilation_big()
{
    std::vector<std::vector<int> > expected_values{
    {255,255,255,255,255,255,255,255,255,255,255,255,255,255,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,2,4,3,1,3,2,254,253,255},
    {255,255,255,255,255,255,255,255,255,255,255,255,255,254,7,7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,1,0,2,4,3,255,254,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,255,255,255,255,253,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,1,0,2,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,4,4,4,251,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3,3,4,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3,3,3,250,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,1,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,5,1,1,3,1,1,1,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,5,1,2,3,3,3,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,5,8,2,7,4,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,5,8,2,7,253,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,2,2,0,0,0,0,0,0,0,0,1,1,1,1,5,8,2,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,2,2,2,0,0,0,0,0,0,5,5,5,5,5,5,8,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,255,255,255,251,250,10,0,0,0,0,0,0,0,0,0,0,2,2,2,5,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,8,8,8,8,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,254,254,253,253,253,3,3,3,0,0,0,0,0,0,0,0,0,0,253,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,255,255,255,255,3,3,3,0,0,0,0,0,0,0,2,2,2,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,255,255,254,6,6,5,5,5,0,0,0,0,0,0,0,0,1,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,255,255,255,2,2,2,1,0,0,0,0,0,0,0,0,2,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,255,255,5,3,1,1,1,0,0,0,0,0,0,0,0,3,253,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,255,250,8,8,8,8,8,0,0,0,0,0,0,0,1,1,253,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,255,8,5,4,4,1,1,1,0,0,0,0,0,0,0,2,2,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,255,5,8,5,5,5,5,5,2,0,0,0,0,0,0,0,5,5,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,255,253,3,8,4,3,3,3,0,0,0,0,0,0,0,0,2,2,2,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,252,7,7,8,4,4,4,4,4,4,0,0,0,0,0,0,1,1,1,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,251,5,5,5,8,5,5,5,1,1,0,0,0,0,0,0,0,0,4,4,4,252,252,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,255,255,4,5,4,5,4,1,0,0,0,0,0,0,0,0,0,0,0,0,3,3,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,252,5,2,5,3,5,4,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,7,4,3,5,3,1,4,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,252,3,4,7,4,1,5,0,1,4,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,5,3,4,3,4,1,5,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4,4,5,252,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,252,255,9,9,9,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,252,3,3,3,3,3,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,5,5,5,251,254,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {255,3,1,2,3,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,3,5,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {9,3,3,3,3,3,3,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,4,4,4,252,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {9,3,1,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,7,7,7,7,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {9,3,1,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,4,4,4,5,6,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {1,3,1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,6,6,7,7,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {1,3,1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,5,6,3,7,247,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {1,3,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,5,5,5,5,5,6,5,7,5,252,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,5,6,3,7,3,3,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,5,5,5,6,5,5,5,5,7,255,255,255,255,255,255,255,255,255,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,3,3,3,5,6,3,5,3,3,7,3,254,255,255,255,255,255,255,255,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,3,3,3,3,2,3,4,247,255,255,255,255,255,255,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,3,3,3,3,2,3,4,7,252,255,255,254,255,255,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,3,3,7,7,7,7,7,7,7,255,255,254,255,255,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,3,3,3,3,3,3,4,7,7,7,4,253,255,254,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,3,0,0,3,7,7,7,7,7,7,7,7,255,254,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,4,4,7,7,7,4,4,7,254,255,253,255,250,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7,7,7,7,7,7,7,7,7,7,253,5,5,5,5}};

    fill_values(expected_values, expected_gray_big);

    auto original_gray_big_view = gil::const_view(original_gray_big);
    auto processed_gray_big_view = gil::view(processed_gray_big);
    gil::dilate(original_gray_big_view,processed_gray_big_view,struct_element_big);
    BOOST_TEST(gil::equal_pixels(gil::view(processed_gray_big), gil::view(expected_gray_big)));
}

void test_erosion_big()
{
    std::vector<std::vector<int> > expected_values{
    {255,255,250,250,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {255,255,255,254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {253,253,253,253,253,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,1,2},
    {255,255,255,255,255,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,250,247,250},
    {255,251,251,251,251,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,253,253,248,253,247,252},
    {254,254,254,254,254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,246,252,253,253,248,253,247,253},
    {252,252,252,251,250,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,250,250,246,252,253,253,252,253,250,253},
    {255,254,254,254,251,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,252,252,251,255,255,253,253,252,255,250,253},
    {250,254,251,252,251,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,250,253,253,255,255,255,253,253,255,255,250,255},
    {250,254,251,249,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,253,253,253,253,253,255,255,255,255,255,255,255,255},
    {250,253,251,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,248,248,253,255,255,255,255,255,255,255,255,255,255},
    {250,253,251,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,250,251,251,251,251,251,255,255,255,255,255,255,255,255},
    {250,251,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,251,252,252,252,252,255,255,255,255,255,255,255,255,255},
    {250,250,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,251,254,255,255,255,255,255,255,255,255,255,255,255,255},
    {249,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,253,253,253,253,253,255,255,255,255,255,255,255,255},
    {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,254,254,254,254,255,255,255,255,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,251,251,254,254,254,255,255,255,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,250,250,250,250,255,255,255,255,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,252,252,255,255,255,255,255,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,254,254,254,254,255,255,255,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,253,253,253,253,255,255,255,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,246,246,253,253,255,255,255,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,252,254,253,255,255,255,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,252,254,253,255,248,255,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,251,253,249,255,248,255,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,5,251,249,253,248,252,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,251,249,253,248,252,255,255,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,249,251,248,252,253,253,255,255,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,248,248,248,248,248,248,248,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,248,249,249,249,251,251,255},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,251,251,252,252,250,252},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,253,0,0,3,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

    fill_values(expected_values, expected_gray_big);

    auto original_gray_big_view = gil::const_view(original_gray_big);
    auto processed_gray_big_view = gil::view(processed_gray_big);
    gil::erode(original_gray_big_view,processed_gray_big_view,struct_element_big);
    BOOST_TEST(gil::equal_pixels(gil::view(processed_gray_big), gil::view(expected_gray_big)));
}

int main()
{
    fill_values(original_gray_dilation_values, original_gray);
    test_dilation_3x3();
    test_dilation_2x2();
    test_dilation_3x3_boundary_case();

    fill_values(original_gray_erosion_values, original_gray);
    test_erosion_3x3();
    test_erosion_2x2();
    test_erosion_3x3_boundary_case();

    fill_values(big_image_values, original_gray_big);
    test_dilation_big();
    test_erosion_big();

    return boost::report_errors();
}
