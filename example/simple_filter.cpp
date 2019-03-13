
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

// Example for calculating verticle and horizontal edges of image and store them
// as different images.

int main() {
    using namespace boost::gil;

    rgb8_image_t img;
    read_image("test.jpg", img, jpeg_tag{});

    // Convolve the rows and the columns of the image with a fixed kernel
    gray8_image_t convolved(img.dimensions());
    gray8_image_t convolved2(img.dimensions());

    // radius-1 Gaussian kernel, size 9
    float sharpen_1[]={-0.05f,0.0f,0.05f};
    /*
    // radius-2 Gaussian kernel, size 15
    float gaussian_2[]={
        0.00048869418f,0.0024031631f,0.0092463447f,
        0.027839607f,0.065602221f,0.12099898f,0.17469721f,
        0.19744757f,
        0.17469721f,0.12099898f,0.065602221f,0.027839607f,
        0.0092463447f,0.0024031631f,0.00048869418f
    };
    //radius-3 Gaussian kernel, size 23
    float gaussian_3[]={
        0.00016944126f,0.00053842377f,0.0015324751f,0.0039068931f,
        0.0089216027f,0.018248675f,0.033434924f,0.054872241f,
        0.080666073f,0.10622258f,0.12529446f,
        0.13238440f,
        0.12529446f,0.10622258f,0.080666073f,
        0.054872241f,0.033434924f,0.018248675f,0.0089216027f,
        0.0039068931f,0.0015324751f,0.00053842377f,0.00016944126f
    };
    //radius-4 Gaussian kernel, size 29
    float gaussian_4[]={
        0.00022466264f,0.00052009715f,0.0011314391f,0.0023129794f,
        0.0044433107f,0.0080211498f,0.013606987f,0.021691186f,
        0.032493830f,0.045742013f,0.060509924f,0.075220309f,
        0.087870099f,0.096459411f,0.099505201f,0.096459411f,0.087870099f,
        0.075220309f,0.060509924f,0.045742013f,0.032493830f,
        0.021691186f,0.013606987f,0.0080211498f,0.0044433107f,
        0.0023129794f,0.0011314391f,0.00052009715f,0.00022466264f,
    };
    */

    // This is how to use a resizable kernel
    kernel_1d<float> kernel2(sharpen_1,3,1);
    convolve_rows<gray32f_pixel_t>(color_converted_view<gray8_pixel_t>(const_view(img)),kernel2,view(convolved));
    convolve_cols<gray32f_pixel_t>(color_converted_view<gray8_pixel_t>(const_view(img)),kernel2,view(convolved2));

    write_view("sharpen-convolution2.jpg", view(convolved), jpeg_tag{});
    write_view("sharpen-convolution3.jpg",view(convolved2),jpeg_tag{});
    return 0;
}
