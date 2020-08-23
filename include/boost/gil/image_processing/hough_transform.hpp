#ifndef BOOST_GIL_IMAGE_PROCESSING_HOUGH_TRANSFORM_HPP
#define BOOST_GIL_IMAGE_PROCESSING_HOUGH_TRANSFORM_HPP

#include <algorithm>
#include <boost/gil/image_processing/hough_parameter.hpp>
#include <boost/gil/rasterization/circle.hpp>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <vector>

namespace boost { namespace gil {
template <typename InputView, typename OutputView>
void hough_line_transform(const InputView& input_view, const OutputView& accumulator_array,
                          const hough_parameter<double>& theta,
                          const hough_parameter<std::ptrdiff_t>& radius)
{
    std::ptrdiff_t r_lower_bound = radius.start_point;
    std::ptrdiff_t r_upper_bound = r_lower_bound + radius.step_size * (radius.step_count - 1);

    for (std::ptrdiff_t y = 0; y < input_view.height(); ++y)
    {
        for (std::ptrdiff_t x = 0; x < input_view.width(); ++x)
        {
            if (!input_view(x, y)[0])
            {
                continue;
            }

            for (std::size_t theta_index = 0; theta_index < theta.step_count; ++theta_index)
            {
                double theta_current =
                    theta.start_point + theta.step_size * static_cast<double>(theta_index);
                std::ptrdiff_t current_r =
                    std::llround(static_cast<double>(x) * std::cos(theta_current) +
                                 static_cast<double>(y) * std::sin(theta_current));
                if (current_r < r_lower_bound || current_r > r_upper_bound)
                {
                    continue;
                }
                std::size_t r_index = static_cast<std::size_t>(
                    std::llround((current_r - radius.start_point) / radius.step_size));
                // one more safety guard to not get out of bounds
                if (r_index < radius.step_count)
                {
                    accumulator_array(theta_index, r_index)[0] += 1;
                }
            }
        }
    }
}

template <typename ImageView, typename ForwardIterator, typename Rasterizer>
void hough_circle_transform_brute(const ImageView& input,
                                  const hough_parameter<std::ptrdiff_t> radius_parameter,
                                  const hough_parameter<std::ptrdiff_t> x_parameter,
                                  const hough_parameter<std::ptrdiff_t>& y_parameter,
                                  ForwardIterator d_first, Rasterizer rasterizer)
{
    const auto width = input.width();
    const auto height = input.height();
    for (std::size_t radius_index = 0; radius_index < radius_parameter.step_count; ++radius_index)
    {
        const auto radius = radius_parameter.start_point +
                            radius_parameter.step_size * static_cast<std::ptrdiff_t>(radius_index);
        std::vector<point_t> circle_points(rasterizer.point_count(radius));
        rasterizer(radius, {0, 0}, circle_points.begin());
        // sort by scanline to improve cache coherence for row major images
        std::sort(circle_points.begin(), circle_points.end(),
                  [](const point_t& lhs, const point_t& rhs) { return lhs.y < rhs.y; });
        const auto translate = [](std::vector<point_t>& points, point_t offset) {
            std::transform(points.begin(), points.end(), points.begin(), [offset](point_t point) {
                return point_t(point.x + offset.x, point.y + offset.y);
            });
        };

        // in case somebody passes iterator to likes of std::vector<bool>
        typename std::iterator_traits<ForwardIterator>::reference current_image = *d_first;

        // the algorithm has to traverse over parameter space and look at input, instead
        // of vice versa, as otherwise it will call translate too many times, as input
        // is usually bigger than the coordinate portion of parameter space.
        // This might cause extensive cache misses
        for (std::size_t x_index = 0; x_index < x_parameter.step_count; ++x_index)
        {
            for (std::size_t y_index = 0; y_index < y_parameter.step_count; ++y_index)
            {
                const std::ptrdiff_t x = x_parameter.start_point + x_index * x_parameter.step_size;
                const std::ptrdiff_t y = y_parameter.start_point + y_index * y_parameter.step_size;

                auto translated_circle = circle_points;
                translate(translated_circle, {x, y});
                for (const auto& point : translated_circle)
                {
                    if (input(point))
                    {
                        ++current_image(x_index, y_index)[0];
                    }
                }
            }
        }
        ++d_first;
    }
}

}} // namespace boost::gil

#endif
