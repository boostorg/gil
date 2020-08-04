#include <algorithm>
#include <boost/core/lightweight_test.hpp>
#include <boost/gil/rasterization/line.hpp>

#include <iostream>
#include <vector>

namespace gil = boost::gil;

void equivalence_test(std::ptrdiff_t width, std::ptrdiff_t height)
{
    const auto point_count = gil::estimate_point_count(width, height);
    std::vector<gil::point_t> naive_points(point_count);
    gil::rasterize_line_naive(width, height, naive_points.begin());
    std::vector<gil::point_t> incr_points(point_count);
    gil::rasterize_line_incr(width, height, incr_points.begin());
    std::vector<gil::point_t> bresenham_points(point_count);
    gil::rasterize_line_bresenham(width, height, bresenham_points.begin());

    // BOOST_TEST(std::equal(naive_points.begin(), naive_points.end(), incr_points.begin()));
    // BOOST_TEST(std::equal(naive_points.begin(), naive_points.end(), bresenham_points.begin()));
    if (naive_points != incr_points)
    {
        BOOST_TEST(false);
        std::cerr << "failure of incr_points on width=" << width << " and height=" << height
                  << '\n';
    }

    if (naive_points != bresenham_points)
    {
        BOOST_TEST(false);
        std::cerr << "failure of bresenham_points on width=" << width << " and height=" << height
                  << '\n';
    }
}

void equivalence_test_cases()
{
    const std::ptrdiff_t size = 16;
    for (std::ptrdiff_t height = 1; height < size; ++height)
    {
        for (std::ptrdiff_t width = 1; width < size; ++width)
        {
            equivalence_test(width, height);
        }
    }
}

void line_equation_following_test(std::ptrdiff_t width, std::ptrdiff_t height)
{
    double m = height / width;
    const auto point_count = gil::estimate_point_count(width, height);
    std::vector<gil::point_t> naive_points(point_count);
    gil::rasterize_line_naive(width, height, naive_points.begin());
    std::vector<gil::point_t> incr_points(point_count);
    gil::rasterize_line_incr(width, height, incr_points.begin());
    std::vector<gil::point_t> bresenham_points(point_count);
    gil::rasterize_line_bresenham(width, height, bresenham_points.begin());

    for (std::ptrdiff_t i = 0; i < point_count; ++i)
    {
        gil::point_t expected_point{i, std::llround(m * i)};
        BOOST_TEST(expected_point == naive_points[i]);
        BOOST_TEST(expected_point == incr_points[i]);
        BOOST_TEST(expected_point == bresenham_points[i]);
    }
}

void equation_following_test_cases()
{
    const std::ptrdiff_t size = 16;
    for (std::ptrdiff_t height = 1; height < size; ++height)
    {
        for (std::ptrdiff_t width = 1; width < size; ++width)
        {
            line_equation_following_test(width, height);
        }
    }
}

int main()
{
    equivalence_test_cases();
    equation_following_test_cases();

    return boost::report_errors();
}
