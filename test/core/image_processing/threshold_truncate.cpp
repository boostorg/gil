//
// Copyright 2019 Miral Shah <miralshah2211@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/gil/algorithm.hpp>
#include <boost/gil/gray.hpp>
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
    gil::threshold_truncate_mode mode,
    gil::threshold_direction direction,
    ChannelValue threshold_value,
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

    threshold_truncate(original_view, threshold_view, threshold_value, mode, direction);

    BOOST_TEST(equal_pixels(threshold_view, expected_view));
}

} // namespace

void test_threshold_truncate_regular()
{
    // truncation mode threshold and direction regular should clip all pixels
    // above the threshold to the threshold value
    test_threshold<gil::gray8_image_t>(
        gil::threshold_truncate_mode::threshold,
        gil::threshold_direction::regular,
        100,
        gil::gray8_pixel_t{50}, gil::gray8_pixel_t{150},
        gil::gray8_pixel_t{50}, gil::gray8_pixel_t{100});

    test_threshold<gil::gray8s_image_t>(
        gil::threshold_truncate_mode::threshold,
        gil::threshold_direction::regular,
        100,
        gil::gray8s_pixel_t{-50}, gil::gray8s_pixel_t{120},
        gil::gray8s_pixel_t{-50}, gil::gray8s_pixel_t{100});

    test_threshold<gil::gray32f_image_t>(
        gil::threshold_truncate_mode::threshold,
        gil::threshold_direction::regular,
        0.5f,
        gil::gray32f_pixel_t{0.3f}, gil::gray32f_pixel_t{0.7f},
        gil::gray32f_pixel_t{0.3f}, gil::gray32f_pixel_t{0.5f});

    test_threshold<gil::rgb8_image_t>(
        gil::threshold_truncate_mode::threshold,
        gil::threshold_direction::regular,
        100,
        gil::rgb8_pixel_t{50, 85, 135}, gil::rgb8_pixel_t{150, 205, 106},
        gil::rgb8_pixel_t{50, 85, 100}, gil::rgb8_pixel_t{100, 100, 100});
}

void test_threshold_truncate_inverse()
{
    // truncation mode threshold and direction inverse should clip all pixels
    // below the threshold to the threshold value
    test_threshold<gil::gray8_image_t>(
        gil::threshold_truncate_mode::threshold,
        gil::threshold_direction::inverse,
        100,
        gil::gray8_pixel_t{50}, gil::gray8_pixel_t{150},
        gil::gray8_pixel_t{100}, gil::gray8_pixel_t{150});

    test_threshold<gil::gray8s_image_t>(
        gil::threshold_truncate_mode::threshold,
        gil::threshold_direction::inverse,
        100,
        gil::gray8s_pixel_t{-50}, gil::gray8s_pixel_t{120},
        gil::gray8s_pixel_t{100}, gil::gray8s_pixel_t{120});

    test_threshold<gil::gray32f_image_t>(
        gil::threshold_truncate_mode::threshold,
        gil::threshold_direction::inverse,
        0.5f,
        gil::gray32f_pixel_t{0.3f}, gil::gray32f_pixel_t{0.7f},
        gil::gray32f_pixel_t{0.5f}, gil::gray32f_pixel_t{0.7f});

    test_threshold<gil::rgb8_image_t>(
        gil::threshold_truncate_mode::threshold,
        gil::threshold_direction::inverse,
        100,
        gil::rgb8_pixel_t{50, 85, 135}, gil::rgb8_pixel_t{150, 205, 106},
        gil::rgb8_pixel_t{100, 100, 135}, gil::rgb8_pixel_t{150, 205, 106});
}

void test_threshold_zero()
{
    // truncation mode zero and direction regular should zero all pixels below the threshold
    test_threshold<gil::gray8_image_t>(
        gil::threshold_truncate_mode::zero,
        gil::threshold_direction::regular,
        100,
        gil::gray8_pixel_t{50}, gil::gray8_pixel_t{150},
        gil::gray8_pixel_t{0}, gil::gray8_pixel_t{150});

    test_threshold<gil::gray8s_image_t>(
        gil::threshold_truncate_mode::zero,
        gil::threshold_direction::regular,
        100,
        gil::gray8s_pixel_t{-50}, gil::gray8s_pixel_t{120},
        gil::gray8s_pixel_t{-128}, gil::gray8s_pixel_t{120});

    test_threshold<gil::gray32f_image_t>(
        gil::threshold_truncate_mode::zero,
        gil::threshold_direction::regular,
        0.5f,
        gil::gray32f_pixel_t{0.3f}, gil::gray32f_pixel_t{0.7f},
        gil::gray32f_pixel_t{0.0f}, gil::gray32f_pixel_t{0.7f});

    test_threshold<gil::rgb8_image_t>(
        gil::threshold_truncate_mode::zero,
        gil::threshold_direction::regular,
        100,
        gil::rgb8_pixel_t{50, 85, 135}, gil::rgb8_pixel_t{150, 205, 106},
        gil::rgb8_pixel_t{0, 0, 135}, gil::rgb8_pixel_t{150, 205, 106});
}

void test_threshold_zero_inverse()
{
    // truncation mode zero and direction inverse should zero all pixels above the threshold
    test_threshold<gil::gray8_image_t>(
        gil::threshold_truncate_mode::zero,
        gil::threshold_direction::inverse,
        100,
        gil::gray8_pixel_t{50}, gil::gray8_pixel_t{150},
        gil::gray8_pixel_t{50}, gil::gray8_pixel_t{0});

    test_threshold<gil::gray8s_image_t>(
        gil::threshold_truncate_mode::zero,
        gil::threshold_direction::inverse,
        100,
        gil::gray8s_pixel_t{-50}, gil::gray8s_pixel_t{120},
        gil::gray8s_pixel_t{-50}, gil::gray8s_pixel_t{-128});

    test_threshold<gil::gray32f_image_t>(
        gil::threshold_truncate_mode::zero,
        gil::threshold_direction::inverse,
        0.5f,
        gil::gray32f_pixel_t{0.3f}, gil::gray32f_pixel_t{0.7f},
        gil::gray32f_pixel_t{0.3f}, gil::gray32f_pixel_t{0.0f});

    test_threshold<gil::rgb8_image_t>(
        gil::threshold_truncate_mode::zero,
        gil::threshold_direction::inverse,
        100,
        gil::rgb8_pixel_t{50, 85, 135}, gil::rgb8_pixel_t{150, 205, 106},
        gil::rgb8_pixel_t{50, 85, 0}, gil::rgb8_pixel_t{0, 0, 0});
}

int main()
{
    test_threshold_truncate_regular();
    test_threshold_truncate_inverse();
    test_threshold_zero();
    test_threshold_zero_inverse();

    return boost::report_errors();
}
