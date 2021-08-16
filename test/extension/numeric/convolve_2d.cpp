//
// Copyright 2019 Miral Shah <miralshah2211@gmail.com>
// Copyright 2021 Prathamesh Tagore <prathameshtagore@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/gil.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <cstddef>

namespace gil = boost::gil;
std::uint8_t img[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 255, 0, 0, 0, 255, 0, 0,
    0, 0, 0, 255, 0, 255, 0, 0, 0,
    0, 0, 0, 0, 255, 0, 0, 0, 0,
    0, 0, 0, 255, 0, 255, 0, 0, 0,
    0, 0, 255, 0, 0, 0, 255, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0
};
std::uint8_t output[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 28, 28, 28, 0, 28, 28, 28, 0,
    0, 28, 56, 56, 56, 56, 56, 28, 0,
    0, 28, 56, 85, 85, 85, 56, 28, 0,
    0, 0, 56, 85, 141, 85, 56, 0, 0,
    0, 28, 56, 85, 85, 85, 56, 28, 0,
    0, 28, 56, 56, 56, 56, 56, 28, 0,
    0, 28, 28, 28, 0, 28, 28, 28, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

void test_convolve_2d_with_normalized_mean_filter()
{
    gil::gray8c_view_t src_view =
        gil::interleaved_view(9, 9, reinterpret_cast<const gil::gray8_pixel_t*>(img), 9);
    gil::image<gil::gray8_pixel_t> temp_img(src_view.width(), src_view.height());
    typename gil::image<gil::gray8_pixel_t>::view_t temp_view = view(temp_img);
    gil::gray8_view_t dst_view(temp_view);
    std::vector<float> v(9, 1.0f / 9.0f);
    gil::detail::kernel_2d<float> kernel(v.begin(), v.size(), 1, 1);

    gil::convolve_2d<gil::gray32f_pixel_t>(src_view, kernel, dst_view);

    gil::gray8c_view_t out_view =
        gil::interleaved_view(9, 9, reinterpret_cast<const gil::gray8_pixel_t*>(output), 9);
    BOOST_TEST(gil::equal_pixels(out_view, dst_view));
}

void test_boundary_option_extend_zero()
{
    gil::gray8_image_t src_gray(15, 15), dst_gray(15, 15), dst_gray_fixed(15, 15), exp_dst_gray(15, 15);
    gil::rgb8_image_t src_rgb(9, 9), dst_rgb(9, 9), dst_rgb_fixed(9, 9), exp_dst_rgb(9, 9);
    std::vector<float> vec(25, 1.0f);
    vec[12] = 0;

    gil::read_image("Test_Images/Extend_Zero/in_extend_zero_gray.png", src_gray, gil::png_tag{});
    gil::read_image("Test_Images/Extend_Zero/in_extend_zero_rgb.png", src_rgb, gil::png_tag{});

    for (std::ptrdiff_t x = 0; x < 5; ++x)
    {
        for (std::ptrdiff_t y = 0; y < 5; ++y)
        {
            gil::detail::kernel_2d<float> kernel(vec.begin(), vec.size(), y, x);
            gil::detail::kernel_2d_fixed<float, 5> kernel_fixed(vec.cbegin(), y, x);

            gil::correlate_2d<gil::gray8_pixel_t>(gil::view(src_gray), kernel, gil::view(dst_gray), 
                gil::boundary_option::extend_zero);
            gil::correlate_2d_fixed<gil::gray8_pixel_t>(gil::view(src_gray), kernel_fixed, 
                gil::view(dst_gray_fixed), gil::boundary_option::extend_zero);

            gil::correlate_2d<gil::rgb8_pixel_t>(gil::view(src_rgb), kernel, gil::view(dst_rgb), 
                gil::boundary_option::extend_zero);
            gil::correlate_2d_fixed<gil::rgb8_pixel_t>(gil::view(src_rgb), kernel_fixed, 
                gil::view(dst_rgb_fixed), gil::boundary_option::extend_zero);

            std::string exp_gray = "Test_Images/Extend_Zero/exp_out_extend_zero_gray_";
            std::string exp_rgb = "Test_Images/Extend_Zero/exp_out_extend_zero_rgb_";

            gil::read_image(exp_gray + static_cast<char>('0' + x) + '_' + static_cast<char>('0' + y) 
                + ".png", exp_dst_gray, gil::png_tag{});
            gil::read_image(exp_rgb + static_cast<char>('0' + x) + '_' + static_cast<char>('0' + y) 
                + ".png", exp_dst_rgb, gil::png_tag{});

            BOOST_TEST(gil::equal_pixels(gil::view(dst_gray), gil::view(exp_dst_gray)));
            BOOST_TEST(gil::equal_pixels(gil::view(dst_gray_fixed), gil::view(exp_dst_gray)));
            BOOST_TEST(gil::equal_pixels(gil::view(dst_rgb), gil::view(exp_dst_rgb)));
            BOOST_TEST(gil::equal_pixels(gil::view(dst_rgb_fixed), gil::view(exp_dst_rgb)));
        }
    }
}

void test_boundary_option_extend_constant()
{
    gil::gray8_image_t src_gray(15, 15), dst_gray(15, 15), dst_gray_fixed(15, 15), exp_dst_gray(15, 15);
    gil::rgb8_image_t src_rgb(9, 9), dst_rgb(9, 9), dst_rgb_fixed(9, 9), exp_dst_rgb(9, 9);
    std::vector<float> vec(25, 1.0f);
    vec[12] = 0;

    gil::read_image("Test_Images/Extend_Constant/in_extend_constant_gray.png", src_gray, gil::png_tag{});
    gil::read_image("Test_Images/Extend_Constant/in_extend_constant_rgb.png", src_rgb, gil::png_tag{});

    for (std::ptrdiff_t x = 0; x < 5; ++x)
    {
        for (std::ptrdiff_t y = 0; y < 5; ++y)
        {
            gil::detail::kernel_2d<float> kernel(vec.begin(), vec.size(), y, x);
            gil::detail::kernel_2d_fixed<float, 5> kernel_fixed(vec.cbegin(), y, x);

            gil::correlate_2d<gil::gray8_pixel_t>(gil::view(src_gray), kernel, gil::view(dst_gray), 
                gil::boundary_option::extend_constant);
            gil::correlate_2d_fixed<gil::gray8_pixel_t>(gil::view(src_gray), kernel_fixed, 
                gil::view(dst_gray_fixed), gil::boundary_option::extend_constant);

            gil::correlate_2d<gil::rgb8_pixel_t>(gil::view(src_rgb), kernel, gil::view(dst_rgb), 
                gil::boundary_option::extend_constant);
            gil::correlate_2d_fixed<gil::rgb8_pixel_t>(gil::view(src_rgb), kernel_fixed, 
                gil::view(dst_rgb_fixed), gil::boundary_option::extend_constant);

            std::string exp_gray = "Test_Images/Extend_Constant/exp_out_extend_constant_gray_";
            std::string exp_rgb = "Test_Images/Extend_Constant/exp_out_extend_constant_rgb_";

            gil::read_image(exp_gray + static_cast<char>('0' + x) + '_' + static_cast<char>('0' + y) 
                + ".png", exp_dst_gray, gil::png_tag{});
            gil::read_image(exp_rgb + static_cast<char>('0' + x) + '_' + static_cast<char>('0' + y) 
                + ".png", exp_dst_rgb, gil::png_tag{});

            BOOST_TEST(gil::equal_pixels(gil::view(dst_gray), gil::view(exp_dst_gray)));
            BOOST_TEST(gil::equal_pixels(gil::view(dst_gray_fixed), gil::view(exp_dst_gray)));
            BOOST_TEST(gil::equal_pixels(gil::view(dst_rgb), gil::view(exp_dst_rgb)));
            BOOST_TEST(gil::equal_pixels(gil::view(dst_rgb_fixed), gil::view(exp_dst_rgb)));
        }
    }
}

void test_boundary_option_extend_reflection()
{
    gil::gray8_image_t src_gray(15, 15), dst_gray(15, 15), dst_gray_fixed(15, 15), exp_dst_gray(15, 15);
    gil::rgb8_image_t src_rgb(9, 9), dst_rgb(9, 9), dst_rgb_fixed(9, 9), exp_dst_rgb(9, 9);
    std::vector<float> vec(25, 1.0f);
    vec[12] = 0;

    gil::read_image("Test_Images/Extend_Reflection/in_extend_reflection_gray.png", src_gray, 
        gil::png_tag{});
    gil::read_image("Test_Images/Extend_Reflection/in_extend_reflection_rgb.png", src_rgb, 
        gil::png_tag{});

    for (std::ptrdiff_t x = 0; x < 5; ++x)
    {
        for (std::ptrdiff_t y = 0; y < 5; ++y)
        {
            gil::detail::kernel_2d<float> kernel(vec.begin(), vec.size(), y, x);
            gil::detail::kernel_2d_fixed<float, 5> kernel_fixed(vec.cbegin(), y, x);

            gil::correlate_2d<gil::gray8_pixel_t>(gil::view(src_gray), kernel, gil::view(dst_gray), 
                gil::boundary_option::extend_reflection);
            gil::correlate_2d_fixed<gil::gray8_pixel_t>(gil::view(src_gray), kernel_fixed, 
                gil::view(dst_gray_fixed), gil::boundary_option::extend_reflection);

            gil::correlate_2d<gil::rgb8_pixel_t>(gil::view(src_rgb), kernel, gil::view(dst_rgb), 
                gil::boundary_option::extend_reflection);
            gil::correlate_2d_fixed<gil::rgb8_pixel_t>(gil::view(src_rgb), kernel_fixed, 
                gil::view(dst_rgb_fixed), gil::boundary_option::extend_reflection);

            std::string exp_gray = "Test_Images/Extend_Reflection/exp_out_extend_reflection_gray_";
            std::string exp_rgb = "Test_Images/Extend_Reflection/exp_out_extend_reflection_rgb_";

            gil::read_image(exp_gray + static_cast<char>('0' + x) + '_' + static_cast<char>('0' + y) 
                + ".png", exp_dst_gray, gil::png_tag{});
            gil::read_image(exp_rgb + static_cast<char>('0' + x) + '_' + static_cast<char>('0' + y) 
                + ".png", exp_dst_rgb, gil::png_tag{});

            BOOST_TEST(gil::equal_pixels(gil::view(dst_gray), gil::view(exp_dst_gray)));
            BOOST_TEST(gil::equal_pixels(gil::view(dst_gray_fixed), gil::view(exp_dst_gray)));
            BOOST_TEST(gil::equal_pixels(gil::view(dst_rgb), gil::view(exp_dst_rgb)));
            BOOST_TEST(gil::equal_pixels(gil::view(dst_rgb_fixed), gil::view(exp_dst_rgb)));
        }
    }
}

void test_boundary_option_extend_padded()
{
    gil::gray8_image_t src_gray(15, 15), dst_gray(5, 5), dst_gray_fixed(5, 5), exp_dst_gray(5, 5);
    gil::rgb8_image_t src_rgb(9, 9), dst_rgb(5, 5), dst_rgb_fixed(5, 5), exp_dst_rgb(5, 5);
    std::vector<float> vec(9, 1.0f);
    vec[4] = 0;

    gil::read_image("Test_Images/Extend_Padded/in_extend_padded_gray.png", src_gray, 
        gil::png_tag{});
    gil::read_image("Test_Images/Extend_Padded/in_extend_padded_rgb.png", src_rgb, 
        gil::png_tag{});

    for (std::ptrdiff_t x = 0; x < 3; ++x)
    {
        for (std::ptrdiff_t y = 0; y < 3; ++y)
        {
            gil::detail::kernel_2d<float> kernel(vec.begin(), vec.size(), y, x);
            gil::detail::kernel_2d_fixed<float, 3> kernel_fixed(vec.cbegin(), y, x);

            gil::correlate_2d<gil::gray8_pixel_t>(
                gil::subimage_view(gil::view(src_gray), 5, 5, 5, 5), kernel, 
                gil::view(dst_gray), gil::boundary_option::extend_padded);
            gil::correlate_2d_fixed<gil::gray8_pixel_t>(
                gil::subimage_view(gil::view(src_gray), 5, 5, 5, 5), kernel_fixed, 
                gil::view(dst_gray_fixed), gil::boundary_option::extend_padded);

            gil::correlate_2d<gil::rgb8_pixel_t>(
                gil::subimage_view(gil::view(src_rgb), 2, 2, 5, 5), kernel, 
                gil::view(dst_rgb), gil::boundary_option::extend_padded);
            gil::correlate_2d_fixed<gil::rgb8_pixel_t>(
                gil::subimage_view(gil::view(src_rgb), 2, 2, 5, 5), kernel_fixed, 
                gil::view(dst_rgb_fixed), gil::boundary_option::extend_padded);

            std::string exp_gray = "Test_Images/Extend_Padded/exp_out_extend_padded_gray_";
            std::string exp_rgb = "Test_Images/Extend_Padded/exp_out_extend_padded_rgb_";

            gil::read_image(exp_gray + static_cast<char>('0' + x) + '_' + static_cast<char>('0' + y) 
                + ".png", exp_dst_gray, gil::png_tag{});
            gil::read_image(exp_rgb + static_cast<char>('0' + x) + '_' + static_cast<char>('0' + y) 
                + ".png", exp_dst_rgb, gil::png_tag{});

            BOOST_TEST(gil::equal_pixels(gil::view(dst_gray), gil::view(exp_dst_gray)));
            BOOST_TEST(gil::equal_pixels(gil::view(dst_gray_fixed), gil::view(exp_dst_gray)));
            BOOST_TEST(gil::equal_pixels(gil::view(dst_rgb), gil::view(exp_dst_rgb)));
            BOOST_TEST(gil::equal_pixels(gil::view(dst_rgb_fixed), gil::view(exp_dst_rgb)));
        }
    }
}

void test_boundary_option_output_zero()
{
    gil::gray8_image_t src_gray(15, 15), dst_gray(15, 15), dst_gray_fixed(15, 15), exp_dst_gray(15, 15);
    gil::rgb8_image_t src_rgb(9, 9), dst_rgb(9, 9), dst_rgb_fixed(9, 9), exp_dst_rgb(9, 9);
    std::vector<float> vec(25, 1.0f);
    vec[12] = 0;

    gil::read_image("Test_Images/Output_Zero/in_output_zero_gray.png", src_gray, 
        gil::png_tag{});
    gil::read_image("Test_Images/Output_Zero/in_output_zero_rgb.png", src_rgb, 
        gil::png_tag{});

    for (std::ptrdiff_t x = 0; x < 5; ++x)
    {
        for (std::ptrdiff_t y = 0; y < 5; ++y)
        {
            gil::detail::kernel_2d<float> kernel(vec.begin(), vec.size(), y, x);
            gil::detail::kernel_2d_fixed<float, 5> kernel_fixed(vec.cbegin(), y, x);

            gil::correlate_2d<gil::gray8_pixel_t>(gil::view(src_gray), kernel, gil::view(dst_gray), 
                gil::boundary_option::output_zero);
            gil::correlate_2d_fixed<gil::gray8_pixel_t>(gil::view(src_gray), kernel_fixed, 
                gil::view(dst_gray_fixed), gil::boundary_option::output_zero);

            gil::correlate_2d<gil::rgb8_pixel_t>(gil::view(src_rgb), kernel, gil::view(dst_rgb), 
                gil::boundary_option::output_zero);
            gil::correlate_2d_fixed<gil::rgb8_pixel_t>(gil::view(src_rgb), kernel_fixed, 
                gil::view(dst_rgb_fixed), gil::boundary_option::output_zero);

            std::string exp_gray = "Test_Images/Output_Zero/exp_out_output_zero_gray_";
            std::string exp_rgb = "Test_Images/Output_Zero/exp_out_output_zero_rgb_";

            gil::read_image(exp_gray + static_cast<char>('0' + x) + '_' + static_cast<char>('0' + y) 
                + ".png", exp_dst_gray, gil::png_tag{});
            gil::read_image(exp_rgb + static_cast<char>('0' + x) + '_' + static_cast<char>('0' + y) 
                + ".png", exp_dst_rgb, gil::png_tag{});

            BOOST_TEST(gil::equal_pixels(gil::view(dst_gray), gil::view(exp_dst_gray)));
            BOOST_TEST(gil::equal_pixels(gil::view(dst_gray_fixed), gil::view(exp_dst_gray)));
            BOOST_TEST(gil::equal_pixels(gil::view(dst_rgb), gil::view(exp_dst_rgb)));
            BOOST_TEST(gil::equal_pixels(gil::view(dst_rgb_fixed), gil::view(exp_dst_rgb)));
        }
    }
}

void test_boundary_option_output_ignore()
{
    gil::gray8_image_t src_gray(15, 15), dst_gray(15, 15), dst_gray_fixed(15, 15), exp_dst_gray(15, 15);
    gil::rgb8_image_t src_rgb(9, 9), dst_rgb(9, 9), dst_rgb_fixed(9, 9), exp_dst_rgb(9, 9);
    std::vector<float> vec(25, 1.0f);
    vec[12] = 0;

    gil::read_image("Test_Images/Output_Ignore/in_output_ignore_gray.png", src_gray, 
        gil::png_tag{});
    gil::read_image("Test_Images/Output_Ignore/in_output_ignore_rgb.png", src_rgb, 
        gil::png_tag{});

    for (std::ptrdiff_t x = 0; x < 5; ++x)
    {
        for (std::ptrdiff_t y = 0; y < 5; ++y)
        {
            gil::detail::kernel_2d<float> kernel(vec.begin(), vec.size(), y, x);
            gil::detail::kernel_2d_fixed<float, 5> kernel_fixed(vec.cbegin(), y, x);

            gil::correlate_2d<gil::gray8_pixel_t>(gil::view(src_gray), kernel, gil::view(dst_gray), 
                gil::boundary_option::output_ignore);
            gil::correlate_2d_fixed<gil::gray8_pixel_t>(gil::view(src_gray), kernel_fixed, 
                gil::view(dst_gray_fixed), gil::boundary_option::output_ignore);

            gil::correlate_2d<gil::rgb8_pixel_t>(gil::view(src_rgb), kernel, gil::view(dst_rgb), 
                gil::boundary_option::output_ignore);
            gil::correlate_2d_fixed<gil::rgb8_pixel_t>(gil::view(src_rgb), kernel_fixed, 
                gil::view(dst_rgb_fixed), gil::boundary_option::output_ignore);

            std::string exp_gray = "Test_Images/Output_Ignore/exp_out_output_ignore_gray_";
            std::string exp_rgb = "Test_Images/Output_Ignore/exp_out_output_ignore_rgb_";

            gil::read_image(exp_gray + static_cast<char>('0' + x) + '_' + static_cast<char>('0' + y) 
                + ".png", exp_dst_gray, gil::png_tag{});
            gil::read_image(exp_rgb + static_cast<char>('0' + x) + '_' + static_cast<char>('0' + y) 
                + ".png", exp_dst_rgb, gil::png_tag{});

            BOOST_TEST(gil::equal_pixels(gil::view(dst_gray), gil::view(exp_dst_gray)));
            BOOST_TEST(gil::equal_pixels(gil::view(dst_gray_fixed), gil::view(exp_dst_gray)));
            BOOST_TEST(gil::equal_pixels(gil::view(dst_rgb), gil::view(exp_dst_rgb)));
            BOOST_TEST(gil::equal_pixels(gil::view(dst_rgb_fixed), gil::view(exp_dst_rgb)));
        }
    }
}

void test_separable_correlation()
{
    gil::gray8_image_t src_gray(15, 15), dst_gray(15, 15), dst_gray_fixed(15, 15), exp_dst_gray(15, 15);
    std::vector<float> vec(25, 1.0f);

    gil::read_image("Test_Images/Separable_Correlation/in_separable_correlation_gray.png", src_gray, 
        gil::png_tag{});

    for (std::ptrdiff_t x = 0; x < 5; ++x)
    {
        for (std::ptrdiff_t y = 0; y < 5; ++y)
        {
            gil::detail::kernel_2d<float> kernel(vec.begin(), vec.size(), y, x);
            gil::detail::kernel_2d_fixed<float, 5> kernel_fixed(vec.cbegin(), y, x);

            gil::correlate_2d<gil::gray8_pixel_t>(gil::view(src_gray), kernel, gil::view(dst_gray));
            gil::correlate_2d_fixed<gil::gray8_pixel_t>(gil::view(src_gray), kernel_fixed, 
                gil::view(dst_gray_fixed));

            std::string exp_gray = 
                "Test_Images/Separable_Correlation/exp_out_separable_correlation_gray_";

            gil::read_image(exp_gray + static_cast<char>('0' + x) + '_' + static_cast<char>('0' + y) 
                + ".png", exp_dst_gray, gil::png_tag{});

            BOOST_TEST(gil::equal_pixels(gil::view(dst_gray), gil::view(exp_dst_gray)));
            BOOST_TEST(gil::equal_pixels(gil::view(dst_gray_fixed), gil::view(exp_dst_gray)));
        }
    }
}


int main()
{
    test_convolve_2d_with_normalized_mean_filter();
    test_boundary_option_extend_zero();
    test_boundary_option_extend_constant();
    test_boundary_option_extend_reflection();
    test_boundary_option_extend_padded();
    test_boundary_option_output_zero();
    test_boundary_option_output_ignore();
    test_separable_correlation();

    return ::boost::report_errors();
}
