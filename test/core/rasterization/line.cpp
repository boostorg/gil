#include <algorithm>
#include <boost/core/lightweight_test.hpp>
#include <boost/gil/rasterization/line.hpp>

#include <iostream>
#include <vector>

namespace gil = boost::gil;

void test_bresenham_rasterizer_follows_equation(std::ptrdiff_t width, std::ptrdiff_t height)
{
    double m = static_cast<double>(height) / width;
    const auto rasterizer = gil::bresenham_line_rasterizer{};
    const auto point_count = rasterizer.point_count(width, height);
    std::vector<gil::point_t> bresenham_points(point_count);
    rasterizer({0, 0}, {width - 1, height - 1}, bresenham_points.begin());

    for (std::ptrdiff_t i = 0; i < point_count; ++i)
    {
        gil::point_t expected_point{i, std::llround(m * i)};
        BOOST_TEST(expected_point == bresenham_points[i]);
    }
}

int main()
{
    const std::ptrdiff_t size = 256;
    for (std::ptrdiff_t width = 1; width < size; ++width)
    {
        for (std::ptrdiff_t height = 1; height <= width; ++height)
        {
            test_bresenham_rasterizer_follows_equation(width, width);
        }
    }

    return boost::report_errors();
}
