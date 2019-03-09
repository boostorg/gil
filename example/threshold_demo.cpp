#include <boost/gil.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include<boost/gil/threshold.hpp>

int main(){
	using namespace boost::gil;
	rgb8_image_t img;
    read_image("test.jpg", img, jpeg_tag());
    gray8_image_t output(img.dimensions());
    bin_thresh(color_converted_view<gray8_pixel_t>(const_view(img)),view(output),255,0,100);
    write_view("out-bin_thresh.jpeg", view(output), jpeg_tag{});
}