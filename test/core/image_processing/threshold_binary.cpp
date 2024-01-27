//
// Copyright 2019 Miral Shah <miralshah2211@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/gil/gray.hpp>
#include <boost/gil/algorithm.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_processing/threshold.hpp>

#include <boost/core/lightweight_test.hpp>

namespace gil = boost::gil;

namespace {

template <typename View, typename Pixel>
void fill_upper_and_lower_half(
    View const& view, Pixel const& upper_half_pixel, Pixel const& lower_half_pixel)
{
    fill_pixels(
        subimage_view(view, 0, 0, view.width(), view.height() / 2),
        upper_half_pixel);
    fill_pixels(
        subimage_view(view, 0, view.height() / 2, view.width(), view.height() / 2),
        lower_half_pixel);
}

template <typename Image, typename Pixel, typename ChannelValue>
void test_threshold(
    gil::threshold_direction direction,
    ChannelValue threshold_value,
    ChannelValue max_value,
    bool set_max_value,
    Pixel const& upper_half_pixel,
    Pixel const& lower_half_pixel,
    Pixel const& expected_upper_half_pixel,
    Pixel const& expected_lower_half_pixel)
{
    int const height = 4;
    int const width = 4;

    Image original_img(width, height);
    Image threshold_img(width, height);
    Image expected_img(width, height);

    auto original_view = gil::view(original_img);
    auto threshold_view = gil::view(threshold_img);
    auto expected_view = gil::view(expected_img);

    fill_upper_and_lower_half(
        original_view, upper_half_pixel, lower_half_pixel);
    fill_upper_and_lower_half(
        expected_view, expected_upper_half_pixel, expected_lower_half_pixel);

    if (set_max_value)
    {
        threshold_binary(
            original_view, threshold_view,
            threshold_value, max_value, direction);
    }
    else
    {
        threshold_binary(original_view, threshold_view, threshold_value, direction);
    }

    BOOST_TEST(equal_pixels(threshold_view, expected_view));
}

} // namespace

void test_threshold_binary()
{
    // threshold binary should set all pixels below the threshold to channel min
    // and above the threshold to the given max value or the channel max if no
    // max is supplied
    test_threshold<gil::gray8_image_t>(
        gil::threshold_direction::regular,
        100,
        150, true,
        gil::gray8_pixel_t{50}, gil::gray8_pixel_t{170},
        gil::gray8_pixel_t{0}, gil::gray8_pixel_t{150});
    test_threshold<gil::gray8_image_t>(
        gil::threshold_direction::regular,
        100,
        150, false,
        gil::gray8_pixel_t{50}, gil::gray8_pixel_t{170},
        gil::gray8_pixel_t{0}, gil::gray8_pixel_t{255});

    test_threshold<gil::gray8s_image_t>(
        gil::threshold_direction::regular,
        50,
        100, true,
        gil::gray8s_pixel_t{50}, gil::gray8s_pixel_t{110},
        gil::gray8s_pixel_t{-128}, gil::gray8s_pixel_t{100});
    test_threshold<gil::gray8s_image_t>(
        gil::threshold_direction::regular,
        50,
        100, false,
        gil::gray8s_pixel_t{50}, gil::gray8s_pixel_t{110},
        gil::gray8s_pixel_t{-128}, gil::gray8s_pixel_t{127});

    test_threshold<gil::gray32f_image_t>(
        gil::threshold_direction::regular,
        0.5f,
        0.6f, true,
        gil::gray32f_pixel_t{0.3f}, gil::gray32f_pixel_t{0.7f},
        gil::gray32f_pixel_t{0.0f}, gil::gray32f_pixel_t{0.6f});
    test_threshold<gil::gray32f_image_t>(
        gil::threshold_direction::regular,
        0.5f,
        0.6f, false,
        gil::gray32f_pixel_t{0.3f}, gil::gray32f_pixel_t{0.7f},
        gil::gray32f_pixel_t{0.0f}, gil::gray32f_pixel_t{1.0f});

    test_threshold<gil::rgb8_image_t>(
        gil::threshold_direction::regular,
        100,
        165, true,
        gil::rgb8_pixel_t{50, 155, 115}, gil::rgb8_pixel_t{203, 9, 60},
        gil::rgb8_pixel_t{0, 165, 165}, gil::rgb8_pixel_t{165, 0, 0});
    test_threshold<gil::rgb8_image_t>(
        gil::threshold_direction::regular,
        100,
        165, false,
        gil::rgb8_pixel_t{50, 155, 115}, gil::rgb8_pixel_t{203, 9, 60},
        gil::rgb8_pixel_t{0, 255, 255}, gil::rgb8_pixel_t{255, 0, 0});
}

void test_threshold_binary_inverse()
{
    // inverse threshold binary should set all pixels above the threshold to channel min
    // and below the threshold to the given max value or the channel max if no
    // max is supplied
    test_threshold<gil::gray8_image_t>(
        gil::threshold_direction::inverse,
        100,
        150, true,
        gil::gray8_pixel_t{50}, gil::gray8_pixel_t{170},
        gil::gray8_pixel_t{150}, gil::gray8_pixel_t{0});
    test_threshold<gil::gray8_image_t>(
        gil::threshold_direction::inverse,
        100,
        150, false,
        gil::gray8_pixel_t{50}, gil::gray8_pixel_t{170},
        gil::gray8_pixel_t{255}, gil::gray8_pixel_t{0});

    test_threshold<gil::gray8s_image_t>(
        gil::threshold_direction::inverse,
        50,
        100, true,
        gil::gray8s_pixel_t{50}, gil::gray8s_pixel_t{110},
        gil::gray8s_pixel_t{100}, gil::gray8s_pixel_t{-128});
    test_threshold<gil::gray8s_image_t>(
        gil::threshold_direction::inverse,
        50,
        100, false,
        gil::gray8s_pixel_t{50}, gil::gray8s_pixel_t{110},
        gil::gray8s_pixel_t{127}, gil::gray8s_pixel_t{-128});

    test_threshold<gil::gray32f_image_t>(
        gil::threshold_direction::inverse,
        0.5f,
        0.6f, true,
        gil::gray32f_pixel_t{0.3f}, gil::gray32f_pixel_t{0.7f},
        gil::gray32f_pixel_t{0.6f}, gil::gray32f_pixel_t{0.0f});
    test_threshold<gil::gray32f_image_t>(
        gil::threshold_direction::inverse,
        0.5f,
        0.6f, false,
        gil::gray32f_pixel_t{0.3f}, gil::gray32f_pixel_t{0.7f},
        gil::gray32f_pixel_t{1.0f}, gil::gray32f_pixel_t{0.0f});

    test_threshold<gil::rgb8_image_t>(
        gil::threshold_direction::inverse,
        100,
        165, true,
        gil::rgb8_pixel_t{50, 155, 115}, gil::rgb8_pixel_t{203, 9, 60},
        gil::rgb8_pixel_t{165, 0, 0}, gil::rgb8_pixel_t{0, 165, 165});
    test_threshold<gil::rgb8_image_t>(
        gil::threshold_direction::inverse,
        100,
        165, false,
        gil::rgb8_pixel_t{50, 155, 115}, gil::rgb8_pixel_t{203, 9, 60},
        gil::rgb8_pixel_t{255, 0, 0}, gil::rgb8_pixel_t{0, 255, 255});
}

int main()
{
    test_threshold_binary();
    test_threshold_binary_inverse();

    return boost::report_errors();
}
