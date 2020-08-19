#include <boost/gil/detail/math.hpp>
#include <boost/gil/point.hpp>
#include <cmath>
#include <cstddef>

namespace boost { namespace gil {

struct trigonometric_circle_rasterizer
{
    std::ptrdiff_t point_count(std::ptrdiff_t radius) const noexcept
    {
        const auto diameter = radius * 2 - 1;
        const double minimum_angle_step = std::atan2(1.0, diameter);
        return 8 * static_cast<std::ptrdiff_t>(std::round(detail::pi / 4 / minimum_angle_step) + 1);
    }

    template <typename RAIterator>
    void operator()(std::ptrdiff_t radius, point_t offset, RAIterator d_first) const
    {
        const double minimum_angle_step = std::atan2(1.0, radius);
        auto translate_mirror_points = [&d_first, offset](point_t p) {
            std::ptrdiff_t xpos = offset.x + p.x;
            std::ptrdiff_t xneg = offset.x - p.x;
            std::ptrdiff_t ypos = offset.y + p.y;
            std::ptrdiff_t yneg = offset.y - p.y;
            *d_first++ = point_t{xpos, ypos};
            *d_first++ = point_t{xpos, yneg};
            *d_first++ = point_t{xneg, ypos};
            *d_first++ = point_t{xneg, yneg};
            *d_first++ = point_t{ypos, xpos};
            *d_first++ = point_t{ypos, xneg};
            *d_first++ = point_t{yneg, xpos};
            *d_first++ = point_t{yneg, xneg};
        };
        const std::ptrdiff_t iteration_count = point_count(radius) / 8;
        double angle = 0;
        // do note that + 1 was done inside count estimation, thus <= is not needed, only <
        for (std::ptrdiff_t i = 0; i < iteration_count; ++i, angle += minimum_angle_step)
        {
            std::ptrdiff_t x = static_cast<std::ptrdiff_t>(std::round(radius * std::cos(angle)));
            std::ptrdiff_t y = static_cast<std::ptrdiff_t>(std::round(radius * std::sin(angle)));
            translate_mirror_points({x, y});
        }
    }
};

struct midpoint_circle_rasterizer
{
    std::ptrdiff_t point_count(std::ptrdiff_t radius) const noexcept
    {
        // the reason for pulling 8 out is so that when the expression radius * cos(45 degrees)
        // is used, it would yield the same result as here
        // + 1 at the end is because the point at radius itself is computed as well
        return 8 * static_cast<std::ptrdiff_t>(
                       std::round(radius * std::cos(boost::gil::detail::pi / 4)) + 1);
    }

    template <typename RAIterator>
    void operator()(std::ptrdiff_t radius, point_t offset, RAIterator d_first) const
    {
        auto translate_mirror_points = [&d_first, offset](point_t p) {
            std::ptrdiff_t xpos = offset.x + p.x;
            std::ptrdiff_t xneg = offset.x - p.x;
            std::ptrdiff_t ypos = offset.y + p.y;
            std::ptrdiff_t yneg = offset.y - p.y;
            *d_first++ = point_t{xpos, ypos};
            *d_first++ = point_t{xpos, yneg};
            *d_first++ = point_t{xneg, ypos};
            *d_first++ = point_t{xneg, yneg};
            *d_first++ = point_t{ypos, xpos};
            *d_first++ = point_t{ypos, xneg};
            *d_first++ = point_t{yneg, xpos};
            *d_first++ = point_t{yneg, xneg};
        };
        std::ptrdiff_t iteration_distance = point_count(radius) / 8;
        std::ptrdiff_t y_current = radius;
        std::ptrdiff_t r_squared = radius * radius;
        translate_mirror_points({0, y_current});
        for (std::ptrdiff_t x = 1; x < iteration_distance; ++x)
        {
            std::ptrdiff_t midpoint = x * x + y_current * y_current - y_current - r_squared;
            if (midpoint > 0)
            {
                --y_current;
            }
            translate_mirror_points({x, y_current});
        }
    }
};
}} // namespace boost::gil
