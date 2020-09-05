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

#include <iostream>
#include <limits>
#include <vector>

namespace gil = boost::gil;

int main()
{
    const std::size_t size = 128;
    gil::gray8_image_t input_image(size, size);
    auto input = gil::view(input_image);

    const std::ptrdiff_t circle_radius = 16;
    const gil::point_t circle_center = {64, 64};
    const auto rasterizer = gil::midpoint_circle_rasterizer{};
    std::vector<gil::point_t> circle_points(rasterizer.point_count(circle_radius));
    rasterizer(circle_radius, circle_center, circle_points.begin());
    for (const auto& point : circle_points)
    {
        input(point) = std::numeric_limits<gil::uint8_t>::max();
    }

    const auto radius_parameter =
        gil::hough_parameter<std::ptrdiff_t>::from_step_count(circle_radius, 3, 3);
    const auto x_parameter =
        gil::hough_parameter<std::ptrdiff_t>::from_step_count(circle_center.x, 3, 3);
    const auto y_parameter =
        gil::hough_parameter<std::ptrdiff_t>::from_step_count(circle_center.x, 3, 3);

    std::vector<gil::gray16_image_t> parameter_space_images(
        radius_parameter.step_count,
        gil::gray16_image_t(x_parameter.step_count, y_parameter.step_count));
    std::vector<gil::gray16_view_t> parameter_space_views(parameter_space_images.size());
    std::transform(parameter_space_images.begin(), parameter_space_images.end(),
                   parameter_space_views.begin(),
                   [](gil::gray16_image_t& img)
                   {
                       return gil::view(img);
                   });

    gil::hough_circle_transform_brute(input, radius_parameter, x_parameter, y_parameter,
                                      parameter_space_views.begin(), rasterizer);
    std::cout << parameter_space_views[3](3, 3) << '\n';
}
