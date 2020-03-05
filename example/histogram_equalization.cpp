#include <iostream>
// #include <boost/gil.hpp>
#include <boost/gil/extension/histogram/histogram.hpp>
#include <boost/gil/extension/histogram/histogram_algorithms.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/color_base.hpp>
#include <type_traits>
#include <boost/gil/detail/mp11.hpp>
#include <boost/mp11.hpp>

using namespace boost::gil;

int main() {
    rgb8_image_t img;
    read_and_convert_image("test.png", img, png_tag());
    histogram<gray8_pixel_t> hist;
    fill_histogram<rgb8_view_t, gray8_pixel_t>(view(img), hist);
    save_histogram_img(hist, "test_histogram.jpg");
    rgb8_image_t equalized_img(img.dimensions());
    equalise_histogram(view(img), view(equalized_img));
    write_view("test_equalized.jpg", view(equalized_img), jpeg_tag{});
    return 0;
}
