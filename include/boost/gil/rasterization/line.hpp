#include <boost/gil/point.hpp>
#include <cmath>
#include <cstddef>

namespace boost { namespace gil {
struct bresenham_line_rasterizer
{
    constexpr std::ptrdiff_t point_count(std::ptrdiff_t width, std::ptrdiff_t height) const noexcept
    {
        return width > height ? width : height;
    }

    template <typename RAIterator>
    void operator()(std::ptrdiff_t width, std::ptrdiff_t height, RAIterator output_first) const
    {
        const auto target_count = point_count(width, height);
        const bool needs_flip = target_count != width;
        const double m = [needs_flip, width, height]() {
            if (width == 1 || height == 1)
            {
                return 0.0;
            }

            return needs_flip ? static_cast<double>(width) / height
                              : static_cast<double>(height) / width;
        }();
        double error_term = 0;
        std::ptrdiff_t y = 0;
        *output_first++ = point_t(0, 0);
        for (std::ptrdiff_t x = 1; x < target_count; ++x)
        {
            error_term += m;
            if (error_term >= 0.5)
            {
                --error_term;
                ++y;
            }
            auto next_point = needs_flip ? point_t{y, x} : point_t{x, y};
            *output_first++ = next_point;
        }
    }
};

}} // namespace boost::gil
