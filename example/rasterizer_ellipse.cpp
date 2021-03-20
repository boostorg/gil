#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/rasterization/ellipse.hpp>
#include <iostream>

namespace gil = boost::gil;

int main()
{
    gil::rgb8_image_t img;
    gil::read_image("test.jpg", img, gil::jpeg_tag{});
    
    gil::draw_ellipse(view(img), {255, 255, 0}, {60, 40}, 5, 20);
    gil::write_view("example_ellipse.jpg", view(img), gil::jpeg_tag{});

    const std::ptrdiff_t size = 256;
    gil::gray8_image_t buffer_image(size, size);
    auto buffer = gil::view(buffer_image);

    gil::draw_ellipse(buffer, {255, 0, 0}, {128, 128}, 100, 50);
    gil::write_view("ellipse_buffer.png", buffer, gil::png_tag{});
}