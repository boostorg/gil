#include <boost/gil.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

#include <cmath>

using namespace std;
using namespace boost::gil;

// value of strong edge pixel
static const int TOP_VALUE = 255;
// value of weak (maybe edge) pixel
static const int MIDDLE_VALUE = 150;
// value of definitely not-edge pixel
static const int BOTTOM_VALUE = 0;

// Example how to detect edges via Canny algorithm
// Read more about Canny edge detection algorithm on
// https://docs.opencv.org/3.1.0/da/d22/tutorial_py_canny.html

template <typename SrcView>
void gaussian_blur(const SrcView &src)
{
    //1-D Gaussian blur kernel with kernel size 5, Sigma 1.0
    float gaussian[] = {0.06136f, 0.24477f, 0.38774f, 0.24477f, 0.06136f};
    kernel_1d<float> kernel(gaussian, 5, 2);

    convolve_rows<gray32f_pixel_t>(src, kernel, src, convolve_option_output_ignore);
    convolve_cols<gray32f_pixel_t>(src, kernel, src, convolve_option_output_ignore);
}

// Function calculates image gradient (intensity and direction)
// Smoothened image is filtered with a Sobel kernels in both horizontal and vertical
// direction to get first derivative in horizontal direction (Gx) and vertical direction (Gy).
// Gradient manitude is found as G = sqrt(Gx^2 + Gy^2)
// Direction is found as angle = arctan(Gy / Gx)
// More information about sobel filtering on https://en.wikipedia.org/wiki/Sobel_operator
void sobel_filtering(const gray8c_view_t &src,
                     const gray8_view_t &magnitude, const gray32f_view_t &slope)
{
    // This we used 1-D convolution
    // Gx = second * (first * img)
    // Gy = first * (second * img)
    // where * is convolution
    float first_sobel[] = {1.f, 0.f, -1.f};
    float second_sobel[] = {1.f, 2.f, 1.f};

    kernel_1d<float> first_sobel_kernel(first_sobel, 3, 1);
    kernel_1d<float> second_sobel_kernel(second_sobel, 3, 1);

    //16 bits signed matrix is used in order to avoid char overflow
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

    // Magnitude and angle calculation
    auto ver_it = view(vertical).begin();
    auto hor_it = view(horizontal).begin();
    auto slope_it = slope.begin();

    for (auto mag_it = magnitude.begin(); mag_it != magnitude.end();
            ++mag_it, ++ver_it, ++hor_it, ++slope_it) {

        // std::min was used to be sure, that pixel max value less than 256
        *mag_it = std::min(UINT8_MAX,
                           (int)std::sqrt(
                               std::pow((int)(*ver_it), 2) + std::pow((int)(*hor_it), 2)));

        *slope_it = (float)std::atan2((int)(*hor_it), (int)(*ver_it));
    }
}

// Non-maximum suppression method is perfomed to thin out edges of the image
// Method goes throug all the points on the gradient intensity matrix and
// finds the pixels with maximum value in the edge direction
void non_maximal_suppression(const gray8c_view_t &magnitude, const gray32fc_view_t &angle,
                             const gray8_view_t &dst)
{
    // Dst matrix fills with 0
    fill_pixels(dst, int8_t(0));

    auto mag_loc = magnitude.xy_at(1, 1);
    int q, r;
    int index;
    int curVal;
    for (int y = 1; y < magnitude.height() - 1; ++y) {
        auto dst_it = dst.row_begin(y);
        auto angle_it = angle.row_begin(y);

        for (int x = 1; x < dst.width() - 1; ++x, ++angle_it, ++dst_it, ++mag_loc.x()) {
            // Index helps to find direction of the edge
            // Pixel has 8 neighbors and 4 possible directions
            // (-1,-1) (0,-1) (1,-1)
            // (-1, 0) (0, 0) (1, 0)
            // (-1, 1) (0, 1) (1, 1)
            // On each direction 2 possible neighbors
            // If both of them is smaller than current value (0, 0),
            // then current value is saved in output image
            index = (int)((*angle_it)[0] * 8 / M_PI);
            index = (index < 0) ? index + 8 : index;

            curVal = mag_loc(0, 0);

            switch (index) {
            // Horizontal direction
            case 0:
            case 7:
            case 8:
                q = mag_loc(1, 0);
                r = mag_loc(-1, 0);
                break;
            // 45 degree
            case 1:
            case 2:
                q = mag_loc(1, 1);
                r = mag_loc(-1, -1);
                break;
            // Vertical direction
            case 3:
            case 4:
                q = mag_loc(0, 1);
                r = mag_loc(0, -1);
                break;
            // 135 degree
            case 5:
            case 6:
                q = mag_loc(1, -1);
                r = mag_loc(-1, 1);
                break;
            }

            if ((curVal >= q) && (curVal >= r)) {
                *dst_it = curVal;
            }
        }
        mag_loc += point2<std::ptrdiff_t>(-dst.width() + 2, 1);
    }
}

void hysteresis_threshold(const gray8_view_t &dst, int minVal, int maxVal)
{
    // Histogram calculation of 3 groups: strong pixel, weak pixel, definitely not-edge pixel
    for_each_pixel(dst, [minVal, maxVal](gray8_pixel_t &pixel) {
        pixel = (pixel < minVal) ? BOTTOM_VALUE :
                ((pixel > maxVal) ? TOP_VALUE : MIDDLE_VALUE);
    });

    // Weak pixels check
    // If weak pixel has a strong neighbor, then it is strong one
    // otherwise it is definitely not-edge pixel
    auto dst_loc = dst.xy_at(1, 1);
    for (int i = 1; i < dst.height() - 1; ++i) {
        for (int j = 1; j < dst.width() - 1; ++j, ++dst_loc.x()) {
            if (dst_loc(0, 0) == MIDDLE_VALUE) {
                if (dst_loc(-1, -1) == TOP_VALUE || dst_loc(-1, 0) == TOP_VALUE ||
                        dst_loc(-1, 1) == TOP_VALUE || dst_loc(1, -1) == TOP_VALUE ||
                        dst_loc(1, 0) == TOP_VALUE || dst_loc(1, 1) == TOP_VALUE ||
                        dst_loc(0, -1) == TOP_VALUE || dst_loc(0, 1) == TOP_VALUE)
                    dst_loc(0, 0) = TOP_VALUE;
                else
                    dst_loc(0, 0) = BOTTOM_VALUE;
            }
        }
        dst_loc += point2<std::ptrdiff_t>(-dst.width() + 2, 1);
    }
}

void canny_edge_detection(const rgb8c_view_t &src,
                          const gray8_view_t &dst, int minVal, int maxVal)
{
    // Canny edge detection algorithm works on grayscale images only
    gray8_image_t gray_img(src.dimensions());
    copy_pixels(color_converted_view<gray8_pixel_t>(src), view(gray_img));

    gaussian_blur(view(gray_img));

    gray32f_image_t angle(src.dimensions());
    gray8_image_t magnitude(src.dimensions());
    sobel_filtering(view(gray_img), view(magnitude), view(angle));

    non_maximal_suppression(view(magnitude), view(angle), dst);

    hysteresis_threshold(dst, minVal, maxVal);
}

int main(int argc, char *argv[])
{
    char *input = "test.jpg";
    char *output = "canny.jpg";
    int min_threshold_value = 30;
    int max_threshold_value = 70;

    if (argc >= 2 && (argv[1][0] == '-') && (argv[1][1] == 'H' || argv[1][1] == 'h')) {
        printf("canny [path_to_input [path_to_output "
               "[min_threshold_value [max_threshold_value]]]]\n");
        return 0;
    }
    if (argc >= 2) input = argv[1];
    if (argc >= 3) output = argv[2];
    if (argc >= 4) min_threshold_value = atoi(argv[3]);
    if (argc >= 5) max_threshold_value = atoi(argv[4]);

    if (min_threshold_value > max_threshold_value)
        std::swap(min_threshold_value, max_threshold_value);

    rgb8_image_t img;
    read_image(input, img, jpeg_tag{});

    gray8_image_t res_img(img.dimensions());
    canny_edge_detection(const_view(img), view(res_img),
                         min_threshold_value, max_threshold_value);
    write_view(output, view(res_img), jpeg_tag{});
    return 0;
}
