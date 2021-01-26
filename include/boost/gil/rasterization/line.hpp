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

namespace boost
{
namespace gil
{
/// \defgroup Rasterization
/// \brief A set of functions to rasterize shapes
///
/// Due to images being discrete, most shapes require specialized algorithms to
/// handle rasterization efficiently and solve problem of connectivity and being
/// close to the original shape.

/// \defgroup LineRasterization
/// \ingroup Rasterization
/// \brief A set of rasterizers for lines
///
/// The main problem with line rasterization is to do it efficiently, e.g. less
/// floating point operations. There are multiple algorithms that on paper
/// should reach the same result, but due to quirks of IEEE-754 they don't.
/// Please select one and stick to it if possible. At the moment only Bresenham
/// rasterizer is implemented.

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

    std::ptrdiff_t point_count(point_t start, point_t end) const noexcept
    {
        const auto abs_width = std::abs(end.x - start.x) + 1;
        const auto abs_height = std::abs(end.y - start.y) + 1;
        return point_count(abs_width, abs_height);
    }

    template <typename RandomAccessIterator>
    void operator()(point_t start, point_t end, RandomAccessIterator d_first) const
    {
        if (start == end)
        {
            // put the point and immediately exit, as later on division by zero will
            // occur
            *d_first = start;
            return;
        }

        auto width = std::abs(end.x - start.x) + 1;
        auto height = std::abs(end.y - start.y) + 1;
        bool const needs_flip = width < height;
        if (needs_flip)
        {
            // transpose the coordinate system if uncomfortable angle detected
            std::swap(width, height);
            std::swap(start.x, start.y);
            std::swap(end.x, end.y);
        }
        std::ptrdiff_t const x_increment = end.x >= start.x ? 1 : -1;
        std::ptrdiff_t const y_increment = end.y >= start.y ? 1 : -1;
        double const slope =
            height == 1 ? 0 : static_cast<double>(height) / static_cast<double>(width);
        std::ptrdiff_t y = start.y;
        double error_term = 0;
        for (std::ptrdiff_t x = start.x; x != end.x; x += x_increment)
        {
            // transpose coordinate system back to proper form if needed
            *d_first++ = needs_flip ? point_t{y, x} : point_t{x, y};
            error_term += slope;
            if (error_term >= 0.5)
            {
                --error_term;
                y += y_increment;
            }
        }
        *d_first++ = needs_flip ? point_t{end.y, end.x} : end;
    }
};

}} // namespace boost::gil
