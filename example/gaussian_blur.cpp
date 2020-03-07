#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/image_processing/filter.hpp>

using namespace boost::gil;

int main()
{
    rgb8_image_t img;
    read_image("test.jpg",img, jpeg_tag{});
    rgb8_image_t img_out(img.dimensions());

//    performing binary threshold on each channel of the image
//    if the pixel value is more than 150 than it will be set to 255 else to 0
    boost::gil::gaussian_filter(const_view(img), view(img_out), 5, 1.0f);
    write_view("gaussian_blur.jpg", view(img_out), jpeg_tag{});

    return 0;
}
