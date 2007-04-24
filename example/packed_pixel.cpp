/// \file
/// \brief Example file to show how to deal with packed pixels
/// \author Lubomir Bourdev and Hailin Jin
/// \date February 27, 2007
///
/// This test file demonstrates how to use packed pixel formats in GIL. 
/// A "packed" pixel is a pixel whose channels are bit ranges.
/// Here we create an RGB image whose pixel has 16-bits, such as:
/// bits [0..6] are the blue channel
/// bits [7..13] are the green channel
/// bits [14..15] are the red channel
/// We read a regular 8-bit RGB image, convert it to packed BGR772, convert it back to 8-bit RGB and save it to a file.
/// Since the red channel is only two bits the color loss should be observable in the result
///
/// To demonstrate that image view transformations work on packed images, we save the result transposed.

#include <algorithm>
#include <boost/gil/extension/io/jpeg_io.hpp>

using namespace boost;
using namespace boost::gil;


// define a bgr772 image
typedef const packed_channel_reference<boost::uint16_t, 0,7,true> bgr772_channel0_t;
typedef const packed_channel_reference<boost::uint16_t, 7,7,true> bgr772_channel1_t;
typedef const packed_channel_reference<boost::uint16_t,14,2,true> bgr772_channel2_t;
typedef heterogeneous_packed_pixel<uint16_t, 
    mpl::vector3<bgr772_channel0_t,bgr772_channel1_t,bgr772_channel2_t>, bgr_layout_t> bgr772_pixel_t;
typedef image<bgr772_pixel_t,false> bgr772_image_t;

int main() {
    boost::function_requires<PixelValueConcept<bgr772_pixel_t> >();
    BOOST_STATIC_ASSERT((sizeof(bgr772_pixel_t)==2));

    bgr8_image_t img;
    jpeg_read_image("test.jpg",img);

    bgr772_image_t img_packed1(img.dimensions());
    copy_and_convert_pixels(const_view(img),view(img_packed1));

    // Save the result. JPEG I/O does not support the packed pixel format, so convert it back to 8-bit RGB
    jpeg_write_view("out-packed_pixel.jpg",color_converted_view<bgr8_pixel_t>(transposed_view(const_view(img_packed1))));

    return 0;
}
