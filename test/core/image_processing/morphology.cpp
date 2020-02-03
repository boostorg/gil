#include <boost/gil/image_processing/morphology.hpp>
#include <boost/core/lightweight_test.hpp>

#include <vector>

namespace gil = boost::gil;

const int width = 5, height = 5;
const int struct_elem_height = 3, struct_elem_width = 3;

gil::gray8_image_t original_gray(width, height), processed_gray(width, height), expected_gray(width, height);

const int original_gray_dilation_values[width][height] = {
        {  0,   0,   0,   0,   0},
        {  0, 255, 255,   0,   0},
        {  0, 255,   0,   0,   0},
        {  0, 255,   0,   0,   0},
        {  0,   0,   0,   0,   0}};

const int original_gray_erosion_values[width][height] = {
        {  0,   0,   0,   0,   0},
        {  0, 255, 255, 255,   0},
        {  0, 255, 255, 255,   0},
        {  0, 255, 255, 255,   0},
        {  0,   0,   0,   0,   0}};

std::vector<std::vector<int> > struct_element{{0,1,0}, {1,1,1}, {0,1,0}};

void fill_values(const int values[width][height], gil::gray8_image_t &img)
{
    for(std::ptrdiff_t y=0; y<height; ++y)
    {
        for(std::ptrdiff_t x=0; x<width; ++x)
        {
            gil::view(img)(x,y) = gil::gray8_pixel_t(values[y][x]);
        }
    }
}

void test_dilation()
{
    int expected_values[width][height] = {
            { 0,   255, 255,   0,   0},
            { 255, 255, 255, 255,   0},
            { 255, 255, 255,   0,   0},
            { 255, 255, 255,   0,   0},
            { 0,   255,   0,   0,   0}};
    fill_values(expected_values, expected_gray);

    gil::dilate(gil::view(original_gray),gil::view(processed_gray),struct_element);
    BOOST_TEST(gil::equal_pixels(gil::view(processed_gray), gil::view(expected_gray)));
}

void test_erosion()
{
    int expected_values[width][height] = {0};
    expected_values[width/2][height/2] = 255;
    fill_values(expected_values, expected_gray);

    gil::erode(gil::view(original_gray),gil::view(processed_gray),struct_element);
    BOOST_TEST(gil::equal_pixels(gil::view(processed_gray), gil::view(expected_gray)));
}

int main()
{
    fill_values(original_gray_dilation_values, original_gray);
    test_dilation();

    fill_values(original_gray_erosion_values, original_gray);
    test_erosion();

    return boost::report_errors();
}
