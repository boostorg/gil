#include <boost/gil.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/image_processing/histogram_equalization.hpp>

using namespace boost::gil;

int main()
{
    gray8_image_t img;
    read_image("test_adaptive.png", img, png_tag{});
    gray8_image_t img_out(img.dimensions());

    rgb8_image_t rgb_img;
    read_image("test.jpg",rgb_img, jpeg_tag{});
    rgb8_image_t rgb_img_out(rgb_img.dimensions());

    boost::gil::histogram_equalization(view(img),view(img_out));

    write_view("histogram_gray_equalized_image.png", view(img_out), png_tag{});

    boost::gil::histogram_equalization(view(rgb_img),view(rgb_img_out));

    write_view("histogram_rgb_equalized_image.jpg", view(rgb_img_out), jpeg_tag{});

    return 0;
}