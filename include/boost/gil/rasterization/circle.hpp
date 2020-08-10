#include <boost/gil/detail/math.hpp>
#include <boost/gil/point.hpp>
#include <cmath>
#include <cstddef>

namespace boost { namespace gil {

inline std::size_t estimate_circle_point_count(std::size_t radius)
{
    // the reason for pulling 8 out is so that when the expression radius * cos(45 degrees)
    // is used, it would yield the same result as here
    // + 1 at the end is because the point at radius itself is computed as well
    return 8 *
           static_cast<std::size_t>(std::round(radius * std::cos(boost::gil::detail::pi / 4)) + 1);
}

template <typename RAIterator>
void rasterize_circle_midpoint(std::ptrdiff_t radius, point_t offset, RAIterator d_first)
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
    std::ptrdiff_t iteration_distance =
        static_cast<std::ptrdiff_t>(std::round(radius * std::cos(boost::gil::detail::pi / 4)));
    std::ptrdiff_t y_current = radius;
    double r_squared = static_cast<double>(radius * radius);
    translate_mirror_points({0, y_current});
    for (std::ptrdiff_t x = 1; x <= iteration_distance; ++x)
    {
        auto y = y_current - 0.5;
        double midpoint = x * x + y * y - r_squared;
        if (midpoint >= 0.0)
        {
            --y_current;
        }
        translate_mirror_points({x, y_current});
    }
}
}} // namespace boost::gil
