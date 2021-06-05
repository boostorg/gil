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
        auto img_out = fixture::create_image<image_t>(1, 1, 0);

        using pixel_t = typename image_t::value_type;
        using channel_t = typename gil::channel_type<pixel_t>::type;
        auto const kernel = fixture::create_kernel<channel_t>({1});
        gil::convolve_rows<pixel_t>(const_view(img), kernel, view(img_out));

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
        auto const img = fixture::create_image<image_t>(1, 1, 7);
        auto img_out = fixture::create_image<image_t>(1, 1, 0);

        using pixel_t = typename image_t::value_type;
        using channel_t = typename gil::channel_type<pixel_t>::type;
        auto const kernel = fixture::create_kernel<channel_t>({0, 0, 0, 0, 1, 0, 0, 0, 0});
        gil::convolve_rows<pixel_t>(const_view(img), kernel, view(img_out));

        BOOST_TEST(gil::const_view(img).front() == gil::const_view(img_out).front());
    }
    static void run()
    {
        boost::mp11::mp_for_each<fixture::image_types>(test_image_1x1_kernel_3x3_identity{});
    }
};

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
        image_t img_out(img), img_expected(img_view.width(), img_view.height());
        unsigned int const kernel_shift_offset = 2;

        fixture::row_conv1D_offset_img_generator(img_view, gil::view(img_expected),
            kernel_shift_offset);

        auto const kernel_shift_by_two = fixture::create_kernel<channel_t>(
            {0, 0, 0, 0, 0, 0, 1, 0, 0});
        gil::convolve_rows<pixel_t>(gil::const_view(img), kernel_shift_by_two,
            gil::view(img_out), gil::boundary_option::extend_zero);
        BOOST_TEST(gil::equal_pixels(gil::const_view(img_out), gil::const_view(img_expected)));
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
        image_t img_out(img), img_expected(img);
        unsigned int const kernel_shift_offset = 2;

        fixture::row_conv1D_offset_img_generator(img_view, gil::view(img_expected),
            kernel_shift_offset);
        fixture::row_conv1D_offset_img_generator(img_view, gil::view(img_expected),
            1, 0, 0, img_view.height(), 2);

        auto const kernel_shift_by_two = fixture::create_kernel<channel_t>(
            {0, 0, 0, 0, 0, 0, 1, 0, 0});
        gil::convolve_rows<pixel_t>(gil::const_view(img), kernel_shift_by_two,
            gil::view(img_out), gil::boundary_option::extend_constant);

        BOOST_TEST(gil::equal_pixels(gil::const_view(img_out), gil::const_view(img_expected)));
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
        image_t img_out(img), img_expected(img_view.width(), img_view.height());
        unsigned int const kernel_shift_offset = 1;

        fixture::row_conv1D_offset_img_generator(img_view, gil::view(img_expected),
            kernel_shift_offset, 0, 0, img_view.height(), img_view.width() - 1);

        auto const kernel_shift_by_one = fixture::create_kernel<channel_t>({0, 0, 1});
        gil::convolve_rows<pixel_t>(gil::const_view(img), kernel_shift_by_one,
            gil::view(img_out), gil::boundary_option::output_zero);

        BOOST_TEST(gil::equal_pixels(gil::const_view(img_out), gil::const_view(img_expected)));
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
        image_t img_out(img), img_expected(img);
        unsigned int const kernel_shift_offset = 1;

        fixture::row_conv1D_offset_img_generator(img_view, gil::view(img_expected),
            kernel_shift_offset, 0, 0, img_view.height(), img_view.width() - 1);

        auto const kernel_shift_by_one = fixture::create_kernel<channel_t>({0, 0, 1});
        gil::convolve_rows<pixel_t>(gil::const_view(img), kernel_shift_by_one,
            gil::view(img_out), gil::boundary_option::output_ignore);

        BOOST_TEST(gil::equal_pixels(gil::const_view(img_out), gil::const_view(img_expected)));
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
        image_t img_out(img), img_expected(img);
        unsigned int const kernel_shift_offset = 1;

        fixture::row_conv1D_offset_img_generator(img_view, gil::view(img_expected),
            kernel_shift_offset);

        auto const kernel_shift_by_one = fixture::create_kernel<channel_t>({0, 0, 1});
        gil::convolve_rows<pixel_t>(gil::const_view(img), kernel_shift_by_one,
            gil::view(img_out), gil::boundary_option::extend_padded);
        
        // First row of "img_out" and "img_expected" is intentionally made similar.
        auto img_out_it = gil::view(img_out).col_begin(0);
        auto img_expected_it = gil::view(img_expected).col_begin(0);
        for (std::ptrdiff_t x = 0; x < gil::view(img_out).width(); ++x)
            img_expected_it[x] = img_out_it[x];
        
        BOOST_TEST(gil::equal_pixels(gil::const_view(img_out), gil::const_view(img_expected)));
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
    test_image_5x5_kernel_1x9_boundary_extend_zero::run();
    test_image_5x5_kernel_1x9_boundary_extend_constant::run();
    test_image_5x5_kernel_1x3_boundary_output_zero::run();
    test_image_5x5_kernel_1x3_boundary_output_ignore::run();
    test_image_5x5_kernel_1x3_boundary_extend_padded::run();

    return ::boost::report_errors();
}
