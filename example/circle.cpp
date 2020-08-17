#include "boost/gil/extension/io/png/tags.hpp"
#include "boost/gil/io/write_view.hpp"
#include "boost/gil/point.hpp"
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/rasterization/circle.hpp>
#include <boost/gil/typedefs.hpp>
#include <cmath>
#include <limits>
#include <vector>

namespace gil = boost::gil;

int main()
{
    const std::ptrdiff_t size = 256;
    gil::gray8_image_t buffer_image(size, size);
    auto buffer = gil::view(buffer_image);

    const std::ptrdiff_t radius = 64;
    std::vector<gil::point_t> circle_points(gil::estimate_circle_point_count_trig(radius));
    gil::rasterize_circle_trig(radius, {128, 128}, circle_points.begin());
    for (const auto& point : circle_points)
    {
        buffer(point) = std::numeric_limits<gil::uint8_t>::max();
    }

    gil::write_view("circle.png", buffer, gil::png_tag{});
}
