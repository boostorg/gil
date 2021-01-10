#include <iostream>
#include <boost/gil/image_processing/threshold.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <vector>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>
int main()
{
    using namespace boost::gil;
    gray8_image_t img;
    read_image("opening.png", img, png_tag{});
    std::vector<float>ker_vec(9,0.1f);
    detail::kernel_2d<float> ker_mat(ker_vec.begin(), ker_vec.size(), 1, 1);
    gray8_image_t img_out_dilation(img.dimensions()),img_out_erosion(img.dimensions());
    threshold_binary(const_view(img), view(img),170, 255);
    detail::convolve_2d(view(img), ker_mat, view(img));
    threshold_binary(const_view(img),view(img_out_dilation),51,255);
    threshold_binary(const_view(img),view(img_out_erosion),128,255);
    write_view("out-morphological-example-dilation.png", view(img_out_dilation), png_tag{});
    write_view("out-morphological-example-erosion.png", view(img_out_erosion), png_tag{});
}
