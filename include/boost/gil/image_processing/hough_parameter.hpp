#include <boost/gil/point.hpp>
#include <cmath>
#include <cstddef>

namespace boost { namespace gil {
template <typename T>
struct hough_parameter
{
    T start_point;
    T step_size;
    std::size_t step_count;

    static hough_parameter<T> from_step_count(T start_point, T neighborhood,
                                              std::size_t half_step_count)
    {
        T step_size = neighborhood / half_step_count;
        std::size_t step_count = half_step_count * 2 + 1;
        return {start_point - neighborhood, step_size, step_count};
    }

    static hough_parameter<T> from_step_size(T start_point, T neighborhood, T step_size)
    {
        std::size_t step_count =
            2 * static_cast<std::size_t>(std::floor(neighborhood / step_size)) + 1;
        // do not use step_size - neighborhood, as step_size might not allow
        // landing exactly on that value when starting from start_point
        // also use parentheses on step_count / 2 because flooring is exactly
        // what we want
        return {start_point - step_size * (step_count / 2), step_size, step_count};
    }
};

inline double minimum_angle_step(point_t dimensions)
{
    auto longer_dimension = dimensions.x > dimensions.y ? dimensions.x : dimensions.y;
    return std::atan2(1, longer_dimension);
}

inline hough_parameter<double> make_theta_parameter(double approx_angle, double neighborhood,
                                                    point_t dimensions)
{
    auto angle_step = minimum_angle_step(dimensions);

    // std::size_t step_count =
    //     2 * static_cast<std::size_t>(std::floor(neighborhood / angle_step)) + 1;
    // return {approx_angle - angle_step * (step_count / 2), angle_step, step_count};
    return hough_parameter<double>::from_step_size(approx_angle, neighborhood, angle_step);
}
}} // namespace boost::gil
