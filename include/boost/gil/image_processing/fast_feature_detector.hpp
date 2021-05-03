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

/// \brief Implements the FAST corner detection algorithm by Edward Rosten.
///       Algorithm :-
///       1.Consider a circle of 16 pixels around a given pixel(Bresenham circle)
///       2.Decide a threshold t
///       3.Let I_p be the intensity of the central pixel.
///       4.Find if there are n consecutive pixels in those 16 pixels where the intensities of all pixels are <I_p-t or >I_p+t.Here n is taken as 9.
///       5.For step 4, to detect that a pixel is not a corner, check first whether the intensities at pixels from 0 to 6 (with a step of 2)or the 8th pixel from each of them satisfies
///         condition of step 4.If not,check the same condition on odd numbered pixels or their corresponding 8th pixel
///       6.If the checks in step 5 is not affirmative, the given pixel cannot be a corner.
///       7.Otherwise, proceed to find whether there are n consecutive pixels to satisfy the criterion. If yes, p is a corner, else no.
///
///       @param buffer -type of source image.Must be grayscale
///       @param r-      row index of the candidate pixel
///       @param c-      column index of candidate pixel
///       @param points- pixel locations for bresenham circle around the given pixel
///       @param t-      threshold
    
template <typename SrcView>
bool fast_feature_detector(SrcView const& buffer, std::size_t r, std::size_t c, std::vector<point_t>& points, int t)
{
    auto src_loc = buffer.xy_at(c, r);
    std::vector<int> threshold_indicator;
    std::vector<int> intensity_array(16);
    std::vector<decltype(src_loc.cache_location(0, -1))> pointers(16);
    //stroring intensities of pixels on circumference beforehand to decrease runtime
    for (std::ptrdiff_t i = 0; i < 16; i++)
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
    for (std::ptrdiff_t i = 0; i <= 6; i += 2)
    {
        if (threshold_indicator[i] == 0 && threshold_indicator[i + 8] == 0)
            return false;
    }
    for (std::ptrdiff_t i = 1; i <= 7; i += 2)
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

///\brief assigns a score to each detected corner to measure their degree of cornerness.
///           Algorithm
///           Perform a binary search on threshold t to find out the maximum threshold
///           for which a corner remains a corner
///
///           @param src         -type of input image
///           @param i           -row index of the detected corner
///           @param j           -column index of the detected corner
///           @param points      -pixel locations for bresenham circle around the given pixel
///           @param threshold   -initial threshold given as input
    
template <typename SrcView>
std::size_t
    calculate_score(SrcView const& src,std::size_t i,std::size_t j, std::vector<point_t>& points,std::size_t threshold)
{
    std::size_t low = threshold;
    std::size_t high = 255;
    //score measure used= highest threshold for which a corner remains a corner. The cornerness of a corner decreases with increasing threshold
    while (high - low > 1)
    {
        std::size_t mid = (low + high) / 2;
        if (fast_feature_detector(src, i, j, points, mid))
        {
            low = mid;
        }
        else
        {
            high = mid - 1;
        }
    }
    return low - 1;
}
}  // namespace detail

/// \brief public function for using fast feature detector
///        @param src   -type of input image
///        @param keypoints   -vector for storing the locations of keypoints(corners)
///        @param scores      -vector for scores of each detected keypoint
///        @param nonmax      -indicates whether to perform nonmaximum suppression or not
///        @param threshold   -initial threshold given as input
    
template <typename SrcView>
void fast(
    SrcView const& src,
    std::vector<point_t>& keypoints,
    std::vector<std::size_t>& scores,
    bool nonmax = true,
    std::size_t threshold = 10)
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
    gray8_image_t fast_image(src.dimensions());
    // scores to be used during nonmaximum suppression
    gray8_view_t fast_score_matrix = view(fast_image);
    fill_pixels(fast_score_matrix, gray8_pixel_t(0));
    std::vector<point_t> kp;

    for (std::size_t i = 3; i < src.height() - 3; i++)
    {
        for (std::size_t j = 3; j < src.width() - 3; j++)
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
        fast_score_matrix(u[0], u[1])[0] = gray8_pixel_t(score);
    }

    for (auto u : kp)
    {
        std::size_t i = u[1];
        std::size_t j = u[0];
        std::size_t score = 0;
        score = int(fast_score_matrix(j, i)[0]);
        //performing nonmaximum suppression
        if (!nonmax || score > fast_score_matrix(j - 1, i)[0] &&
                           score > fast_score_matrix(j + 1, i)[0] &&
                           score > fast_score_matrix(j - 1, i - 1)[0] &&
                           score > fast_score_matrix(j, i - 1)[0] &&
                           score > fast_score_matrix(j + 1, i - 1)[0] &&
                           score > fast_score_matrix(j - 1, i + 1)[0] &&
                           score > fast_score_matrix(j, i + 1)[0] &&
                           score > fast_score_matrix(j + 1, i + 1)[0])
        {
            keypoints.push_back(u);
            scores.push_back(score);
        }
    }
}

}}  // namespace boost::gil
#endif
