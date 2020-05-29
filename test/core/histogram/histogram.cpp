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
#include <boost/core/lightweight_test.hpp>

// Supported Container types for histogram
#include <vector>
#include <map>
#include <array>
#include <deque>
#include <unordered_map>

// Basic tests to make sure compatible container types produce 
// expected output histogram.

namespace gil = boost::gil;

gil::gray8_image_t img1(4, 4, gil::gray8_pixel_t(1));
gil::gray8_view_t v1 = view(img1);

gil::rgb8_image_t img2(4, 4, gil::rgb8_pixel_t(1));
gil::rgb8_view_t v2 = view(img2);

template<typename T>
bool check_equal(T &cont1, T &cont2, std::size_t size)
{
    bool ch = true;
    for(std::size_t i = 0; i < size; ++i)
    {
        ch = ch & (cont1[i] == cont2[i]);
    }
    return ch;
}

template<typename T>
bool check_equal(T &cont1, T &cont2)
{
    bool ch = true;
    for(auto &it : cont1)
    {
        ch = ch & (cont1[it.first] == cont2[it.first]);
    }
    return ch;
}

void check_vector_type_container()
{
    std::vector<int> c1,c1_expected(256,0);
    c1_expected[1] = 16;
    c1 = gil::image_histogram<std::vector<int>>(v1);
    BOOST_TEST(check_equal(c1, c1_expected, c1_expected.size()));

    c1_expected[1] = 32;
    gil::image_histogram(v1, c1);
    BOOST_TEST(check_equal(c1, c1_expected, c1_expected.size()));

    std::vector<int> c2,c2_expected;
    c2 = gil::image_histogram<std::vector<int>>(v2);
    c2_expected = gil::image_histogram<std::vector<int>>(gil::color_converted_view<gil::gray8_pixel_t>(v2));
    BOOST_TEST(check_equal(c2, c2_expected, c2_expected.size()));
}


void check_array_type_container()
{
    std::array<int, 256> c1{0},c1_expected{0};
    c1_expected[1] = 16;
    gil::image_histogram(v1, c1);
    BOOST_TEST(check_equal(c1, c1_expected, c1_expected.size()));

    std::array<int, 256> c2{0},c2_expected{0};
    gil::image_histogram(v1, c2);
    gil::image_histogram(gil::color_converted_view<gil::gray8_pixel_t>(v2), c2_expected);
    BOOST_TEST(check_equal(c2, c2_expected, c2_expected.size()));
}

void check_deque_type_container()
{
    std::deque<int> c1,c1_expected(256,0);
    c1_expected[1] = 16;
    c1 = gil::image_histogram<std::deque<int>>(v1);
    BOOST_TEST(check_equal(c1, c1_expected, c1_expected.size()));

    c1_expected[1] = 32;
    gil::image_histogram(v1, c1);
    BOOST_TEST(check_equal(c1, c1_expected, c1_expected.size()));

    std::deque<int> c2,c2_expected;
    c2 = gil::image_histogram<std::deque<int>>(v2);
    c2_expected = gil::image_histogram<std::deque<int>>(gil::color_converted_view<gil::gray8_pixel_t>(v2));
    BOOST_TEST(check_equal(c2, c2_expected, c2_expected.size()));
}

void check_map_type_container()
{
    std::map<int, int> c1,c1_expected;
    c1_expected[1] = 16;
    c1 = gil::image_histogram<std::map<int, int>>(v1);
    BOOST_TEST(check_equal(c1, c1_expected));

    c1_expected[1] = 32;
    gil::image_histogram(v1, c1);
    BOOST_TEST(check_equal(c1, c1_expected));

    std::map<int, int> c2,c2_expected;
    c2 = gil::image_histogram<std::map<int, int>>(v2);
    c2_expected = gil::image_histogram<std::map<int, int>>(gil::color_converted_view<gil::gray8_pixel_t>(v2));
    BOOST_TEST(check_equal(c2, c2_expected));
}

void check_unordered_map_type_container()
{
    std::unordered_map<int, int> c1,c1_expected;
    c1_expected[1] = 16;
    c1 = gil::image_histogram<std::unordered_map<int, int>>(v1);
    BOOST_TEST(check_equal(c1, c1_expected));

    c1_expected[1] = 32;
    gil::image_histogram(v1, c1);
    BOOST_TEST(check_equal(c1, c1_expected));

    std::unordered_map<int, int> c2,c2_expected;
    c2 = gil::image_histogram<std::unordered_map<int, int>>(v2);
    c2_expected = gil::image_histogram<std::unordered_map<int, int>>(gil::color_converted_view<gil::gray8_pixel_t>(v2));
    BOOST_TEST(check_equal(c2, c2_expected));
}


int main()
{
    check_vector_type_container();
    check_array_type_container();
    check_deque_type_container();
    check_map_type_container();
    check_unordered_map_type_container();

    return boost::report_errors();
}
