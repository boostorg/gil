#include <algorithm>
#include <boost/core/lightweight_test.hpp>
#include <boost/gil/point.hpp>
#include <boost/gil/rasterization/line.hpp>

#include <cmath>
#include <iostream>
#include <random>
#include <vector>

namespace gil = boost::gil;

namespace boost
{
namespace gil
{
std::ostream& operator<<(std::ostream& os, const point_t p)
{
    os << "{x=" << p.x << ", y=" << p.y << "}";
    return os;
}
}} // namespace boost::gil

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

using line_type = std::vector<gil::point_t>;

struct endpoints
{
    gil::point_t start;
    gil::point_t end;
};

endpoints create_endpoints(std::mt19937& twister,
                           std::uniform_int_distribution<std::ptrdiff_t>& distr)
{
    gil::point_t start{distr(twister), distr(twister)};
    gil::point_t end{distr(twister), distr(twister)};
    return {start, end};
}

line_type create_line(endpoints points)
{
    gil::bresenham_line_rasterizer rasterizer;
    line_type forward_line(rasterizer.point_count(points.start, points.end));
    rasterizer(points.start, points.end, forward_line.begin());
    return forward_line;
}

void test_start_end(const line_type& line_points, endpoints points)
{
    BOOST_TEST(line_points.front() == points.start);
    BOOST_TEST(line_points.back() == points.end);
}

void test_two_way_equivalence(const line_type& forward, line_type backward)
{
    std::reverse(backward.begin(), backward.end());
    BOOST_TEST_ALL_EQ(forward.begin(), forward.end(), backward.begin(), backward.end());
}

void test_connectivity(const line_type& line_points)
{
    for (std::size_t i = 1; i < line_points.size(); ++i)
    {
        const auto x_diff = std::abs(line_points[i].x - line_points[i - 1].x);
        const auto y_diff = std::abs(line_points[i].y - line_points[i - 1].y);
        BOOST_TEST_LE(x_diff, 1);
        BOOST_TEST_LE(y_diff, 1);
    }
}

void test_bresenham_rasterizer_follows_equation(const line_type& line_points)
{
    const auto start = line_points.front();
    const auto end = line_points.back();

    const auto width = end.x - start.x;
    const auto height = end.y - start.y;
    const double slope = static_cast<double>(height) / static_cast<double>(width);
    const std::ptrdiff_t intercept = static_cast<std::ptrdiff_t>(
        std::round(static_cast<double>(start.y) - slope * static_cast<double>(start.x)));
    const std::ptrdiff_t error_tolerance = 4; // one point off is okay
    for (const auto& point : line_points)
    {
        double response = point.x * slope + intercept - point.y;
        // BOOST_TEST_LE(std::abs(response), error_tolerance);
        BOOST_TEST(std::abs(response) <= error_tolerance || std::isnan(response));
    }
}

int main()
{
    const std::ptrdiff_t size = 256;
    // for (std::ptrdiff_t width = 1; width < size; ++width)
    // {
    //     for (std::ptrdiff_t height = 1; height <= width; ++height)
    //     {
    //         test_bresenham_rasterizer_follows_equation(width, width);
    //     }
    // }
    for (std::size_t seed = 0; seed < 101; ++seed)
    {
        std::mt19937 twister(seed);
        std::uniform_int_distribution<std::ptrdiff_t> distr(0, size - 1);
        const std::size_t sample_count = 100;
        for (std::size_t sample_index = 0; sample_index < sample_count; ++sample_index)
        {
            auto endpoints = create_endpoints(twister, distr);
            auto forward_line = create_line(endpoints);
            test_start_end(forward_line, endpoints);
            auto backward_line = create_line({endpoints.end, endpoints.start});
            test_two_way_equivalence(forward_line, backward_line);
            test_connectivity(forward_line);
            test_connectivity(backward_line);
            test_bresenham_rasterizer_follows_equation(forward_line);
            test_bresenham_rasterizer_follows_equation(backward_line);
        }
    }

    return boost::report_errors();
}
