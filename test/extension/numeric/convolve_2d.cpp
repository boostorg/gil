//
// Copyright 2019 Miral Shah <miralshah2211@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/gil.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

#include <boost/core/lightweight_test.hpp>

#include <cstddef>

namespace gil = boost::gil;

std::uint8_t img1[] =
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

std::uint8_t output1[] =
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
        gil::interleaved_view(9, 9, reinterpret_cast<const gil::gray8_pixel_t*>(img1), 9);

    gil::image<gil::gray8_pixel_t> temp_img(src_view.width(), src_view.height());
    typename gil::image<gil::gray8_pixel_t>::view_t temp_view = view(temp_img);
    gil::gray8_view_t dst_view(temp_view);

    std::vector<float> v(9, 1.0f / 9.0f);
    gil::detail::kernel_2d<float> kernel(v.begin(), v.size(), 1, 1);

    gil::convolve_2d<gil::gray32f_pixel_t>(src_view, kernel, dst_view);

    gil::gray8c_view_t out_view =
        gil::interleaved_view(9, 9, reinterpret_cast<const gil::gray8_pixel_t*>(output1), 9);

    BOOST_TEST(gil::equal_pixels(out_view, dst_view));
}

template <typename GrayImageView>
void fill_gil_gray_image(GrayImageView& src_view)
{
    for (std::ptrdiff_t row = 0; row < src_view.height(); ++row)
        for (std::ptrdiff_t col = 0; col < src_view.width(); ++col)
            src_view(col, row) = (src_view.height() * row + col) % 10;
}

template <typename SrcView>
void print_gray(SrcView src_view)
{
    for (std::ptrdiff_t row = 0; row < src_view.height(); ++row)
    {
        for (std::ptrdiff_t col = 0; col < src_view.width(); ++col)
            std::cout << static_cast<float>(src_view(col, row)) << " ";
        std::cout << "\n";
    }
    std::cout << "\n\n";
}

template <typename RGBImageView>
void fill_gil_rgb_image(RGBImageView& src_view)
{
    for (std::ptrdiff_t row = 0; row < src_view.height(); ++row)
    {
        for (std::ptrdiff_t col = 0; col < src_view.width(); ++col)
        {
            src_view(col, row)[0] = (src_view.height() * row + col) % 10;
            src_view(col, row)[1] = (src_view.height() * row + col) % 10;
            src_view(col, row)[2] = (src_view.height() * row + col) % 10;
        }
    }
}

template <typename RGBImageView>
void print_gil_rgb_image(RGBImageView src_view)
{
    std::cout << "Gil Image = \n";
    for (std::ptrdiff_t row = 0; row < src_view.height(); ++row)
    {
        for (std::ptrdiff_t col = 0; col < src_view.width(); ++col)
        {
            std::cout << static_cast<float>(src_view(col, row)[0]) << " " << static_cast<float>(src_view(col, row)[1]) << 
                " " << static_cast<float>(src_view(col, row)[2]) << "   ";
        }
        std::cout << "\n";
    }
}

void basic_test()
{
    gil::rgb8_image_t src_img(15, 15), dst_img(5, 5);
    // std::vector<float> vec = {
    //     1, 0, 0, 0, 0, 
    //     0, 1, 0, 0, 0, 
    //     0, 0, 1, 0, 0, 
    //     0, 0, 0, 1, 0, 
    //     0, 0, 0, 0, 1};
    std::vector<float> vec(25, 1.0f);
    vec[12] = 0;

    gil::detail::kernel_2d<float> kernel(vec.begin(), vec.size(), 0, 0);

    fill_gil_rgb_image(gil::view(src_img));

    print_gil_rgb_image(gil::view(src_img));
    print_gil_rgb_image(gil::subimage_view(gil::view(src_img), 2, 2, 5, 5));
    gil::correlate_2d<gil::rgb8_pixel_t>(gil::subimage_view(gil::view(src_img), 2, 2, 5, 5), // Test separable convolution result printing
        kernel, gil::view(dst_img), gil::boundary_option::extend_padded);
    print_gil_rgb_image(gil::view(dst_img));
}

void test_7x7_image_5x5_kernel()
{
    gil::gray8_image_t img_in(7, 7);
    for (std::ptrdiff_t row = 0; row < 7; ++row)
        for (std::ptrdiff_t col = 0; col < 7; ++col)
            gil::view(img_in)(col, row) = 1;

    gil::gray8_image_t img_out(7, 7), img_out_correlate(7, 7);
    std::vector<float> v(9, 1.0f);
    v[4] = 0;
    gil::detail::kernel_2d<float> kernel(v.begin(), v.size(), 0, 0);

    gil::convolve_2d<gil::gray8_pixel_t>(gil::view(img_in), kernel, gil::view(img_out));
    gil::correlate_2d<gil::gray8_pixel_t>(gil::view(img_in), kernel, gil::view(img_out_correlate));

    print_gray(gil::view(img_in));
    print_gray(gil::view(img_out));
    print_gray(gil::view(img_out_correlate));
}

int main()
{
    // test_convolve_2d_with_normalized_mean_filter();
    basic_test();

    // test_7x7_image_5x5_kernel();

    return ::boost::report_errors();
}
