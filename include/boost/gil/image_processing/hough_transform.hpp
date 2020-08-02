#ifndef BOOST_GIL_IMAGE_PROCESSING_HOUGH_TRANSFORM_HPP
#define BOOST_GIL_IMAGE_PROCESSING_HOUGH_TRANSFORM_HPP

#include <boost/gil/image_processing/hough_parameter.hpp>
#include <cmath>
#include <cstddef>

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

}} // namespace boost::gil

#endif
