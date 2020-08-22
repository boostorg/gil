#include <boost/core/lightweight_test.hpp>

#include <boost/gil/image.hpp>
#include <boost/gil/image_processing/hough_transform.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/typedefs.hpp>

#include <cstddef>
#include <iostream>
#include <limits>
#include <vector>

namespace gil = boost::gil;

template <typename Rasterizer>
void exact_fit_test(std::ptrdiff_t radius, gil::point_t offset, Rasterizer rasterizer,
                    std::ptrdiff_t neighborhood, std::size_t half_step_count)
{
    std::vector<gil::point_t> circle_points(rasterizer.point_count(radius));
    rasterizer(radius, offset, circle_points.begin());
    // const std::ptrdiff_t diameter = radius * 2 - 1;
    const std::ptrdiff_t width = offset.x + radius + 2 * neighborhood;
    const std::ptrdiff_t height = offset.y + radius + 2 * neighborhood;
    gil::gray8_image_t image(width, height);
    auto input = gil::view(image);

    for (const auto& point : circle_points)
    {
        input(point) = std::numeric_limits<gil::uint8_t>::max();
    }

    using param_t = gil::hough_parameter<std::ptrdiff_t>;
    const auto radius_parameter = param_t{radius, 0, 1};
    // const auto x_parameter = param_t::from_step_count(offset.x, neighborhood, half_step_count);
    // const auto y_parameter = param_t::from_step_count(offset.y, neighborhood, half_step_count);
    const auto x_parameter = param_t{offset.x, 0, 1};
    const auto y_parameter = param_t{offset.y, 0, 1};

    std::vector<gil::gray8_image_t> output_images(
        radius_parameter.step_count,
        gil::gray8_image_t(x_parameter.step_count, y_parameter.step_count));
    std::vector<gil::gray8_view_t> output_views(radius_parameter.step_count);
    std::transform(output_images.begin(), output_images.end(), output_views.begin(),
                   [](gil::gray8_image_t& img) { return gil::view(img); });
    gil::hough_circle_transform_brute(input, radius_parameter, x_parameter, y_parameter,
                                      output_views.begin(), rasterizer);
    const auto expected_index = half_step_count;
    std::cout << "x offset: " << offset.x << " y offset: " << offset.y << " radius:" << radius
              << " accumulated value:" << static_cast<int>(output_views[0](0, 0)[0]) << "\n\n\n";
    BOOST_TEST(output_views[0](0, 0) == rasterizer.point_count(radius));
}

int main()
{
    const std::ptrdiff_t neighborhood = 3;
    const std::size_t half_step_count = 3;
    for (std::ptrdiff_t radius = 5; radius < 100; ++radius)
    {
        for (std::ptrdiff_t x_offset = radius; x_offset < 100; ++x_offset)
        {
            for (std::ptrdiff_t y_offset = radius; y_offset < 100; ++y_offset)
            {

                exact_fit_test(radius, {x_offset + neighborhood, y_offset + neighborhood},
                               gil::midpoint_circle_rasterizer{}, neighborhood, half_step_count);
                exact_fit_test(radius, {x_offset + neighborhood, y_offset + neighborhood},
                               gil::trigonometric_circle_rasterizer{}, neighborhood,
                               half_step_count);
            }
        }
    }

    return boost::report_errors();
}
