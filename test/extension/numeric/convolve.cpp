//
// Copyright 2019-2020 Mateusz Loskot <mateusz at loskot dot net>
// Copyright 2021 Prathamesh Tagore <prathameshtagore@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include <boost/gil.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

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
        gil::detail::convolve_1d<pixel_t>(gil::const_view(img), kernel, gil::view(img_out));

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
        gil::detail::convolve_1d<pixel_t>(gil::const_view(img), kernel, gil::view(img_out));

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
        gil::detail::convolve_1d<pixel_t>(gil::const_view(img), kernel, gil::view(img_out));

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
        gil::detail::convolve_1d<pixel_t>(gil::const_view(img), kernel, gil::view(img_out));

        BOOST_TEST(gil::equal_pixels(gil::const_view(img), gil::const_view(img_out)));
    }
    static void run()
    {
        boost::mp11::mp_for_each<fixture::image_types>(test_image_5x5_kernel_3x3_identity{});
    }
};

// Convention used for naming variables : 
// 1. img : Original image which is to be used for testing.
// 2. img_out : Output image obtained after applying 1D row and column convolution using library
// function.
// 3. img_expected_row : Expected image after applying row convolution on "img".
// 4. img_expected_col : Expected image after applying column convolution on "img_expected_row".

struct test_image_5x5_kernel_1x9_boundary_extend_zero
{
    template <typename Image>
    void operator()(Image const&)
    {
        using image_t = Image;
        using pixel_t = typename image_t::value_type;
        using channel_t = typename gil::channel_type<pixel_t>::type;
        auto img = fixture::generate_image<image_t>(5, 5, fixture::random_value<channel_t>{});
        auto img_view = gil::view(img);
        image_t img_out(img), img_expected_row(img_view.width(), img_view.height());
        image_t img_expected_col(img_view.width(), img_view.height());
        unsigned int const kernel_shift_offset = 2;

        fixture::row_conv1D_offset_img_generator(img_view, gil::view(img_expected_row),
            kernel_shift_offset);
        fixture::col_conv1D_offset_img_generator(gil::view(img_expected_row),
            gil::view(img_expected_col), kernel_shift_offset);

        auto const kernel_shift_by_two = fixture::create_kernel<channel_t>(
            {0, 0, 0, 0, 0, 0, 1, 0, 0});
        gil::detail::convolve_1d<pixel_t>(gil::const_view(img), kernel_shift_by_two,
            gil::view(img_out), boost::gil::boundary_option::extend_zero);

        BOOST_TEST(gil::equal_pixels(gil::const_view(img_out), gil::const_view(img_expected_col)));
    }
    static void run()
    {
        boost::mp11::mp_for_each<fixture::image_types>(
            test_image_5x5_kernel_1x9_boundary_extend_zero{});
    }
};

struct test_image_5x5_kernel_1x9_boundary_extend_constant
{
    template <typename Image>
    void operator()(Image const&)
    {
        using image_t = Image;
        using pixel_t = typename image_t::value_type;
        using channel_t = typename gil::channel_type<pixel_t>::type;
        auto img = fixture::generate_image<image_t>(5, 5, fixture::random_value<channel_t>{});
        auto img_view = gil::view(img);
        image_t img_out(img), img_expected_row(img);
        unsigned int const kernel_shift_offset = 2;

        fixture::row_conv1D_offset_img_generator(img_view, gil::view(img_expected_row),
            kernel_shift_offset);
        fixture::row_conv1D_offset_img_generator(img_view, gil::view(img_expected_row),
            1, 0, 0, img_view.height(), 2);

        image_t img_expected_col(img_expected_row);

        fixture::col_conv1D_offset_img_generator(gil::view(img_expected_row),
            gil::view(img_expected_col), kernel_shift_offset);
        fixture::col_conv1D_offset_img_generator(gil::view(img_expected_row),
            gil::view(img_expected_col), 1, 0, 0, 2, img_view.width());

        auto const kernel_shift_by_two = fixture::create_kernel<channel_t>({0, 0, 0, 0, 0, 0, 1, 0, 0});
        gil::detail::convolve_1d<pixel_t>(gil::const_view(img), kernel_shift_by_two,
            gil::view(img_out), boost::gil::boundary_option::extend_constant);

        BOOST_TEST(gil::equal_pixels(gil::const_view(img_out), gil::const_view(img_expected_col)));
    }
    static void run()
    {
        boost::mp11::mp_for_each<fixture::image_types>(
            test_image_5x5_kernel_1x9_boundary_extend_constant{});
    }
};

struct test_image_5x5_kernel_1x3_boundary_output_zero
{
    template <typename Image>
    void operator()(Image const&)
    {
        using image_t = Image;
        using pixel_t = typename image_t::value_type;
        using channel_t = typename gil::channel_type<pixel_t>::type;
        auto img = fixture::generate_image<image_t>(5, 5, fixture::random_value<channel_t>{});
        auto img_view = gil::view(img);
        image_t img_out(img), img_expected_row(img_view.width(), img_view.height());
        image_t img_expected_col(img_view.width(), img_view.height());
        unsigned int const kernel_shift_offset = 1;

        fixture::row_conv1D_offset_img_generator(img_view, gil::view(img_expected_row),
            kernel_shift_offset, 0, 0, img_view.height(), img_view.width() - 1);
        fixture::col_conv1D_offset_img_generator(gil::view(img_expected_row),
            gil::view(img_expected_col), kernel_shift_offset, 0, 0, img_view.height() - 1,
            img_view.width());

        auto const kernel_shift_by_one = fixture::create_kernel<channel_t>({0, 0, 1});
        gil::detail::convolve_1d<pixel_t>(gil::const_view(img), kernel_shift_by_one,
            gil::view(img_out), boost::gil::boundary_option::output_zero);

        BOOST_TEST(gil::equal_pixels(gil::const_view(img_out), gil::const_view(img_expected_col)));
    }
    static void run()
    {
        boost::mp11::mp_for_each<fixture::image_types>(
            test_image_5x5_kernel_1x3_boundary_output_zero{});
    }
};

struct test_image_5x5_kernel_1x3_boundary_output_ignore
{
    template <typename Image>
    void operator()(Image const&)
    {
        using image_t = Image;
        using pixel_t = typename image_t::value_type;
        using channel_t = typename gil::channel_type<pixel_t>::type;
        auto img = fixture::generate_image<image_t>(5, 5, fixture::random_value<channel_t>{});
        auto img_view = gil::view(img);
        image_t img_out(img), img_expected_row(img);
        unsigned int const kernel_shift_offset = 1;

        fixture::row_conv1D_offset_img_generator(img_view, gil::view(img_expected_row),
            kernel_shift_offset, 0, 0, img_view.height(), img_view.width() - 1);

        image_t img_expected_col(img_expected_row);

        fixture::col_conv1D_offset_img_generator(gil::view(img_expected_row),
            gil::view(img_expected_col), kernel_shift_offset, 0, 0, img_view.height() - 1,
            img_view.width());
        
        auto const kernel_shift_by_one = fixture::create_kernel<channel_t>({0, 0, 1});
        gil::detail::convolve_1d<pixel_t>(gil::const_view(img), kernel_shift_by_one,
            gil::view(img_out), gil::boundary_option::output_ignore);

        BOOST_TEST(gil::equal_pixels(gil::const_view(img_out), gil::const_view(img_expected_col)));
    }
    static void run()
    {
        boost::mp11::mp_for_each<fixture::image_types>(
            test_image_5x5_kernel_1x3_boundary_output_ignore{});
    }
};

struct test_image_5x5_kernel_1x3_boundary_extend_padded
{
    template <typename Image>
    void operator()(Image const&)
    {
        using image_t = Image;
        using pixel_t = typename image_t::value_type;
        using channel_t = typename gil::channel_type<pixel_t>::type;
        auto img = fixture::generate_image<image_t>(5, 5, fixture::random_value<channel_t>{});
        auto img_view = gil::view(img);
        image_t img_out(img), img_expected_row(img);
        unsigned int const kernel_shift_offset = 1;

        fixture::row_conv1D_offset_img_generator(img_view, gil::view(img_expected_row),
            kernel_shift_offset);

        image_t img_expected_col(img_expected_row);

        fixture::col_conv1D_offset_img_generator(gil::view(img_expected_row),
            gil::view(img_expected_col), kernel_shift_offset);

        auto const kernel_shift_by_one = fixture::create_kernel<channel_t>({0, 0, 1});
        gil::detail::convolve_1d<pixel_t>(gil::const_view(img), kernel_shift_by_one,
            gil::view(img_out), gil::boundary_option::extend_padded);

        // First row and first column of "img_out" and "img_expected_col" are intentionally made
        // similar.
        auto img_out_it1 = gil::view(img_out).col_begin(0);
        auto img_expected_col_it1 = gil::view(img_expected_col).col_begin(0);
        for (std::ptrdiff_t x = 0; x < gil::view(img_out).width(); ++x)
            img_expected_col_it1[x] = img_out_it1[x];
        
        auto img_out_it2 = gil::view(img_out).row_begin(0);
        auto img_expected_col_it2 = gil::view(img_expected_col).row_begin(0);
        for (std::ptrdiff_t y = 0; y < gil::view(img_out).height(); ++y)
            img_expected_col_it2[y] = img_out_it2[y];
        
        BOOST_TEST(gil::equal_pixels(gil::const_view(img_out), gil::const_view(img_expected_col)));
        
    }
    static void run()
    {
        boost::mp11::mp_for_each<fixture::image_types>(
            test_image_5x5_kernel_1x3_boundary_extend_padded{});
    }
};

int main()
{
    test_image_1x1_kernel_1x1_identity::run();
    test_image_1x1_kernel_3x3_identity::run();
    test_image_3x3_kernel_3x3_identity::run();
    test_image_5x5_kernel_3x3_identity::run();

    test_image_5x5_kernel_1x9_boundary_extend_zero::run();
    test_image_5x5_kernel_1x9_boundary_extend_constant::run();
    test_image_5x5_kernel_1x3_boundary_output_zero::run();
    test_image_5x5_kernel_1x3_boundary_output_ignore::run();
    test_image_5x5_kernel_1x3_boundary_extend_padded::run();
    return ::boost::report_errors();
}
