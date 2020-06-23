//
// Copyright 2020 Debabrata Mandal <mandaldebabrata123@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//

#include <boost/gil/histogram.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/pixel.hpp>

#include <boost/mp11.hpp>
#include <boost/core/lightweight_test.hpp>

#include <string>

namespace gil = boost::gil;
namespace mp11 = boost::mp11;

void check_helper_fn_pixel_to_tuple() 
{
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

void check_helper_fn_tuple_to_tuple() 
{
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

int main() {

    check_helper_fn_pixel_to_tuple();
    check_helper_fn_tuple_to_tuple();

    return boost::report_errors();
}   