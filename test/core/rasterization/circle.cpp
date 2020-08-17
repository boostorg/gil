#include <boost/core/lightweight_test.hpp>
#include <boost/gil/rasterization/circle.hpp>
#include <cstddef>
#include <iostream>
#include <vector>

namespace gil = boost::gil;

void test_equation_following_test_midpoint(std::ptrdiff_t radius)
{
    std::vector<gil::point_t> circle_points(gil::estimate_circle_point_count_midpoint(radius));
    std::ptrdiff_t r_squared = radius * radius;
    gil::rasterize_circle_midpoint(radius, {0, 0}, circle_points.begin());
    std::vector<gil::point_t> first_octant(gil::estimate_circle_point_count_midpoint(radius) / 2);

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

void test_equation_following_test_trig(std::ptrdiff_t radius)
{
    std::vector<gil::point_t> circle_points(gil::estimate_circle_point_count_trig(radius));
    std::ptrdiff_t r_squared = radius * radius;
    gil::rasterize_circle_trig(radius, {0, 0}, circle_points.begin());
    std::vector<gil::point_t> first_octant(gil::estimate_circle_point_count_trig(radius) / 2);

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

void connectivity_test_midpoint(std::ptrdiff_t radius)
{
    std::vector<gil::point_t> circle_points(gil::estimate_circle_point_count_midpoint(radius));
    gil::rasterize_circle_midpoint(radius, {radius, radius}, circle_points.begin());
    for (std::size_t i = 0; i < 8; ++i)
    {
        std::vector<gil::point_t> octant(circle_points.size() / 8);
        for (std::size_t octant_index = i, index = 0; octant_index < circle_points.size();
             octant_index += 8, ++index)
        {
            octant[index] = circle_points[octant_index];
        }

        for (std::size_t index = 1; index < octant.size(); ++index)
        {
            const auto diff_x = std::abs(octant[index].x - octant[index - 1].x);
            const auto diff_y = std::abs(octant[index].y - octant[index - 1].y);
            BOOST_TEST_LE(diff_x, 1);
            BOOST_TEST_LE(diff_y, 1);
        }
    }
}

void connectivity_test_trig(std::ptrdiff_t radius)
{
    std::vector<gil::point_t> circle_points(gil::estimate_circle_point_count_trig(radius));
    gil::rasterize_circle_trig(radius, {radius, radius}, circle_points.begin());
    for (std::size_t i = 0; i < 8; ++i)
    {
        std::vector<gil::point_t> octant(circle_points.size() / 8);
        for (std::size_t octant_index = i, index = 0; octant_index < circle_points.size();
             octant_index += 8, ++index)
        {
            octant[index] = circle_points[octant_index];
        }

        for (std::size_t index = 1; index < octant.size(); ++index)
        {
            const auto diff_x = std::abs(octant[index].x - octant[index - 1].x);
            const auto diff_y = std::abs(octant[index].y - octant[index - 1].y);
            BOOST_TEST_LE(diff_x, 1);
            BOOST_TEST_LE(diff_y, 1);
        }
    }
}

int main()
{
    for (std::ptrdiff_t radius = 5; radius <= 512; ++radius)
    {
        test_equation_following_test_midpoint(radius);
        test_equation_following_test_trig(radius);
        connectivity_test_midpoint(radius);
        connectivity_test_trig(radius);
    }

    return boost::report_errors();
}
