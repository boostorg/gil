// Boost.GIL (Generic Image Library) - tests
//
// Copyright 2020 Olzhas Zhumabek <anonymous.from.applecity@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/gil.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <cmath>
#include <cstddef>
#include <iostream>

namespace gil = boost::gil;

int main()
{
    std::ptrdiff_t size = 32;
    gil::gray16_image_t input_image(size, size);
    auto input_view = gil::view(input_image);

    // fill secondary diagonal with ones
    // do note that origin is located at upper left,
    // not bottom left as in usual plots
    for (std::ptrdiff_t i = 0; i < size; ++i)
    {
        input_view(i, size - i - 1) = 1;
    }

    // print vertically flipped for better understanding of origin location
    for (std::ptrdiff_t y = size - 1; y >= 0; --y)
    {
        for (std::ptrdiff_t x = 0; x < size; ++x)
        {
            std::cout << input_view(x, y)[0] << ' ';
        }
        std::cout << '\n';
    }

    double minimum_theta_step = std::atan(1.0 / size);
    // this is the expected theta
    double _45_degrees = gil::detail::pi / 4;
    double _5_degrees = gil::detail::pi / 36;
    std::size_t step_count = 5;
    auto theta_parameter =
        gil::make_theta_parameter(_45_degrees, _5_degrees, input_view.dimensions());
    auto expected_radius = static_cast<std::ptrdiff_t>(std::round(std::cos(_45_degrees) * size));
    auto radius_parameter =
        gil::hough_parameter<std::ptrdiff_t>::from_step_size(expected_radius, 7, 1);
    gil::gray32_image_t accumulator_array_image(theta_parameter.step_count,
                                                radius_parameter.step_count);
    auto accumulator_array = gil::view(accumulator_array_image);
    gil::hough_line_transform(input_view, accumulator_array, theta_parameter, radius_parameter);
    std::cout << "expecting maximum at theta=" << _45_degrees << " and radius=" << expected_radius
              << '\n';
    for (std::size_t theta_index = 0; theta_index < theta_parameter.step_count; ++theta_index)
    {
        for (std::size_t radius_index = 0; radius_index < radius_parameter.step_count;
             ++radius_index)
        {
            double current_theta =
                theta_parameter.start_point + theta_index * theta_parameter.step_size;
            std::ptrdiff_t current_radius =
                radius_parameter.start_point + radius_parameter.step_size * radius_index;
            std::cout << "theta: " << current_theta << " radius: " << current_radius
                      << " accumulated value: " << accumulator_array(theta_index, radius_index)[0]
                      << '\n';
        }
    }
}
