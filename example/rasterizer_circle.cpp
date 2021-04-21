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
#include <limits>
#include <vector>

namespace gil = boost::gil;

int main()
{
    const std::ptrdiff_t size = 256;
    gil::gray8_image_t buffer_image(size, size);
    auto buffer = gil::view(buffer_image);

    const std::ptrdiff_t radius = 64;
    const auto rasterizer = gil::trigonometric_circle_rasterizer{};
    std::vector<gil::point_t> circle_points(rasterizer.point_count(radius));
    rasterizer(radius, {128, 128}, circle_points.begin());
    for (const auto& point : circle_points)
    {
        buffer(point) = std::numeric_limits<gil::uint8_t>::max();
    }

    gil::write_view("circle.png", buffer, gil::png_tag{});
}
