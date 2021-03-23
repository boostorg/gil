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
/// \brief Performs ellipse rasterization using midpoint algorithm.
struct midpoint_elliptical_rasterizer
{
    /// \brief Returns a vector containing co-ordinates of first quadrant points which lie on
    /// rasterizer trajectory of the ellipse.
    /// \param semi_axes - Array containing half of lengths of horizontal and vertical axis respectively.
    std::vector<std::array<std::ptrdiff_t, 2>> obtain_trajectory(
                                                    const std::array<unsigned int, 2> semi_axes)
    {
        // Citation : J. Van Aken, "An Efficient Ellipse-Drawing Algorithm" in IEEE Computer Graphics and Applications, vol. 4, no. 09, pp. 24-35, 1984.
        // doi: 10.1109/MCG.1984.275994
        // keywords: {null}
        // url: https://doi.ieeecomputersociety.org/10.1109/MCG.1984.275994
        std::vector<std::array<std::ptrdiff_t, 2>> trajectory_points;
        std::ptrdiff_t x = semi_axes[0], y = 0;
        
        // Variables declared on the following line are temporary variables used for improving 
        // performance since they help in converting all multiplicative operations inside the loop
        // in to additive/subtractive operations.
        long long int t1, t2, t3, t4, t5, t6, t7, t8, t9;
        t1 = semi_axes[0] * semi_axes[0], t2 = 2 * t1, t3 = 2 * t2;
        t4 = semi_axes[1] * semi_axes[1], t5 = 2 * t4, t6 = 2 * t5;
        t7 = semi_axes[0] * t5, t8 = 2 * t7, t9 = 0;

        // Following variables serve as decision parameters and help in choosing the right point
        // to be included in rasterizer trajectory.
        long long int d1, d2;
        d1 = t2 - t7 + t4 / 2, d2 = t1 / 2 - t8 + t5;
        
        while(d2 < 0)
        {
            trajectory_points.push_back({x, y});
            y += 1;
            t9 += t3;
            if(d1 < 0)
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
        while(x >= 0)
        {
            trajectory_points.push_back({x, y});
            x -= 1;
            t8 -= t6;
            if(d2 < 0)
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

    /// \brief Helps in drawing ellipse by filling desired colours at pixel co-ordinates returned 
    /// by function 'obtain_trajectory'.
    /// \param view - Gil view of image which will contain the elliptical curve after it is drawn.
    /// \param colour - Constant vector specifying colour intensity values for all channels present
    ///                 in 'view'.
    /// \param center - Constant array specifying co-ordinates of center of ellipse to be drawn.
    /// \param trajectory_points - Constant vector specifying pixel co-ordinates of points lying 
    ///                            on rasterizer trajectory.
    /// \tparam View - Type of input image view.
    template<typename View>
    void draw_curve(View view, const std::vector<unsigned int> colour,
    const std::array<unsigned int, 2> center,
    const std::vector<std::array<std::ptrdiff_t, 2>> trajectory_points)
    {
        for(std::size_t i = 0, colour_index = 0;i < view.num_channels(); ++i, ++colour_index)
        {
            for(std::array<std::ptrdiff_t, 2> point : trajectory_points)
            {
                std::array<std::ptrdiff_t, 4> co_ords = {center[0] + point[0],
                center[0] - point[0], center[1] + point[1], center[1] - point[1]
                };
                bool validity[4] = {0};
                if(co_ords[0] < view.width())
                {
                    validity[0] = 1;
                }
                if(co_ords[1] >= 0 && co_ords[1] < view.width())
                {
                    validity[1] = 1;
                }
                if(co_ords[2] < view.height())
                {
                    validity[2] = 1;
                }
                if(co_ords[3] >= 0 && co_ords[3] < view.height())
                {
                    validity[3] = 1;
                }
                if(validity[0] && validity[2])
                {
                    nth_channel_view(view, i)(co_ords[0], co_ords[2]) = colour[colour_index];
                }
                if(validity[1] && validity[2])
                {
                    nth_channel_view(view, i)(co_ords[1], co_ords[2]) = colour[colour_index];
                }
                if(validity[1] && validity[3])
                {
                    nth_channel_view(view, i)(co_ords[1], co_ords[3]) = colour[colour_index];
                }
                if(validity[0] && validity[3])
                {
                    nth_channel_view(view, i)(co_ords[0], co_ords[3]) = colour[colour_index];
                }
            }
        }
    }

    /// \brief Calls the function 'obtain_trajectory' and then passes obtained trajectory points
    ///        in the function 'draw_curve' for drawing the desired ellipse.
    /// \param view - Gil view of image which will contain the elliptical curve after it is drawn.
    /// \param colour - Constant vector specifying colour intensity values for all channels present
    ///                 in 'view'.
    /// \param center - Array specifying co-ordinates of center of ellipse to be drawn.
    /// \param semi_axes - Array containing lengths of semi-major axis and semi-minor axis respectively.                
    /// \tparam View - Type of input image view.
    template<typename View>
    void operator()(View view, const std::vector<unsigned int> colour,
            std::array<unsigned int, 2> center, const std::array<unsigned int, 2> semi_axes)
    {
        --center[0], --center[1]; // For converting center co-ordinate values to zero based indexing.
        if(colour.size() != view.num_channels())
        {
            throw std::length_error("Number of channels in given image is not equal to the "
                                                                "number of colours provided.");
        }
        if(center[0] + semi_axes[0] >= view.width() || center[1] + semi_axes[1] >= view.height()
            || static_cast<int>(center[0] - semi_axes[0]) < 0 
            || static_cast<int>(center[0] - semi_axes[0]) >= view.width()
            || static_cast<int>(center[1] - semi_axes[1]) < 0
            || static_cast<int>(center[1] - semi_axes[1]) >= view.height())
        {
            std::cout << "Image can't contain whole curve.\n"
            "However, it will contain those parts of curve which can fit inside it.\n";
            std::cout << "Note : Image width = " << view.width() << " and Image height = "
                                                                          << view.height() << "\n";
        }
        std::vector<std::array<std::ptrdiff_t, 2>> trajectory_points = 
                                                              obtain_trajectory(semi_axes);
        draw_curve(view, colour, center, trajectory_points);
    }
}; // midpoint elliptical rasterizer
}} // namespace boost::gil
#endif