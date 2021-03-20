#ifndef BOOST_GIL_RASTERIZATION_ELLIPSE_HPP
#define BOOST_GIL_RASTERIZATION_ELLIPSE_HPP

#include <cmath>
#include <array>
#include <vector>
#include <iostream>

namespace boost { namespace gil {

template<typename View>
void draw_ellipse_impl(View view, const std::array<unsigned int, 3> colour,
 const std::array<unsigned int, 2> center, const int horizontal_axis_length,
 const int vertical_axis_length)
{
    static unsigned int colour_index;
    static std::vector<std::array<std::ptrdiff_t, 2>> trajectory_points;
    if(trajectory_points.size() == 0)
    {
        
        std::ptrdiff_t x = horizontal_axis_length, y = 0;
        long long int t1, t2, t3, t4, t5, t6, t7, t8, t9, d1, d2;
        t1 = horizontal_axis_length * horizontal_axis_length, t2 = 2 * t1, t3 = 2 * t2;
        t4 = vertical_axis_length * vertical_axis_length, t5 = 2 * t4, t6 = 2 * t5;
        t7 = horizontal_axis_length * t5, t8 = 2 * t7, t9 = 0;
        d1 = t2 - t7 + t4 / 2, d2 = t1 / 2 - t8 + t5;
        while(d2 < 0)
        {
            std::cout << "I am here\n";
            std::cout << x << "  " << y << "  x  and  y\n";
            view(center[0] + x, center[1] + y) = view(center[0] - x, center[1] + y) = 
            view(center[0] - x, center[1] - y) = view(center[0] + x, center[1] - y) = 
            colour[colour_index];
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
            std::cout << x << "  " << y << "  x  and  y\n";
            view(center[0] + x, center[1] + y) = view(center[0] - x, center[1] + y) = 
            view(center[0] - x, center[1] - y) = view(center[0] + x, center[1] - y) = 
            colour[colour_index];
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
    }
    else
    {
        for (auto point : trajectory_points)
        {
            view(center[0] + point[0], center[1] + point[1]) =
            view(center[0] - point[0], center[1] + point[1]) = 
            view(center[0] - point[0], center[1] - point[1]) = 
            view(center[0] + point[0], center[1] - point[1]) = 
            colour[colour_index];
        }
    }
    ++colour_index;
}

template<typename View>
void draw_ellipse(View view, const std::array<unsigned int, 3> colour,
 const std::array<unsigned int, 2> center, const int horizontal_axis_length,
 const int vertical_axis_length)
{
    // Include checks
    for(std::size_t i = 0;i < view.num_channels(); ++i)
    {
        draw_ellipse_impl(nth_channel_view(view, i), colour, center, 2 * horizontal_axis_length ,
                                                                         2 * vertical_axis_length);
    }
}

}} // namespace boost::gil
#endif