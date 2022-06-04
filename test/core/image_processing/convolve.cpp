//
// Copyright 2019-2020 Mateusz Loskot <mateusz at loskot dot net>
// Copyright 2021 Pranam Lashkari <plashkari628@gmail.com>
// Copyright 2021 Prathamesh Tagore <prathameshtagore@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include <boost/gil.hpp>
#include <boost/gil/image_processing/convolve.hpp>

#include <boost/core/lightweight_test.hpp>

#include <tuple>
#include <type_traits>

#include "test_fixture.hpp"
#include "core/test_fixture.hpp"
#include "core/image/test_fixture.hpp"

namespace gil = boost::gil;
namespace fixture = boost::gil::test::fixture;

struct test_image_1x1_kernel_1x1_identity
{
    template <typename Image>
    void operator()(Image const&)
    {
        using image_t = Image;
        auto const img = fixture::create_image<image_t>(1, 1, 7);
        image_t img_out(img);

        using pixel_t = typename image_t::value_type;
        using channel_t = typename gil::channel_type<pixel_t>::type;
        auto const kernel = fixture::create_kernel<channel_t>({1});
        gil::detail::convolve_1d<pixel_t>(gil::const_view(img_out), kernel, gil::view(img_out));

        // 1x1 kernel reduces convolution to multiplication
        BOOST_TEST(gil::const_view(img).front() == gil::const_view(img_out).front());
    }
    static void run()
    {
        boost::mp11::mp_for_each<fixture::image_types>(test_image_1x1_kernel_1x1_identity{});
    }
};

struct test_image_1x1_kernel_3x3_identity
{
    template <typename Image>
    void operator()(Image const&)
    {
        using image_t = Image;
        auto const img = fixture::create_image<Image>(1, 1, 7);
        image_t img_out(img);

        using pixel_t = typename image_t::value_type;
        using channel_t = typename gil::channel_type<pixel_t>::type;
        auto const kernel = fixture::create_kernel<channel_t>({0, 0, 0, 0, 1, 0, 0, 0, 0});
        gil::detail::convolve_1d<pixel_t>(gil::const_view(img_out), kernel, gil::view(img_out));

        BOOST_TEST(gil::const_view(img).front() == gil::const_view(img_out).front());
    }
    static void run()
    {
        boost::mp11::mp_for_each<fixture::image_types>(test_image_1x1_kernel_3x3_identity{});
    }
};

struct test_image_3x3_kernel_3x3_identity
{
    template <typename Image>
    void operator()(Image const&)
    {
        using image_t = Image;
        using pixel_t = typename image_t::value_type;
        using channel_t = typename gil::channel_type<pixel_t>::type;
        auto const img = fixture::generate_image<image_t>(3, 3, fixture::random_value<channel_t>{});
        image_t img_out(img);

        auto const kernel = fixture::create_kernel<channel_t>({0, 0, 0, 0, 1, 0, 0, 0, 0});
        gil::detail::convolve_1d<pixel_t>(gil::const_view(img_out), kernel, gil::view(img_out));

        BOOST_TEST(gil::equal_pixels(gil::const_view(img), gil::const_view(img_out)));
    }
    static void run()
    {
        boost::mp11::mp_for_each<fixture::image_types>(test_image_3x3_kernel_3x3_identity{});
    }
};

struct test_image_5x5_kernel_3x3_identity
{
    template <typename Image>
    void operator()(Image const&)
    {
        using image_t = Image;
        using pixel_t = typename image_t::value_type;
        using channel_t = typename gil::channel_type<pixel_t>::type;
        auto const img = fixture::generate_image<image_t>(5, 5, fixture::random_value<channel_t>{});
        image_t img_out(img);

        auto const kernel = fixture::create_kernel<channel_t>({0, 0, 0, 0, 1, 0, 0, 0, 0});
        gil::detail::convolve_1d<pixel_t>(gil::const_view(img_out), kernel, gil::view(img_out));
        // TODO: Test different boundary options

        BOOST_TEST(gil::equal_pixels(gil::const_view(img), gil::const_view(img_out)));
    }
    static void run()
    {
        boost::mp11::mp_for_each<fixture::image_types>(test_image_5x5_kernel_3x3_identity{});
    }
};

struct test_image_5x5_kernel_1x9_boundary_extend_reflection
{
    template <typename Image>
    void operator()(Image const&)
    {
        using image_t = Image;
        using pixel_t = typename image_t::value_type;
        using channel_t = typename gil::channel_type<pixel_t>::type;
        auto img = fixture::generate_image<image_t>(5, 5, fixture::random_value<channel_t>{});
        auto img_view = gil::view(img);
        image_t img_out_up_left_offset(img), img_expected_row_up_offset(img);
        image_t img_expected_col_left_offset(img), img_out_down_right_offset(img);
        image_t img_expected_row_down_offset(img), img_expected_col_right_offset(img);
        int kernel_shift_up_left_offset = 2, kernel_shift_down_right_offset = -2;

        fixture::row_conv1D_offset_img_generator(img_view, gil::view(img_expected_row_up_offset),
            kernel_shift_up_left_offset);
        fixture::row_conv1D_offset_img_generator(img_view, gil::view(img_expected_row_up_offset),
            -1, 0, 1, img_view.height(), 1);
        fixture::row_conv1D_offset_img_generator(img_view, gil::view(img_expected_row_up_offset),
            1, 0, 0, img_view.height(), 2);

        fixture::col_conv1D_offset_img_generator(gil::view(img_expected_row_up_offset),
            gil::view(img_expected_col_left_offset), kernel_shift_up_left_offset);
        fixture::col_conv1D_offset_img_generator(gil::view(img_expected_row_up_offset),
            gil::view(img_expected_col_left_offset), -1, 1, 0, 1, img_view.width());
        fixture::col_conv1D_offset_img_generator(gil::view(img_expected_row_up_offset),
            gil::view(img_expected_col_left_offset), 1, 0, 0, 2, img_view.width());

        fixture::row_conv1D_offset_img_generator(img_view, gil::view(img_expected_row_down_offset),
            kernel_shift_down_right_offset, 0, 2, img_view.height(), img_view.width());
        fixture::row_conv1D_offset_img_generator(img_view, gil::view(img_expected_row_down_offset),
            -1, 0, img_view.width() - 1, img_view.height(), img_view.width());
        fixture::row_conv1D_offset_img_generator(img_view, gil::view(img_expected_row_down_offset),
            1, 0, img_view.width() - 2, img_view.height(), img_view.width());

        fixture::col_conv1D_offset_img_generator(gil::view(img_expected_row_down_offset),
            gil::view(img_expected_col_right_offset), kernel_shift_down_right_offset, 2, 0,
            img_view.height(), img_view.width());
        fixture::col_conv1D_offset_img_generator(gil::view(img_expected_row_down_offset),
            gil::view(img_expected_col_right_offset), -1, img_view.height() - 1, 0,
            img_view.height(), img_view.width());
        fixture::col_conv1D_offset_img_generator(gil::view(img_expected_row_down_offset),
            gil::view(img_expected_col_right_offset), 1, img_view.height() - 2, 0,
            img_view.height(), img_view.width());

        auto const kernel_up_left_offset = fixture::create_kernel<channel_t>(
            {0, 0, 0, 0, 0, 0, 1, 0, 0});
        gil::detail::convolve_1d<pixel_t>(gil::const_view(img), kernel_up_left_offset,
            gil::view(img_out_up_left_offset), gil::boundary_option::extend_reflection);

        auto const kernel_down_right_offset = fixture::create_kernel<channel_t>(
            {0, 0, 1, 0, 0, 0, 0, 0, 0});
        gil::detail::convolve_1d<pixel_t>(gil::const_view(img), kernel_down_right_offset, 
            gil::view(img_out_down_right_offset), gil::boundary_option::extend_reflection);

        BOOST_TEST(gil::equal_pixels(gil::const_view(img_out_up_left_offset), 
            gil::const_view(img_expected_col_left_offset)));
        BOOST_TEST(gil::equal_pixels(gil::const_view(img_out_down_right_offset), 
            gil::const_view(img_expected_col_right_offset)));
    }
    static void run()
    {
        boost::mp11::mp_for_each<fixture::image_types>(
            test_image_5x5_kernel_1x9_boundary_extend_reflection{});
    }
};

int main()
{
    test_image_1x1_kernel_1x1_identity::run();
    test_image_1x1_kernel_3x3_identity::run();
    test_image_3x3_kernel_3x3_identity::run();
    test_image_5x5_kernel_3x3_identity::run();

    test_image_5x5_kernel_1x9_boundary_extend_reflection::run();
    return ::boost::report_errors();
}
