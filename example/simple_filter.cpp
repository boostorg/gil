//include libraries for image io, kernel definition and convolution
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>
//This example demonstrates the use of simple 1d kernel for detection of horizontal 
//and vertical edges.This difference kernel is [-.05,0,0.05] whose convolution gives
// the difference between neighbouring pixels as the value of pixel. Edges have sharp
//changes in intensities so difference between neighbouring pixels will be more. Hence
//pixels with higher intensity(brighter pixels) depict the presence of an edge.

int main() {
    using namespace boost::gil;
    // if(argc!=2){
    //     std::cerr<<"Usage: simple_filter <image_path>";
    //     return -1;
    // }
    rgb8_image_t img;
    
    read_image("test.jpg", img, jpeg_tag{});

    
    gray8_image_t convolved(img.dimensions());
    gray8_image_t convolved2(img.dimensions());

    // difference kernel
    float sharpen_1[]={-0.05f,0.0f,0.05f};
    

    
    kernel_1d<float> kernel2(sharpen_1,3,1);
    //compute horizontal edges
    convolve_rows<gray32f_pixel_t>(color_converted_view<gray8_pixel_t>(const_view(img)),kernel2,view(convolved));
    //compute vertical edges
    convolve_cols<gray32f_pixel_t>(color_converted_view<gray8_pixel_t>(const_view(img)),kernel2,view(convolved2));
    
    write_view("sharpen-convolution2.jpg", view(convolved), jpeg_tag{});
    write_view("sharpen-convolution3.jpg",view(convolved2),jpeg_tag{});
    return 0;
}
