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
    std::vector<gil::point_t> first_octant(gil::estimate_circle_point_count(radius) / 2);

    for (std::size_t i = 0, octant_index = 0; i < circle_points.size(); i += 8, ++octant_index)
    {
        first_octant[octant_index] = circle_points[i];
    }

    for (const auto& point : first_octant)
    {
        double y_exact = std::sqrt(radius * radius - point.x * point.x);
        std::ptrdiff_t lower_result = static_cast<std::ptrdiff_t>(std::floor(y_exact)) - 1;
        std::ptrdiff_t upper_result = static_cast<std::ptrdiff_t>(std::ceil(y_exact)) + 1;
        BOOST_TEST(point.y >= lower_result || point.y <= upper_result);
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
