/// \file
/// \brief Test file for convolve_rows() and convolve_cols() in the numeric extension
/// \author Lubomir Bourdev and Hailin Jin
/// \date February 27, 2007

#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

int main() {
    using namespace boost::gil;

    rgb8_image_t img;
    jpeg_read_image("test.jpg",img);

    // Convolve the rows and the columns of the image with a fixed kernel
    rgb8_image_t convolved(img);

    float gaussian[]={0.00022923296f,0.0059770769f,0.060597949f,0.24173197f,0.38292751f,
                      0.24173197f,0.060597949f,0.0059770769f,0.00022923296f};

    kernel_1d_fixed<float,9> kernel(gaussian,4);

    convolve_rows_fixed<rgb32f_pixel_t>(const_view(convolved),kernel,view(convolved));
    convolve_cols_fixed<rgb32f_pixel_t>(const_view(convolved),kernel,view(convolved));
    jpeg_write_view("out-convolution.jpg", view(convolved));

    // This is how to use a resizable kernel
    kernel_1d<float> kernel2(gaussian,9,4);
    convolve_rows<rgb32f_pixel_t>(const_view(img),kernel2,view(img));
    convolve_cols<rgb32f_pixel_t>(const_view(img),kernel2,view(img));
    jpeg_write_view("out-convolution2.jpg", view(img));

    return 0;
}
