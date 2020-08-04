#include "boost/gil/extension/io/png/tags.hpp"
#include "boost/gil/point.hpp"
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/rasterization/line.hpp>
#include <boost/gil/typedefs.hpp>

#include <limits>
#include <vector>

namespace gil = boost::gil;

const std::ptrdiff_t size = 256;

void line_naive(std::ptrdiff_t width, std::ptrdiff_t height, const std::string& output_name)
{
    std::vector<gil::point_t> line_points(gil::estimate_point_count_naive(width, height));

    gil::gray8_image_t image(size, size);
    auto view = gil::view(image);

    gil::rasterize_line_incr(width, height, line_points.begin());
    for (const auto& point : line_points)
    {
        view(point) = std::numeric_limits<gil::uint8_t>::max();
    }

    gil::write_view(output_name, view, gil::png_tag{});
}

int main()
{
    line_naive(256, 256, "line-naive-256-256.png");
    line_naive(256, 128, "line-naive-256-128.png");
    line_naive(256, 1, "line-naive-256-1.png");
    line_naive(1, 256, "line-naive-1-256.png");
}
