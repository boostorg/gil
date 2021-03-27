//
// Copyright 2021 Prathamesh Tagore <prathameshtagore@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_GIL_RASTERIZATION_ELLIPSE_HPP
#define BOOST_GIL_RASTERIZATION_ELLIPSE_HPP

#include <array>
#include <vector>
#include <iostream>

namespace boost { namespace gil {

/// \defgroup EllipseRasterization
/// \ingroup Rasterization
/// \brief Ellipse rasterization algorithms.

/// \ingroup EllipseRasterization
/// \brief Performs ellipse rasterization using midpoint algorithm. Initially, program considers
/// origin as center of ellipse and obtains first quadrant trajectory points. After that,
/// it shifts origin to provided co-ordinates of center and then draws the curve.
struct midpoint_elliptical_rasterizer
{
    /// \brief Returns a vector containing co-ordinates of first quadrant points which lie on
    /// rasterizer trajectory of the ellipse.
    /// \param semi_axes - Array containing half of lengths of horizontal and vertical axis
    /// respectively.
    auto obtain_trajectory(std::array<unsigned int, 2> const semi_axes)
        -> std::vector<std::array<std::ptrdiff_t, 2>>
    {
        // Citation : J. Van Aken, "An Efficient Ellipse-Drawing Algorithm" in IEEE Computer
        // Graphics and Applications, vol. 4, no. 09, pp. 24-35, 1984.
        // doi: 10.1109/MCG.1984.275994
        // keywords: {null}
        // url: https://doi.ieeecomputersociety.org/10.1109/MCG.1984.275994
        std::vector<std::array<std::ptrdiff_t, 2>> trajectory_points;
        std::ptrdiff_t x = semi_axes[0], y = 0;
        
        // Variables declared on following lines are temporary variables used for improving 
        // performance since they help in converting all multiplicative operations inside the while
        // loop into additive/subtractive operations.
        long long int const t1 = semi_axes[0] * semi_axes[0];
        long long int const t4 = semi_axes[1] * semi_axes[1];
        long long int t2, t3, t5, t6, t8, t9;
        t2 = 2 * t1, t3 = 2 * t2;
        t5 = 2 * t4, t6 = 2 * t5;
        long long int const t7 = semi_axes[0] * t5;
        t8 = 2 * t7, t9 = 0;

        // Following variables serve as decision parameters and help in choosing the right point
        // to be included in rasterizer trajectory.
        long long int d1, d2;
        d1 = t2 - t7 + t4 / 2, d2 = t1 / 2 - t8 + t5;
        
        while (d2 < 0)
        {
            trajectory_points.push_back({x, y});
            y += 1;
            t9 += t3;
            if (d1 < 0)
            {
                d1 += t9 + t2;
                d2 += t9;
            }
            else 
            {
                x -= 1;
                t8 -= t6;
                d1 += t9 + t2 - t8;
                d2 += t5 + t9 - t8;
            }
        }
        while (x >= 0)
        {
            trajectory_points.push_back({x, y});
            x -= 1;
            t8 -= t6;
            if (d2 < 0)
            {
                y += 1;
                t9 += t3;
                d2 += t5 + t9 - t8;
            }
            else 
            {
                d2 += t5 - t8;
            }
        }
        return trajectory_points;
    }

    /// \brief Fills pixels returned by function 'obtain_trajectory' as well as pixels
    /// obtained from their reflection along major axis, minor axis and line passing through
    /// center with slope -1 using colours provided by user.
    /// \param view - Gil view of image on which the elliptical curve is to be drawn.
    /// \param colour - Constant vector specifying colour intensity values for all channels present
    ///                 in 'view'.
    /// \param center - Constant array specifying co-ordinates of center of ellipse to be drawn.
    /// \param trajectory_points - Constant vector specifying pixel co-ordinates of points lying 
    ///                            on rasterizer trajectory.
    /// \tparam View - Type of input image view.
    template<typename View>
    void draw_curve(View view, std::vector<unsigned int> const colour,
        std::array<unsigned int, 2> const center,
        std::vector<std::array<std::ptrdiff_t, 2>> const trajectory_points)
    {
        for (int i = 0, colour_index = 0; i < static_cast<int>(view.num_channels()); 
            ++i, ++colour_index)
        {
            for (std::array<std::ptrdiff_t, 2> point : trajectory_points)
            {
                std::array<std::ptrdiff_t, 4> co_ords = {center[0] + point[0],
                center[0] - point[0], center[1] + point[1], center[1] - point[1]
                };
                bool validity[4] = {0};
                if (co_ords[0] < view.width())
                {
                    validity[0] = 1;
                }
                if (co_ords[1] >= 0 && co_ords[1] < view.width())
                {
                    validity[1] = 1;
                }
                if (co_ords[2] < view.height())
                {
                    validity[2] = 1;
                }
                if (co_ords[3] >= 0 && co_ords[3] < view.height())
                {
                    validity[3] = 1;
                }
                if (validity[0] && validity[2])
                {
                    nth_channel_view(view, i)(co_ords[0], co_ords[2])[0] = colour[colour_index];
                }
                if (validity[1] && validity[2])
                {
                    nth_channel_view(view, i)(co_ords[1], co_ords[2])[0] = colour[colour_index];
                }
                if (validity[1] && validity[3])
                {
                    nth_channel_view(view, i)(co_ords[1], co_ords[3])[0] = colour[colour_index];
                }
                if (validity[0] && validity[3])
                {
                    nth_channel_view(view, i)(co_ords[0], co_ords[3])[0] = colour[colour_index];
                }
            }
        }
    }

    /// \brief Calls the function 'obtain_trajectory' and then passes obtained trajectory points
    ///        in the function 'draw_curve' for drawing the desired ellipse.
    /// \param view - Gil view of image on which the elliptical curve is to be drawn.
    /// \param colour - Constant vector specifying colour intensity values for all channels present
    ///                 in 'view'.
    /// \param center - Array containing positive integer x co-ordinate and y co-ordinate of the
    /// center respectively.
    /// \param semi_axes - Array containing positive integer lengths of horizontal semi-axis
    /// and vertical semi-axis respectively.
    /// \tparam View - Type of input image view.
    template<typename View>
    void operator()(View view, std::vector<unsigned int> const colour,
        std::array<unsigned int, 2> center, std::array<unsigned int, 2> const semi_axes)
    {
        --center[0], --center[1]; // For converting center co-ordinate values to zero based indexing.
        if (colour.size() != view.num_channels())
        {
            throw std::length_error("Number of channels in given image is not equal to the "
                "number of colours provided.");
        }
        if (center[0] + semi_axes[0] >= view.width() || center[1] + semi_axes[1] >= view.height()
            || static_cast<int>(center[0] - semi_axes[0]) < 0 
            || static_cast<int>(center[0] - semi_axes[0]) >= view.width()
            || static_cast<int>(center[1] - semi_axes[1]) < 0
            || static_cast<int>(center[1] - semi_axes[1]) >= view.height())
        {
            std::cout << "Image can't contain whole curve.\n"
                "However, it will contain those parts of curve which can fit inside it.\n"
                "Note : Image width = " << view.width() << " and Image height = " << 
                view.height() << "\n";
        }
        std::vector<std::array<std::ptrdiff_t, 2>> trajectory_points = 
            obtain_trajectory(semi_axes);
        draw_curve(view, colour, center, trajectory_points);
    }
}; // midpoint elliptical rasterizer
}} // namespace boost::gil
#endif
