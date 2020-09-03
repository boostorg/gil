#include <algorithm>
#include <boost/core/lightweight_test.hpp>
#include <boost/gil/point.hpp>
#include <boost/gil/rasterization/line.hpp>

#include <cmath>
#include <cstddef>
#include <iostream>
#include <iterator>
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
    BOOST_TEST_EQ(line_points.front(), points.start);
    BOOST_TEST_EQ(line_points.back(), points.end);
}

void test_two_way_equivalence(const line_type& forward, line_type backward)
{
    std::reverse(backward.begin(), backward.end());
    BOOST_TEST_ALL_EQ(forward.begin(), forward.end(), backward.begin(), backward.end());
}

void test_connectivity(line_type const& line_points)
{
    for (std::size_t i = 1; i < line_points.size(); ++i)
    {
        const auto x_diff = std::abs(line_points[i].x - line_points[i - 1].x);
        const auto y_diff = std::abs(line_points[i].y - line_points[i - 1].y);
        BOOST_TEST_LE(x_diff, 1);
        BOOST_TEST_LE(y_diff, 1);
    }
}

void print_endpoints(endpoints points)
{
    std::cout << points.start << " to " << points.end << '\n';
}

void print_line(line_type const& line)
{
    std::copy(line.begin(), line.end(), std::ostream_iterator<gil::point_t>(std::cout, ", "));
    std::cout << '\n';
}

void print_equation(double slope, double intercept)
{
    std::cout << "y=" << slope << "x+" << intercept << '\n';
}

void test_bresenham_rasterizer_follows_equation(const line_type& line_points)
{
    std::cout << line_points.size() << '\n';
    auto start = line_points.front();
    auto end = line_points.back();

    const double sign = [start, end]()
    {
        auto const width_sign = end.x < start.x;
        auto const height_sign = end.y < start.y;
        auto const slope_sign = width_sign != height_sign;
        return slope_sign ? -1 : 1;
    }();
    auto width = std::abs(end.x - start.x);
    auto height = std::abs(end.y - start.y);
    const double slope = static_cast<double>(height) / static_cast<double>(width);
    const double intercept =
        static_cast<double>(start.y) - sign * slope * static_cast<double>(start.x);
    print_line(line_points);
    print_equation(slope, intercept);
    print_endpoints({start, end});
    for (const auto& point : line_points)
    {
        double const expected_y = sign * slope * static_cast<double>(point.x) + intercept;
        // BOOST_TEST_LE(std::abs(response), error_tolerance);
        // BOOST_TEST(std::abs(response) <= error_tolerance);
        auto const difference =
            std::abs(point.y - static_cast<std::ptrdiff_t>(std::round(expected_y)));
        BOOST_TEST_LE(difference, static_cast<std::ptrdiff_t>(slope + 1));
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
    for (std::size_t seed = 0; seed < 1; ++seed)
    {
        std::mt19937 twister(seed);
        std::uniform_int_distribution<std::ptrdiff_t> distr(0, size - 1);
        const std::size_t sample_count = 100;
        for (std::size_t sample_index = 0; sample_index < sample_count; ++sample_index)
        {
            std::cout << "=============================================\n";
            auto endpoints = create_endpoints(twister, distr);
            auto forward_line = create_line(endpoints);
            test_start_end(forward_line, endpoints);
            auto backward_line = create_line({endpoints.end, endpoints.start});
            test_two_way_equivalence(forward_line, backward_line);
            test_connectivity(forward_line);
            // test_connectivity(backward_line);
            test_bresenham_rasterizer_follows_equation(forward_line);
            // test_bresenham_rasterizer_follows_equation(backward_line);
        }
    }

    return boost::report_errors();
}
