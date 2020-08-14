#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_processing/hough_transform.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/typedefs.hpp>

#include <iostream>
#include <limits>
#include <vector>

namespace gil = boost::gil;

int main()
{
    const std::size_t size = 128;
    gil::gray8_image_t input_image(size, size);
    auto input = gil::view(input_image);

    const std::ptrdiff_t circle_radius = 16;
    const gil::point_t circle_center = {64, 64};
    std::vector<gil::point_t> circle_points(gil::estimate_circle_point_count(circle_radius));
    gil::rasterize_circle_midpoint(circle_radius, circle_center, circle_points.begin());
    for (const auto& point : circle_points)
    {
        input(point) = std::numeric_limits<gil::uint8_t>::max();
    }

    const auto radius_parameter =
        gil::hough_parameter<std::ptrdiff_t>::from_step_count(circle_radius, 3, 3);
    const auto x_parameter =
        gil::hough_parameter<std::ptrdiff_t>::from_step_count(circle_center.x, 3, 3);
    const auto y_parameter =
        gil::hough_parameter<std::ptrdiff_t>::from_step_count(circle_center.x, 3, 3);

    std::vector<gil::gray16_image_t> parameter_space_images(
        radius_parameter.step_count,
        gil::gray16_image_t(x_parameter.step_count, y_parameter.step_count));
    std::vector<gil::gray16_view_t> parameter_space_views(parameter_space_images.size());
    std::transform(parameter_space_images.begin(), parameter_space_images.end(),
                   parameter_space_views.begin(),
                   [](gil::gray16_image_t& img) { return gil::view(img); });

    gil::hough_circle_transform_brute(input, radius_parameter, x_parameter, y_parameter,
                                      parameter_space_views.begin());
    std::cout << parameter_space_views[3](3, 3) << '\n';
}
