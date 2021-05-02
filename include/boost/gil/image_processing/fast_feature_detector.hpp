#ifndef BOOST_GIL_IMAGE_PROCESSING_FAST_HPP
#define BOOST_GIL_IMAGE_PROCESSING_FAST_HPP

#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/locator.hpp>
#include <boost/gil/point.hpp>
#include <algorithm>
#include <cmath>
#include <vector>
namespace boost { namespace gil {
namespace detail {
template <typename srcview>
bool fast_feature_detector(const srcview& buffer, int r, int c, std::vector<point_t>& points, int t)
{
    int valid_points_count = 16;
    auto src_loc = buffer.xy_at(c, r);
    std::vector<int> threshold_indicator, intensity_array(16);
    std::vector<decltype(src_loc.cache_location(0, -1))> pointers(valid_points_count);
    //stroring intensities of pixels on circumference beforehand to decrease runtime
    for (int i = 0; i < 16; i++)
    {
        pointers[i] = src_loc.cache_location(points[i][0], points[i][1]);
        intensity_array[i] = src_loc[pointers[i]];
    }
    //calculating the flags to be used during segment test
    auto const I_p = buffer(point_t(c, r));
    //int low,high;
    std::transform(
        intensity_array.begin(),
        intensity_array.end(),
        back_inserter(threshold_indicator),
        [low = I_p - t, hi = I_p + t](auto const& intensity) {
            if (intensity < low)
                return -1;
            else if (intensity > hi)
                return 1;
            else
                return 0;
        });
    std::transform(
        intensity_array.begin(),
        intensity_array.end(),
        back_inserter(threshold_indicator),
        [low = I_p - t, hi = I_p + t](auto const& intensity) {
            if (intensity < low)
                return -1;
            else if (intensity > hi)
                return 1;
            else
                return 0;
        });
    //high speed test for eliminating non-corners
    for (int i = 0; i <= 6; i += 2)
    {
        if (threshold_indicator[i] == 0 && threshold_indicator[i + 8] == 0)
            return false;
    }
    for (int i = 1; i <= 7; i += 2)
    {
        if (threshold_indicator[i] == 0 && threshold_indicator[i + 8] == 0)
            return false;
    }
    //final segment test
    bool is_feature_point =
        threshold_indicator.end() !=
            std::search_n(threshold_indicator.begin(), threshold_indicator.end(), 9, -1) ||
        threshold_indicator.end() !=
            std::search_n(threshold_indicator.begin(), threshold_indicator.end(), 9, 1);
    return is_feature_point;
}
template <typename srcview>
std::ptrdiff_t
    calculate_score(srcview& src, int i, int j, std::vector<point_t>& points, int threshold)
{
    int score = threshold;
    std::ptrdiff_t low = threshold;
    std::ptrdiff_t high = 255;
    //score measure used= highest threshold for which a corner remains a corner. The cornerness of a corner decreases with increasing threshold
    while (high - low > 1)
    {
        int mid = (low + high) / 2;
        if (fast_feature_detector(src, i, j, points, mid))
        {
            low = mid;
            score = std::max(score, mid);
        }
        else
        {
            high = mid - 1;
        }
    }
    return low - 1;
}
}  // namespace detail
template <typename srcview>
void fast(
    srcview& src,
    std::vector<point_t>& keypoints,
    std::vector<int>& scores,
    bool nonmax = true,
    int threshold = 10)
{
    //coordinates of a bresenham circle of radius 3
    std::vector<point_t> final_points_clockwise{
        point_t(3, 0),
        point_t(3, 1),
        point_t(2, 2),
        point_t(1, 3),
        point_t(0, 3),
        point_t(-1, 3),
        point_t(-2, 2),
        point_t(-3, 1),
        point_t(-3, 0),
        point_t(-3, -1),
        point_t(-2, -2),
        point_t(-1, -3),
        point_t(0, -3),
        point_t(1, -3),
        point_t(2, -2),
        point_t(3, -1)};
    //FAST features only calculated on grayscale images
    auto input_image_view = color_converted_view<gray8_pixel_t>(src);
    gray8_image_t FAST_image(src.dimensions());
    // scores to be used during nonmaximum suppression
    gray8_view_t FAST_SCORE_MATRIX = view(FAST_image);
    fill_pixels(FAST_SCORE_MATRIX, gray8_pixel_t(0));
    std::vector<point_t> kp;
    for (int i = 3; i < src.height() - 3; i++)
    {
        for (int j = 3; j < src.width() - 3; j++)
        {
            if (detail::fast_feature_detector(
                    input_image_view, i, j, final_points_clockwise, threshold))
            {
                kp.push_back(point_t(j, i));
            }
        }
    }

    for (auto u : kp)
    {
        int score = 0;
        score = detail::calculate_score(
            input_image_view, u[1], u[0], final_points_clockwise, threshold);
        FAST_SCORE_MATRIX(u[0], u[1])[0] = gray8_pixel_t(score);
    }
    for (auto u : kp)
    {
        int i = u[1];
        int j = u[0];
        int score = 0;
        score = int(FAST_SCORE_MATRIX(j, i)[0]);
        //performing nonmaximum suppression
        if (!nonmax || score > FAST_SCORE_MATRIX(j - 1, i)[0] &&
                           score > FAST_SCORE_MATRIX(j + 1, i)[0] &&
                           score > FAST_SCORE_MATRIX(j - 1, i - 1)[0] &&
                           score > FAST_SCORE_MATRIX(j, i - 1)[0] &&
                           score > FAST_SCORE_MATRIX(j + 1, i - 1)[0] &&
                           score > FAST_SCORE_MATRIX(j - 1, i + 1)[0] &&
                           score > FAST_SCORE_MATRIX(j, i + 1)[0] &&
                           score > FAST_SCORE_MATRIX(j + 1, i + 1)[0])
        {
            keypoints.push_back(u);
            scores.push_back(score);
        }
    }
}
}}  // namespace boost::gil
#endif
