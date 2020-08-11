#include <boost/core/lightweight_test.hpp>
#include <boost/gil/rasterization/circle.hpp>
#include <cstddef>
#include <vector>

namespace gil = boost::gil;

void test_equation_following_test(std::ptrdiff_t radius)
{
    std::vector<gil::point_t> circle_points(gil::estimate_circle_point_count(radius));
    std::ptrdiff_t r_squared = radius * radius;
    gil::rasterize_circle_midpoint(radius, {0, 0}, circle_points.begin());

    for (const auto& point : circle_points)
    {
        std::ptrdiff_t one_greater_x = point.x + 1;
        std::ptrdiff_t expected_x_diff = one_greater_x * one_greater_x - point.x * point.x;
        std::ptrdiff_t one_greater_y = point.y + 1;
        std::ptrdiff_t expected_y_diff = one_greater_y * one_greater_y - point.y * point.y;
        std::ptrdiff_t response = std::abs(point.x * point.x + point.y * point.y - r_squared);
        // allow tolerance of off by one on each coordinate,
        // thus (x + 1)^2 - x^2 + (y + 1)^2 - y^2
        BOOST_TEST(response <= expected_x_diff + expected_y_diff);
    }
}

int main()
{
    for (std::ptrdiff_t radius = 5; radius <= 32; ++radius)
    {
        test_equation_following_test(radius);
    }

    return boost::report_errors();
}
