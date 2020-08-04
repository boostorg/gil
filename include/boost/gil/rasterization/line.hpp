#include <boost/gil/point.hpp>
#include <cmath>
#include <cstddef>

namespace boost { namespace gil {
inline std::ptrdiff_t estimate_point_count(std::ptrdiff_t width, std::ptrdiff_t height)
{
    return width > height ? width : height;
}

template <typename RAIterator>
void rasterize_horizontal_line(RAIterator first, RAIterator last)
{
    std::ptrdiff_t x = 0;
    for (; first != last; ++first)
    {
        *first = point_t{x, 0};
        ++x;
    }
}

template <typename RAIterator>
void rasterize_vertical_line(RAIterator first, RAIterator last)
{
    std::ptrdiff_t y = 0;
    for (; first != last; ++first)
    {
        *first = point_t{0, y};
        ++y;
    }
}

template <typename RAIterator>
void rasterize_line_naive(std::ptrdiff_t width, std::ptrdiff_t height, RAIterator output_first)
{
    const auto point_count = estimate_point_count(width, height);
    // walk over longer axis, swap coordinates later
    const bool needs_flip = point_count != width;
    // m is guaranteed to be <= 1
    const double m = [needs_flip, width, height]() {
        if (width == 1 || height == 1)
        {
            return 0.0;
        }

        return needs_flip ? static_cast<double>(width) / height
                          : static_cast<double>(height) / width;
    }();
    *output_first++ = point_t(0, 0);
    for (std::ptrdiff_t x = 1; x < point_count; ++x)
    {
        double y = m * x;
        auto discrete_y = static_cast<std::ptrdiff_t>(std::round(y));
        auto next_point = needs_flip ? point_t{discrete_y, x} : point_t{x, discrete_y};
        *output_first++ = next_point;
    }
}

template <typename RAIterator>
void rasterize_line_incr(std::ptrdiff_t width, std::ptrdiff_t height, RAIterator output_first)
{
    const auto point_count = estimate_point_count(width, height);
    // walk over longer axis, swap coordinates later
    const bool needs_flip = point_count != width;
    // m is guaranteed to be <= 1
    const double m = [needs_flip, width, height]() {
        if (width == 1 || height == 1)
        {
            return 0.0;
        }

        return needs_flip ? static_cast<double>(width) / height
                          : static_cast<double>(height) / width;
    }();
    double y = 0;
    *output_first++ = point_t(0, 0);
    for (std::ptrdiff_t x = 1; x < point_count; ++x)
    {
        y += m;
        auto discrete_y = static_cast<std::ptrdiff_t>(std::round(y));
        auto next_point = needs_flip ? point_t{discrete_y, x} : point_t{x, discrete_y};
        *output_first++ = next_point;
    }
}

template <typename RAIterator>
void rasterize_line_bresenham(std::ptrdiff_t width, std::ptrdiff_t height, RAIterator output_first)
{
    const auto point_count = estimate_point_count(width, height);
    const bool needs_flip = point_count != width;
    const double m = [needs_flip, width, height]() {
        if (width == 1 || height == 1)
        {
            return 0.0;
        }

        return needs_flip ? static_cast<double>(width) / height
                          : static_cast<double>(height) / width;
    }();
    double error_term = -1;
    std::ptrdiff_t y = 0;
    *output_first++ = point_t(0, 0);
    for (std::ptrdiff_t x = 1; x < point_count; ++x)
    {
        error_term += m;
        if (error_term > 0.5)
        {
            --error_term;
            ++y;
        }
        auto next_point = needs_flip ? point_t{y, x} : point_t{x, y};
        *output_first++ = next_point;
    }
}

}} // namespace boost::gil
