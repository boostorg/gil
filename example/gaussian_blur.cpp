#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/image_processing/filter.hpp>

using namespace boost::gil;

int main()
{
    rgb8_image_t img;
    read_image("test.jpg",img, jpeg_tag{});
    rgb8_image_t img_out(img.dimensions());

//    performing Gaussian Blur on image
//    here kernel size is 5 and sigma is taken as 1
    boost::gil::gaussian_filter(const_view(img), view(img_out), 5, 1.0f);
    write_view("gaussian_blur.jpg", view(img_out), jpeg_tag{});

    return 0;
}
