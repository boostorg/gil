#include <bits/stdc++.h>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

#include <boost/gil/extension/numeric/convolve.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/gil.hpp>

namespace gil = boost::gil;

void fill_opencv_gray_image(cv::Mat& src)
{
    uint8_t i = 0;
    while (i <= src.rows * src.cols)
        src.at<cv::Vec2b>(0, 0)[i] = i % 10, ++i;
}

template <typename GrayImageView>
void fill_gil_gray_image(GrayImageView& src_view)
{
    for (std::ptrdiff_t row = 0; row < src_view.height(); ++row)
        for (std::ptrdiff_t col = 0; col < src_view.width(); ++col)
            src_view(col, row) = (src_view.height() * row + col) % 10;
}

void fill_opencv_rgb_image(cv::Mat& src)
{
    uint8_t i = 0, j = 0;
    while (i <= src.rows * src.cols)
    {
        src.at<cv::Vec2b>(0, 0)[j] = i % 10;
        src.at<cv::Vec2b>(0, 0)[j + 1] = i % 10;
        src.at<cv::Vec2b>(0, 0)[j + 2] = i % 10;
        ++i, j += 3;
    }
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

template <typename GrayImageView>
void print_gil_gray_image(GrayImageView src_view)
{
    std::cout << "Gil Image = \n";
    for (std::ptrdiff_t row = 0; row < src_view.height(); ++row)
    {
        for (std::ptrdiff_t col = 0; col < src_view.width(); ++col)
            std::cout << static_cast<float>(static_cast<gil::gray8_pixel_t>(src_view(col, row))) << " ";
        std::cout << "\n";
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

template <typename GrayImageView>
bool test_gray_images(cv::Mat& dst_o_gray, GrayImageView& dst_g_gray)
{
    for (std::ptrdiff_t row = 0; row < dst_g_gray.height(); ++row)
    {
        for (std::ptrdiff_t col = 0; col < dst_g_gray.width(); ++col)
        {
            if (static_cast<int>(static_cast<gil::gray8_pixel_t>(dst_g_gray(col, row))) != 
                dst_o_gray.at<cv::Vec2b>(0, 0)[dst_g_gray.height() * row + col])
            {
                std::cout << "row col  " << row << "  " << col << "\n";
                return 0;
            }
        }
    }
    return 1;
}

template <typename RGBImageView>
bool test_rgb_images(cv::Mat& dst_o_rgb, RGBImageView& dst_g_rgb)
{
    int j = 0;
    for (std::ptrdiff_t row = 0; row < dst_g_rgb.height(); ++row)
    {
        for (std::ptrdiff_t col = 0; col < dst_g_rgb.width(); ++col)
        {
            if (
                static_cast<float>(dst_g_rgb(col, row)[0]) != dst_o_rgb.at<cv::Vec2b>(0, 0)[j] || 
                static_cast<float>(dst_g_rgb(col, row)[1]) != dst_o_rgb.at<cv::Vec2b>(0, 0)[j + 1] || 
                static_cast<float>(dst_g_rgb(col, row)[2]) != dst_o_rgb.at<cv::Vec2b>(0, 0)[j + 2]
                )
            {
                std::cout << "row col  " << row << " " << col << "\n";
                return 0;
            }
            j += 3;
        }
    }
    return 1;
}

void test_boundary_option_extend_zero()
{
    cv::Mat src_o_gray, dst_o_gray, src_o_rgb, dst_o_rgb;
    cv::Mat kernel_o = cv::Mat::ones(5, 5, CV_32F);
    kernel_o.at<float>(2, 2) = 0;

    double delta = 0;
    int ddepth = -1;
    bool flag = 1;

    src_o_gray.create(15, 15, CV_8UC1);
    src_o_rgb.create(9, 9, CV_8UC3);
    fill_opencv_gray_image(src_o_gray);
    fill_opencv_rgb_image(src_o_rgb);

    gil::gray8_image_t src_g_gray(15, 15), dst_g_gray(15, 15), dst_g_gray_fixed(15, 15);
    gil::rgb8_image_t src_g_rgb(9, 9), dst_g_rgb(9, 9), dst_g_rgb_fixed(9, 9);
    std::vector<float> vec(25, 1.0f);
    vec[12] = 0;

    fill_gil_gray_image(gil::view(src_g_gray));
    fill_gil_rgb_image(gil::view(src_g_rgb));

    for (std::ptrdiff_t x = 0; x < 5; ++x)
    {
        for (std::ptrdiff_t y = 0; y < 5; ++y)
        {
            cv::Point anchor = cv::Point(x, y);
            gil::detail::kernel_2d<float> kernel_g(vec.begin(), vec.size(), y, x);
            gil::detail::kernel_2d_fixed<float, 5> kernel_g_fixed(vec.cbegin(), y, x);

            cv::filter2D(src_o_gray, dst_o_gray, ddepth, kernel_o, anchor, delta, cv::BORDER_CONSTANT);
            gil::correlate_2d<gil::gray8_pixel_t>(gil::view(src_g_gray), kernel_g, gil::view(dst_g_gray), 
                gil::boundary_option::extend_zero);
            gil::correlate_2d_fixed<gil::gray8_pixel_t>(gil::view(src_g_gray), kernel_g_fixed, 
                gil::view(dst_g_gray_fixed), gil::boundary_option::extend_zero);

            cv::filter2D(src_o_rgb, dst_o_rgb, ddepth, kernel_o, anchor, delta, cv::BORDER_CONSTANT);
            gil::correlate_2d<gil::rgb8_pixel_t>(gil::view(src_g_rgb), kernel_g, gil::view(dst_g_rgb), 
                gil::boundary_option::extend_zero);
            gil::correlate_2d_fixed<gil::rgb8_pixel_t>(gil::view(src_g_rgb), kernel_g_fixed, 
                gil::view(dst_g_rgb_fixed), gil::boundary_option::extend_zero);

            if (test_gray_images(dst_o_gray, gil::view(dst_g_gray)) == 0 || 
                test_gray_images(dst_o_gray, gil::view(dst_g_gray_fixed)) == 0 || 
                test_rgb_images(dst_o_rgb, gil::view(dst_g_rgb)) == 0 || 
                test_rgb_images(dst_o_rgb, gil::view(dst_g_rgb_fixed)) == 0)
            {
                std::cout << "Test Failed \n";
                std::cout << x << " " << y << "\n";
                flag = 0;
                break;
            }
        }
        if (flag == 0)
        {
            break;
        }
    }
}

void test_boundary_option_extend_constant()
{
    cv::Mat src_o_gray, dst_o_gray, src_o_rgb, dst_o_rgb;
    cv::Mat kernel_o = cv::Mat::ones(5, 5, CV_32F);
    kernel_o.at<float>(2, 2) = 0;

    double delta = 0;
    int ddepth = -1;
    bool flag = 1;

    src_o_gray.create(15, 15, CV_8UC1);
    src_o_rgb.create(9, 9, CV_8UC3);

    fill_opencv_gray_image(src_o_gray);
    fill_opencv_rgb_image(src_o_rgb);

    gil::gray8_image_t src_g_gray(15, 15), dst_g_gray(15, 15), dst_g_gray_fixed(15, 15);
    gil::rgb8_image_t src_g_rgb(9, 9), dst_g_rgb(9, 9), dst_g_rgb_fixed(9, 9);
    std::vector<float> vec(25, 1.0f);
    vec[12] = 0;

    fill_gil_gray_image(gil::view(src_g_gray));
    fill_gil_rgb_image(gil::view(src_g_rgb));

    for (std::ptrdiff_t x = 0; x < 5; ++x)
    {
        for (std::ptrdiff_t y = 0; y < 5; ++y)
        {
            cv::Point anchor = cv::Point(x, y);
            gil::detail::kernel_2d<float> kernel_g(vec.begin(), vec.size(), y, x);
            gil::detail::kernel_2d_fixed<float, 5> kernel_g_fixed(vec.begin(), y, x);

            cv::filter2D(src_o_gray, dst_o_gray, ddepth, kernel_o, anchor, delta, cv::BORDER_REPLICATE);
            gil::correlate_2d<gil::gray8_pixel_t>(gil::view(src_g_gray), kernel_g, gil::view(dst_g_gray), 
                gil::boundary_option::extend_constant);
            gil::correlate_2d_fixed<gil::gray8_pixel_t>(gil::view(src_g_gray), kernel_g_fixed, 
                gil::view(dst_g_gray_fixed), gil::boundary_option::extend_constant);

            cv::filter2D(src_o_rgb, dst_o_rgb, ddepth, kernel_o, anchor, delta, cv::BORDER_REPLICATE);
            gil::correlate_2d<gil::rgb8_pixel_t>(gil::view(src_g_rgb), kernel_g, gil::view(dst_g_rgb), 
                gil::boundary_option::extend_constant);
            gil::correlate_2d_fixed<gil::rgb8_pixel_t>(gil::view(src_g_rgb), kernel_g_fixed, gil::view(dst_g_rgb_fixed), 
                gil::boundary_option::extend_constant);

            if (test_gray_images(dst_o_gray, gil::view(dst_g_gray)) == 0 || 
                test_gray_images(dst_o_gray, gil::view(dst_g_gray_fixed)) == 0 || 
                test_rgb_images(dst_o_rgb, gil::view(dst_g_rgb)) == 0 || 
                test_rgb_images(dst_o_rgb, gil::view(dst_g_rgb_fixed)) == 0)
            {
                std::cout << "Test Failed \n";
                std::cout << x << " " << y << "\n";
                flag = 0;
                break;
            }
        }
        if (flag == 0)
        {
            break;
        }
    }
}

void test_separable_correlation()
{
    cv::Mat src_o_gray, dst_o_gray;
    cv::Mat kernel_o = cv::Mat::ones(5, 5, CV_32F);

    double delta = 0;
    int ddepth = -1;
    bool flag = 1;

    src_o_gray.create(15, 15, CV_8UC1);
    fill_opencv_gray_image(src_o_gray);

    gil::gray8_image_t src_g_gray(15, 15), dst_g_gray(15, 15), dst_g_gray_fixed(15, 15);
    std::vector<float> vec(25, 1.0f);

    fill_gil_gray_image(gil::view(src_g_gray));

    for (std::ptrdiff_t x = 0; x < 5; ++x)
    {
        for (std::ptrdiff_t y = 0; y < 5; ++y)
        {
            cv::Point anchor = cv::Point(x, y);
            gil::detail::kernel_2d<float> kernel_g(vec.begin(), vec.size(), y, x);
            gil::detail::kernel_2d_fixed<float, 5> kernel_g_fixed(vec.begin(), y, x);

            cv::filter2D(src_o_gray, dst_o_gray, ddepth, kernel_o, anchor, delta, cv::BORDER_CONSTANT);
            gil::correlate_2d<gil::gray8_pixel_t>(gil::view(src_g_gray), kernel_g, gil::view(dst_g_gray), 
                gil::boundary_option::extend_zero);
            gil::correlate_2d_fixed<gil::gray8_pixel_t>(gil::view(src_g_gray), kernel_g_fixed, 
                gil::view(dst_g_gray_fixed), gil::boundary_option::extend_zero);

            if (test_gray_images(dst_o_gray, gil::view(dst_g_gray)) == 0 || 
                test_gray_images(dst_o_gray, gil::view(dst_g_gray_fixed)) == 0)
            {
                std::cout << "Test Failed\n";
                std::cout << x << " " << y << "\n";
                flag = 0;
                break;
            }
        }
        if (flag == 0)
        {
            break;
        }
    }
}

void test_boundary_option_extend_reflection()
{
    cv::Mat src_o_gray, dst_o_gray, src_o_rgb, dst_o_rgb;
    cv::Mat kernel_o = cv::Mat::ones(5, 5, CV_32F);
    kernel_o.at<float>(2, 2) = 0;

    double delta = 0;
    int ddepth = -1;
    bool flag = 1;

    src_o_gray.create(15, 15, CV_8UC1);
    src_o_rgb.create(9, 9, CV_8UC3);

    fill_opencv_gray_image(src_o_gray);
    fill_opencv_rgb_image(src_o_rgb);

    gil::gray8_image_t src_g_gray(15, 15), dst_g_gray(15, 15), dst_g_gray_fixed(15, 15);
    gil::rgb8_image_t src_g_rgb(9, 9), dst_g_rgb(9, 9), dst_g_rgb_fixed(9, 9);
    std::vector<float> vec(25, 1.0f);
    vec[12] = 0;

    fill_gil_gray_image(gil::view(src_g_gray));
    fill_gil_rgb_image(gil::view(src_g_rgb));

    for (std::ptrdiff_t x = 0; x < 5; ++x)
    {
        for (std::ptrdiff_t y = 0; y < 5; ++y)
        {
            cv::Point anchor = cv::Point(x, y);
            gil::detail::kernel_2d<float> kernel_g(vec.begin(), vec.size(), y, x);
            gil::detail::kernel_2d_fixed<float, 5> kernel_g_fixed(vec.begin(), y, x);

            cv::filter2D(src_o_gray, dst_o_gray, ddepth, kernel_o, anchor, delta, cv::BORDER_REFLECT);
            gil::correlate_2d<gil::gray8_pixel_t>(gil::view(src_g_gray), kernel_g, gil::view(dst_g_gray), 
                gil::boundary_option::extend_reflection);
            gil::correlate_2d_fixed<gil::gray8_pixel_t>(gil::view(src_g_gray), kernel_g_fixed, 
                gil::view(dst_g_gray_fixed), gil::boundary_option::extend_reflection);

            cv::filter2D(src_o_rgb, dst_o_rgb, ddepth, kernel_o, anchor, delta, cv::BORDER_REFLECT);
            gil::correlate_2d<gil::rgb8_pixel_t>(gil::view(src_g_rgb), kernel_g, gil::view(dst_g_rgb), 
                gil::boundary_option::extend_reflection);
            gil::correlate_2d_fixed<gil::rgb8_pixel_t>(gil::view(src_g_rgb), kernel_g_fixed, gil::view(dst_g_rgb_fixed), 
                gil::boundary_option::extend_reflection);

            if (test_gray_images(dst_o_gray, gil::view(dst_g_gray)) == 0 || 
                test_gray_images(dst_o_gray, gil::view(dst_g_gray_fixed)) == 0 || 
                test_rgb_images(dst_o_rgb, gil::view(dst_g_rgb)) == 0 || 
                test_rgb_images(dst_o_rgb, gil::view(dst_g_rgb_fixed)) == 0) 
            {
                std::cout << "Test Failed \n";
                std::cout << x << " " << y << "\n";
                flag = 0;
                break;
            }
        }
        if (flag == 0)
        {
            break;
        }
    }
}

void test_boundary_option_output_zero()
{
    cv::Mat src_o_gray, dst_o_gray, src_o_rgb, dst_o_rgb;
    cv::Mat kernel_o = cv::Mat::ones(5, 5, CV_32F);
    kernel_o.at<float>(2, 2) = 0;

    double delta = 0;
    int ddepth = -1;
    bool flag = 1;

    src_o_gray.create(15, 15, CV_8UC1);
    dst_o_gray.create(15, 15, CV_8UC1);
    src_o_rgb.create(9, 9, CV_8UC3);
    dst_o_rgb.create(9, 9, CV_8UC3);

    fill_opencv_gray_image(src_o_gray);
    fill_opencv_rgb_image(src_o_rgb);

    gil::gray8_image_t src_g_gray(15, 15), dst_g_gray(15, 15), dst_g_gray_fixed(15, 15);
    gil::rgb8_image_t src_g_rgb(9, 9), dst_g_rgb(9, 9), dst_g_rgb_fixed(9, 9);
    std::vector<float> vec(25, 1.0f);
    vec[12] = 0;

    fill_gil_gray_image(gil::view(src_g_gray));
    fill_gil_rgb_image(gil::view(src_g_rgb));

    for (std::ptrdiff_t x = 0; x < 5; ++x)
    {
        for (std::ptrdiff_t y = 0; y < 5; ++y)
        {
            cv::Rect rect_gray(x, y, 11, 11);
            cv::Rect rect_rgb(x, y, 5, 5);
            cv::Point anchor = cv::Point(x, y);
            gil::detail::kernel_2d<float> kernel_g(vec.begin(), vec.size(), y, x);
            gil::detail::kernel_2d_fixed<float, 5> kernel_g_fixed(vec.cbegin(), y, x);

            uint8_t i = 0, j = 0;
            while (i <= dst_o_gray.rows * dst_o_gray.cols)
            {
                dst_o_gray.at<cv::Vec2b>(0, 0)[i] = 0;
                ++i;
            }
            i = 0;
            while (i <= dst_o_rgb.rows * dst_o_rgb.cols)
            {
                dst_o_rgb.at<cv::Vec2b>(0, 0)[j] = 0;
                dst_o_rgb.at<cv::Vec2b>(0, 0)[j + 1] = 0;
                dst_o_rgb.at<cv::Vec2b>(0, 0)[j + 2] = 0;
                ++i, j += 3;
            }

            cv::filter2D(src_o_gray(rect_gray), dst_o_gray(rect_gray), ddepth, kernel_o, anchor, 
                delta, cv::BORDER_CONSTANT);
            gil::correlate_2d<gil::gray8_pixel_t>(gil::view(src_g_gray), kernel_g, gil::view(dst_g_gray), 
                gil::boundary_option::output_zero);
            gil::correlate_2d_fixed<gil::gray8_pixel_t>(gil::view(src_g_gray), kernel_g_fixed, 
                gil::view(dst_g_gray_fixed), gil::boundary_option::output_zero);

            cv::filter2D(src_o_rgb(rect_rgb), dst_o_rgb(rect_rgb), ddepth, kernel_o, anchor, delta, 
                cv::BORDER_CONSTANT);
            gil::correlate_2d<gil::rgb8_pixel_t>(gil::view(src_g_rgb), kernel_g, gil::view(dst_g_rgb), 
                gil::boundary_option::output_zero);
            gil::correlate_2d_fixed<gil::rgb8_pixel_t>(gil::view(src_g_rgb), kernel_g_fixed, gil::view(dst_g_rgb_fixed), 
                gil::boundary_option::output_zero);

            if (test_gray_images(dst_o_gray, gil::view(dst_g_gray)) == 0 || 
                test_gray_images(dst_o_gray, gil::view(dst_g_gray_fixed)) == 0 || 
                test_rgb_images(dst_o_rgb, gil::view(dst_g_rgb)) == 0 || 
                test_rgb_images(dst_o_rgb, gil::view(dst_g_rgb_fixed)) == 0)
            {
                std::cout << "Test Failed \n";
                std::cout << x << " " << y << "\n";
                flag = 0;
                break;
            }
        }
        if (flag == 0)
        {
            break;
        }
    }
}

void test_boundary_option_output_ignore()
{
    cv::Mat src_o_gray, dst_o_gray, src_o_rgb, dst_o_rgb;
    cv::Mat kernel_o = cv::Mat::ones(5, 5, CV_32F);
    kernel_o.at<float>(2, 2) = 0;

    double delta = 0;
    int ddepth = -1;
    bool flag = 1;

    src_o_gray.create(15, 15, CV_8UC1);
    dst_o_gray.create(15, 15, CV_8UC1);
    src_o_rgb.create(9, 9, CV_8UC3);
    dst_o_rgb.create(9, 9, CV_8UC3);

    fill_opencv_gray_image(src_o_gray);
    fill_opencv_rgb_image(src_o_rgb);

    gil::gray8_image_t src_g_gray(15, 15), dst_g_gray(15, 15), dst_g_gray_fixed(15, 15);
    gil::rgb8_image_t src_g_rgb(9, 9), dst_g_rgb(9, 9), dst_g_rgb_fixed(9, 9);
    std::vector<float> vec(25, 1.0f);
    vec[12] = 0;

    fill_gil_gray_image(gil::view(src_g_gray));
    fill_gil_rgb_image(gil::view(src_g_rgb));

    for (std::ptrdiff_t x = 0; x < 5; ++x)
    {
        for (std::ptrdiff_t y = 0; y < 5; ++y)
        {
            cv::Rect rect_gray(x, y, 11, 11);
            cv::Rect rect_rgb(x, y, 5, 5);
            cv::Point anchor = cv::Point(x, y);
            gil::detail::kernel_2d<float> kernel_g(vec.begin(), vec.size(), y, x);
            gil::detail::kernel_2d_fixed<float, 5> kernel_g_fixed(vec.cbegin(), y, x);

            uint8_t i = 0, j = 0;
            while (i <= dst_o_gray.rows * dst_o_gray.cols)
            {
                dst_o_gray.at<cv::Vec2b>(0, 0)[i] = i % 10;
                ++i;
            }
            i = 0;
            while (i <= dst_o_rgb.rows * dst_o_rgb.cols)
            {
                dst_o_rgb.at<cv::Vec2b>(0, 0)[j] = i % 10;
                dst_o_rgb.at<cv::Vec2b>(0, 0)[j + 1] = i % 10;
                dst_o_rgb.at<cv::Vec2b>(0, 0)[j + 2] = i % 10;
                ++i, j += 3;
            }

            cv::filter2D(src_o_gray(rect_gray), dst_o_gray(rect_gray), ddepth, kernel_o, anchor, 
                delta, cv::BORDER_CONSTANT);
            gil::correlate_2d<gil::gray8_pixel_t>(gil::view(src_g_gray), kernel_g, gil::view(dst_g_gray), 
                gil::boundary_option::output_ignore);
            gil::correlate_2d_fixed<gil::gray8_pixel_t>(gil::view(src_g_gray), kernel_g_fixed, 
                gil::view(dst_g_gray_fixed), gil::boundary_option::output_ignore);

            cv::filter2D(src_o_rgb(rect_rgb), dst_o_rgb(rect_rgb), ddepth, kernel_o, anchor, delta, 
                cv::BORDER_CONSTANT);
            gil::correlate_2d<gil::rgb8_pixel_t>(gil::view(src_g_rgb), kernel_g, gil::view(dst_g_rgb), 
                gil::boundary_option::output_ignore);
            gil::correlate_2d_fixed<gil::rgb8_pixel_t>(gil::view(src_g_rgb), kernel_g_fixed, gil::view(dst_g_rgb_fixed), 
                gil::boundary_option::output_ignore);

            if (test_gray_images(dst_o_gray, gil::view(dst_g_gray)) == 0 || 
                test_gray_images(dst_o_gray, gil::view(dst_g_gray_fixed)) == 0 || 
                test_rgb_images(dst_o_rgb, gil::view(dst_g_rgb)) == 0 || 
                test_rgb_images(dst_o_rgb, gil::view(dst_g_rgb_fixed)) == 0)
            {
                std::cout << "Test Failed \n";
                std::cout << x << " " << y << "\n";
                flag = 0;
                break;
            }
        }
        if (flag == 0)
        {
            break;
        }
    }
}

void test_boundary_option_extend_padded()
{
    cv::Mat src_o_gray, dst_o_gray, src_o_rgb, dst_o_rgb;
    cv::Mat kernel_o = cv::Mat::ones(3, 3, CV_32F);
    kernel_o.at<float>(1, 1) = 0;
    cv::Rect rect_gray(5, 5, 5, 5);
    cv::Rect rect_rgb(2, 2, 5, 5);

    double delta = 0;
    int ddepth = -1;
    bool flag = 1;

    src_o_gray.create(15, 15, CV_8UC1);
    src_o_rgb.create(9, 9, CV_8UC3);
    fill_opencv_gray_image(src_o_gray);
    fill_opencv_rgb_image(src_o_rgb);

    gil::gray8_image_t src_g_gray(15, 15), dst_g_gray(5, 5), dst_g_gray_fixed(5, 5);
    gil::rgb8_image_t src_g_rgb(9, 9), dst_g_rgb(5, 5), dst_g_rgb_fixed(5, 5);
    std::vector<float> vec(9, 1.0f);
    vec[4] = 0;

    fill_gil_gray_image(gil::view(src_g_gray));
    fill_gil_rgb_image(gil::view(src_g_rgb));

    for (std::ptrdiff_t x = 0; x < 3; ++x)
    {
        for (std::ptrdiff_t y = 0; y < 3; ++y)
        {
            cv::Point anchor = cv::Point(x, y);
            gil::detail::kernel_2d<float> kernel_g(vec.begin(), vec.size(), y, x);
            gil::detail::kernel_2d_fixed<float, 3> kernel_g_fixed(vec.cbegin(), y, x);

            cv::filter2D(src_o_gray, dst_o_gray, ddepth, kernel_o, anchor, delta, cv::BORDER_CONSTANT);
            gil::correlate_2d<gil::gray8_pixel_t>(
                gil::subimage_view(gil::view(src_g_gray), 5, 5, 5, 5), 
                kernel_g, gil::view(dst_g_gray), gil::boundary_option::extend_padded);
            gil::correlate_2d_fixed<gil::gray8_pixel_t>(
                gil::subimage_view(gil::view(src_g_gray), 5, 5, 5, 5), 
                kernel_g_fixed, gil::view(dst_g_gray_fixed), gil::boundary_option::extend_padded);

            cv::filter2D(src_o_rgb, dst_o_rgb, ddepth, kernel_o, anchor, delta, cv::BORDER_CONSTANT);
            gil::correlate_2d<gil::rgb8_pixel_t>(
                gil::subimage_view(gil::view(src_g_rgb), 2, 2, 5, 5), 
                kernel_g, gil::view(dst_g_rgb), gil::boundary_option::extend_padded);
            gil::correlate_2d_fixed<gil::rgb8_pixel_t>(
                gil::subimage_view(gil::view(src_g_rgb), 2, 2, 5, 5), 
                kernel_g_fixed, gil::view(dst_g_rgb_fixed), gil::boundary_option::extend_padded);

            cv::Mat dst_o_gray_d = dst_o_gray(rect_gray).clone();
            cv::Mat dst_o_rgb_d = dst_o_rgb(rect_rgb).clone();

            if (test_gray_images(dst_o_gray_d, gil::view(dst_g_gray)) == 0 || 
                test_gray_images(dst_o_gray_d, gil::view(dst_g_gray_fixed)) == 0 || 
                test_rgb_images(dst_o_rgb_d, gil::view(dst_g_rgb)) == 0 || 
                test_rgb_images(dst_o_rgb_d, gil::view(dst_g_rgb_fixed)) == 0)
            {
                std::cout << "Test Failed \n";
                std::cout << x << " " << y << "\n";
                flag = 0;
                break;
            }
        }
        if (flag == 0)
        {
            break;
        }
    }
}

int main()
{
    test_separable_correlation();
    test_boundary_option_extend_zero();
    test_boundary_option_extend_constant();
    test_boundary_option_extend_reflection();
    test_boundary_option_extend_padded();
    test_boundary_option_output_zero();
    test_boundary_option_output_ignore();
}
