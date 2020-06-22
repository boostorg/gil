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
#include <boost/gil/pixel.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/mp11.hpp>
#include <string>
#include <iostream>

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

void fill_gray_image(std::ptrdiff_t width, std::ptrdiff_t height, std::uint8_t* matrix, gil::gray8_view_t const& view) {
    for (std::ptrdiff_t i = 0; i < height; ++i)
    {
        for (std::ptrdiff_t j = 0; j < width; ++j)
        {
            view(i, j) = matrix[i * height + j];
        }
    }
}

void fill_rgb_image(std::ptrdiff_t width, std::ptrdiff_t height, std::uint8_t* matrix, gil::rgb8_view_t const& view) {
    for (std::ptrdiff_t i = 0; i < height; ++i)
    {
        for (std::ptrdiff_t j = 0; j < width; ++j)
        {
            for(std::size_t k = 0; k < 3; ++k)
                view(i, j)[k] = matrix[i * height + j] + k;
        }
    }
}


void check_helper_fn_pixel_to_tuple() {
    gil::gray8_pixel_t g1(2);
    auto g2 = gil::detail::pixel_to_tuple(g1, mp11::make_index_sequence<1>{});

    bool const same_gray_type = std::is_same<std::tuple<gil::uint8_t>, decltype(g2)>::value;
    BOOST_TEST(same_gray_type);
    BOOST_TEST(g1[0] == std::get<0>(g2));

    gil::rgb8_pixel_t r1(1,2,3);
    auto r2 = gil::detail::pixel_to_tuple(r1, mp11::index_sequence<0, 1, 2>{});

    bool const same_rgb_type = std::is_same<std::tuple<gil::uint8_t, gil::uint8_t, gil::uint8_t>,
                    decltype(r2)>::value;
    BOOST_TEST(same_rgb_type);
    BOOST_TEST(r1[0] == std::get<0>(r2) && r1[1] == std::get<1>(r2) && r1[2] == std::get<2>(r2));

    auto r3 = gil::detail::pixel_to_tuple(r1, mp11::index_sequence<1, 2, 0>{});
    BOOST_TEST(r1[0] == std::get<2>(r3) && r1[1] == std::get<0>(r3) && r1[2] == std::get<1>(r3));
}

void check_helper_fn_tuple_to_tuple() {
    std::tuple<int> t1(1);
    auto t2 = gil::detail::tuple_to_tuple(t1, mp11::make_index_sequence<1>{});

    bool const same_gray_type = std::is_same<std::tuple<int>, decltype(t2)>::value;
    BOOST_TEST(same_gray_type);
    BOOST_TEST(std::get<0>(t1) == std::get<0>(t2));

    std::tuple<int, int, std::string> r1(1, 2, "A");
    auto r2 = gil::detail::tuple_to_tuple(r1, mp11::index_sequence<0, 1, 2>{});

    bool const same_rgb_type = std::is_same<std::tuple<int, int, std::string>,
                    decltype(r2)>::value;
    BOOST_TEST(same_rgb_type);
    BOOST_TEST( std::get<0>(r1) == std::get<0>(r2) && 
                std::get<1>(r1) == std::get<1>(r2) && 
                std::get<2>(r1) == std::get<2>(r2));

    auto r3 = gil::detail::tuple_to_tuple(r1, mp11::index_sequence<1, 2, 0>{});
    BOOST_TEST( std::get<0>(r1) == std::get<2>(r3) && 
                std::get<1>(r1) == std::get<0>(r3) && 
                std::get<2>(r1) == std::get<1>(r3));
}


void check_histogram_constructors() {
    gil::histogram<int> h1;
    gil::histogram<int> h2 = h1;
    gil::histogram<int, int, int> h3;
    gil::histogram<int, int ,int> h4(h3);

    BOOST_TEST(h1.dimension() == h2.dimension());
    BOOST_TEST(h1.dimension() != h3.dimension());
    BOOST_TEST(h3.dimension() == h4.dimension());
}

void check_indexing_operator() {
    gil::histogram<int> h1;
    h1(1) = 3;
    BOOST_TEST(h1(1) == 3);
    BOOST_TEST(h1(3) == 0);

    gil::histogram<int, char, std::string> h2;
    h2(1, 'a', "A") = 4;
    BOOST_TEST(h2(1, 'a', "A") == 4);
    BOOST_TEST(h2(1, 'a', "B") == 0);
}

void check_is_pixel_compatible() {
    gil::histogram<int> h1;
    gil::histogram<int, double> h2;
    gil::histogram<int, unsigned char> h3;
    gil::histogram<int, std::string> h4;

    BOOST_TEST(h1.is_pixel_compatible());
    BOOST_TEST(h2.is_pixel_compatible());
    BOOST_TEST(h3.is_pixel_compatible());
    BOOST_TEST(!h4.is_pixel_compatible());
}

void check_is_tuple_compatible() {
    gil::histogram<int> h1;
    gil::histogram<double> h2;
    gil::histogram<int, double> h3;
    gil::histogram<int, unsigned char> h4;
    gil::histogram<int, std::string> h5;

    std::tuple<int> t1;
    std::tuple<float> t2;
    std::tuple<int, double> t3;
    std::tuple<int, unsigned char> t4;
    std::tuple<int, std::string> t5;

    BOOST_TEST(h1.is_tuple_compatible(t1));
    BOOST_TEST(h1.is_tuple_compatible(t2));
    BOOST_TEST(!h1.is_tuple_compatible(t3));
    BOOST_TEST(!h1.is_tuple_compatible(t5));

    BOOST_TEST(h2.is_tuple_compatible(t1));
    BOOST_TEST(h2.is_tuple_compatible(t2));
    BOOST_TEST(!h2.is_tuple_compatible(t3));

    BOOST_TEST(!h3.is_tuple_compatible(t1));
    BOOST_TEST(h3.is_tuple_compatible(t3));
    BOOST_TEST(h3.is_tuple_compatible(t4));
    BOOST_TEST(!h3.is_tuple_compatible(t5));

    BOOST_TEST(!h4.is_tuple_compatible(t1));
    BOOST_TEST(h4.is_tuple_compatible(t3));
    BOOST_TEST(h4.is_tuple_compatible(t4));
    BOOST_TEST(!h4.is_tuple_compatible(t5));
}

void check_histogram_key_from_tuple() {
    gil::histogram<int, int> h1;
    std::tuple<int, int> t1(1, 2);
    auto t2 = h1.key_from_tuple(t1);
    const bool same_type = std::is_same<std::tuple<int, int>, decltype(t2)>::value;
    
    BOOST_TEST(same_type);
    BOOST_TEST(std::get<0>(t2) == 1 && std::get<1>(t2) == 2);

    std::tuple<int, int, int, int> t3(1, 2, 4, 2);
    auto t4 = h1.key_from_tuple<0, 2>(t3);
    const bool same_type1 = std::is_same<std::tuple<int, int>, decltype(t4)>::value;
    
    BOOST_TEST(same_type1);
    BOOST_TEST(std::get<0>(t4) == 1 && std::get<1>(t4) == 4);
}

void check_histogram_key_from_pixel() {
    gil::histogram<int> h1;
    gil::gray8_pixel_t g1(1);
    auto t1 = h1.key_from_pixel(g1);
    const bool same_type = std::is_same<std::tuple<int>, decltype(t1)>::value;
    
    BOOST_TEST(same_type);
    BOOST_TEST(std::get<0>(t1) == 1);

    gil::histogram<int, int> h2;
    gil::rgb8_pixel_t r1(1, 0, 3);
    auto t2 = h2.key_from_pixel<0, 2>(r1);
    const bool same_type1 = std::is_same<std::tuple<int, int>, decltype(t2)>::value;
    
    BOOST_TEST(same_type1);
    BOOST_TEST(std::get<0>(t2) == 1 && std::get<1>(t2) == 3);
}

void check_histogram_fill() {
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

void check_histogram_fill_algorithm() {
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

void check_sub_histogram_without_tuple() {
    gil::histogram<int, int, std::string, int> h;
    h(1, 1, "A", 1) = 1;
    h(1, 2, "B", 1) = 1;
    h(2, 1, "C", 1) = 1;
    h(2, 1, "D", 1) = 1;
    h(2, 3, "E", 4) = 1;
    auto h1 = h.sub_histogram<0,3>();
    BOOST_TEST(h1(1, 1) == 2); 
    BOOST_TEST(h1(2, 1) == 2); 
    BOOST_TEST(h1(2, 4) == 1); 
    BOOST_TEST(h1(5, 5) == 0);
}

void check_sub_histogram_with_tuple() {
    gil::histogram<int, int, std::string, int> h;
    h(1, 1, "A", 1) = 3;
    h(1, 2, "B", 1) = 1;
    h(2, 1, "C", 1) = 1;
    h(2, 1, "A", 1) = 1;
    h(2, 3, "E", 4) = 1;
    h(1, 3, "A", 1) = 2;
    std::tuple<double, int, std::string, int> t(1.0, 1000, "A", 1);
    // This means 1st dimension is useless for matching.
    auto h1 = h.sub_histogram<0,2,3>(t);
    BOOST_TEST(h1(1, 1, "A", 1) == 3); 
    BOOST_TEST(h1(1, 2, "B", 1) == 0); 
    BOOST_TEST(h1(2, 1, "C", 1) == 0); 
    BOOST_TEST(h1(2, 1, "A", 1) == 0);
    BOOST_TEST(h1(2, 1, "A", 1) == 0);
    BOOST_TEST(h1(1, 3, "A", 1) == 2);
}


int main() {

    check_helper_fn_pixel_to_tuple();
    check_helper_fn_tuple_to_tuple();
    check_histogram_constructors();
    check_indexing_operator();
    check_is_pixel_compatible();
    check_is_tuple_compatible();
    check_histogram_key_from_tuple();
    check_histogram_key_from_pixel();
    check_histogram_fill();
    check_histogram_fill_algorithm();
    check_sub_histogram_without_tuple();
    check_sub_histogram_with_tuple();

    return boost::report_errors();
}   
