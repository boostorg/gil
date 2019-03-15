#include <boost/gil.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

#include <math.h>

using namespace std;
using namespace boost::gil;

//Example how to detect edges via Canny algorithm

template <typename SrcView>
void smoothing(const SrcView &src)
{
    float gaussian[] = {0.06136f, 0.24477f, 0.38774f, 0.24477f, 0.06136f};
    kernel_1d<float> kernel(gaussian, 5, 2);

    convolve_rows<gray32f_pixel_t>(src, kernel, src, convolve_option_output_ignore);
    convolve_cols<gray32f_pixel_t>(src, kernel, src, convolve_option_output_ignore);
}

void sobel_filtering(const gray8c_view_t &src,
                     const gray8_view_t &magnitude, const gray32f_view_t &slope)
{
    float first_sobel[] = {1.f, 0.f, -1.f};
    float second_sobel[] = {1.f, 2.f, 1.f};

    kernel_1d<float> first_sobel_kernel(first_sobel, 3, 1);
    kernel_1d<float> second_sobel_kernel(second_sobel, 3, 1);

    gray16s_image_t vertical(src.dimensions());
    gray16s_image_t horizontal(src.dimensions());

    convolve_rows<gray32f_pixel_t>(src, first_sobel_kernel,
                                   view(vertical), convolve_option_output_zero);
    convolve_cols<gray32f_pixel_t>(const_view(vertical), second_sobel_kernel,
                                   view(vertical), convolve_option_output_zero);

    convolve_rows<gray32f_pixel_t>(src, second_sobel_kernel, view(horizontal),
                                   convolve_option_output_zero);
    convolve_cols<gray32f_pixel_t>(const_view(horizontal), first_sobel_kernel,
                                   view(horizontal), convolve_option_output_zero);

    //Magnitude and angle
    auto ver_it = view(vertical).begin();
    auto hor_it = view(horizontal).begin();
    auto slope_it = slope.begin();

    for (auto mag_it = magnitude.begin(); mag_it != magnitude.end();
            ++mag_it, ++ver_it, ++hor_it, ++slope_it) {

        *mag_it = std::min(255,
                           (int)std::sqrt(
                               std::pow((int)(*ver_it), 2) + std::pow((int)(*hor_it), 2)));

        *slope_it = (float)std::atan2((int)(*hor_it), (int)(*ver_it));
    }
}

void suppression_threshold(const gray8c_view_t &magnitude, const gray32fc_view_t &angle,
                           int minVal, int maxVal, const gray8_view_t &dst)
{
    fill_pixels(dst, int8_t(0));

    auto mag_loc = magnitude.xy_at(1, 1);
    int q, r;
    int index;
    int curVal;
    for (int y = 1; y < magnitude.height() - 1; ++y) {
        auto dst_it = dst.row_begin(y);
        auto angle_it = angle.row_begin(y);

        for (int x = 1; x < dst.width() - 1; ++x, ++angle_it, ++dst_it, ++mag_loc.x()) {
            index = (int)((*angle_it)[0] * 8 / M_PI);
            index = (index < 0) ? index + 8 : index;

            curVal = mag_loc(0, 0);

            switch (index) {
            case 0:
            case 7:
            case 8:
                q = mag_loc(1, 0);
                r = mag_loc(-1, 0);
                break;
            case 1:
            case 2:
                q = mag_loc(1, 1);
                r = mag_loc(-1, -1);
                break;
            case 3:
            case 4:
                q = mag_loc(0, 1);
                r = mag_loc(0, -1);
                break;
            case 5:
            case 6:
                q = mag_loc(1, -1);
                r = mag_loc(-1, 1);
                break;
            default:
                q = 255;
                r = 255;
            }

            if ((curVal >= minVal) && (curVal >= q) && (curVal >= r)) {
                if (curVal >= maxVal)
                    *dst_it = 255;
                else
                    *dst_it = 150;
            }
        }
        mag_loc += point2<std::ptrdiff_t>(-dst.width() + 2, 1);
    }
}

void hysteresis(const gray8_view_t &dst)
{
    auto dst_loc = dst.xy_at(1, 1);
    for (int i = 1; i < dst.height() - 1; ++i) {
        for (int j = 1; j < dst.width() - 1; ++j, ++dst_loc.x()) {
            if (dst_loc(0, 0) == 150) {
                if (dst_loc(-1, -1) == 255 || dst_loc(-1, 0) == 255 ||
                        dst_loc(-1, 1) == 255 || dst_loc(1, -1) == 255 ||
                        dst_loc(1, 0) == 255 || dst_loc(1, 1) == 255 ||
                        dst_loc(0, -1) == 255 || dst_loc(0, 1) == 255)
                    dst_loc(0, 0) = 255;
                else
                    dst_loc(0, 0) = 0;
            }
        }
        dst_loc += point2<std::ptrdiff_t>(-dst.width() + 2, 1);
    }
}

void canny(const rgb8c_view_t &src, const gray8_view_t &dst, int minVal, int maxVal)
{
    //grayscale img
    gray8_image_t gray_img(src.dimensions());
    copy_pixels(color_converted_view<gray8_pixel_t>(src), view(gray_img));

    smoothing(view(gray_img));

    gray32f_image_t angle(src.dimensions());
    gray8_image_t magnitude(src.dimensions());
    sobel_filtering(view(gray_img), view(magnitude), view(angle));

    suppression_threshold(view(magnitude), view(angle), minVal, maxVal, dst);

    hysteresis(dst);
}

int main()
{
    rgb8_image_t img;
    read_image("test.jpg", img, jpeg_tag{});

    gray8_image_t res_img(img.dimensions());
    canny(const_view(img), view(res_img), 20, 40);
    write_view("canny.jpg", view(res_img), jpeg_tag{});
    return 0;
}
