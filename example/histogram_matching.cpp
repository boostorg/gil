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
    rgb8_image_t equalized_img(img.dimensions());
    rgb8_image_t ref_img;
    histogram<gray8_pixel_t> ref_hist;
    read_and_convert_image("test2.png", ref_img, png_tag());
    fill_histogram<rgb8_view_t, gray8_pixel_t>(view(ref_img), ref_hist);
    float ref_cdf[256];
    ref_cdf[0] = (float)ref_hist[gray8_pixel_t(0)] / ref_img.width() / ref_img.height();
    for (int i = 1; i < 256; ++i) ref_cdf[i] = ref_cdf[i - 1] + (float)ref_hist[gray8_pixel_t(i)] / ref_img.width() / ref_img.height();

    match_histograms(view(img), ref_cdf, view(equalized_img));
    write_view("test_histogram_matched.jpg", view(equalized_img), jpeg_tag{});
    return 0;
}