// Boost.GIL (Generic Image Library) - tests
//
// Copyright 2020 Olzhas Zhumabek <anonymous.from.applecity@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/gil/point.hpp>
#include <cmath>
#include <cstddef>

namespace boost { namespace gil {
/// \defgroup Rasterization
/// \brief A set of functions to rasterize shapes
///
/// Due to images being discrete, most shapes require specialized algorithms to handle rasterization
/// efficiently and solve problem of connectivity and being close to the original shape.

/// \defgroup LineRasterization
/// \ingroup Rasterization
/// \brief A set of rasterizers for lines
///
/// The main problem with line rasterization is to do it efficiently, e.g. less floating point
/// operations. There are multiple algorithms that on paper should reach the same result, but
/// due to quirks of IEEE-754 they don't. Please select one and stick to it if possible. At
/// the moment only Bresenham rasterizer is implemented.

/// \ingroup LineRasterization
/// \brief Rasterize a line according to Bresenham algorithm
///
/// Do note that if either width or height is 1, slope is set to zero.
/// reference:
/// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm#:~:text=Bresenham's%20line%20algorithm%20is%20a,straight%20line%20between%20two%20points.
struct bresenham_line_rasterizer
{
    constexpr std::ptrdiff_t point_count(std::ptrdiff_t width, std::ptrdiff_t height) const noexcept
    {
        return width > height ? width : height;
    }

    template <typename RandomAccessIterator>
    void operator()(std::ptrdiff_t width, std::ptrdiff_t height,
                    RandomAccessIterator output_first) const
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
