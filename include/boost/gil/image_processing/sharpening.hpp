//
// Copyright 2021 Harsit Pant <harshitpant83@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_GIL_IMAGE_PROCESSING_SHARPENING_HPP
#define BOOST_GIL_IMAGE_PROCESSING_SHARPENING_HPP

#include <boost/gil/extension/numeric/algorithm.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/toolbox/color_spaces/lab.hpp>

#include <boost/gil/concepts/concept_check.hpp>
#include <boost/gil/image_processing/numeric.hpp>
#include <boost/gil/detail/math.hpp>
#include <boost/gil/algorithm.hpp>
#include <boost/gil/color_convert.hpp>
#include <boost/gil/gray.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/rgb.hpp>

#include <boost/assert.hpp>

#include <cmath>

namespace boost { namespace gil {
namespace detail{

/// \breif USM algorithm might sharpen noise (perceiving noise as edges), to control this
/// threshold is specified, thus sharpening only high frequency regions (strong edges).
/// For each pixel I(u,v), if (local_contrast(u, v) < minimum local contrast), put I(u, v) = 0.
template<typename View>
void threshold_mask(View const& highpass_filtered_image, double threshold)
{
    float32_t max_absolute_contrast = 0.0;
    for_each_pixel(nth_channel_view(highpass_filtered_image, 0),
                                              [&max_absolute_contrast](auto& pixel) {
		                                    if (max_absolute_contrast < std::abs(pixel[0]))
		                                        max_absolute_contrast = std::abs(pixel[0]); });

    float32_t const minimum_local_contrat = max_absolute_contrast * static_cast<float>(threshold);

    for_each_pixel(nth_channel_view(highpass_filtered_image, 0),
                                             [&minimum_local_contrat](auto& pixel) {
                                           if (std::abs(pixel[0]) < minimum_local_contrat)
		                                                pixel[0] = 0; });
}

/// \breif clamps lab colors out of gamut for RGB color space.
template<typename SrcView>
inline void clip_lab_color(SrcView& float32_output_image)
{
    rgb32f_pixel_t rgb32f_temp_pixel;

    for_each_pixel(float32_output_image, [&rgb32f_temp_pixel](auto& input_pixel) {

                                          color_convert(input_pixel, rgb32f_temp_pixel);

	                                      if (rgb32f_temp_pixel[0] > 1) rgb32f_temp_pixel[0] = 1;
                                          if (rgb32f_temp_pixel[0] < 0) rgb32f_temp_pixel[0] = 0;
                                          if (rgb32f_temp_pixel[1] > 1) rgb32f_temp_pixel[1] = 1;
                                          if (rgb32f_temp_pixel[1] < 0) rgb32f_temp_pixel[1] = 0;
                                          if (rgb32f_temp_pixel[2] > 1) rgb32f_temp_pixel[2] = 1;
                                          if (rgb32f_temp_pixel[2] < 0) rgb32f_temp_pixel[2] = 0;

                                          color_convert(rgb32f_temp_pixel, input_pixel); });
}

/// TODO - Replace use of this function with convolve_2d when,
/// it starts supporting boundary_option::extend_constant.
template <typename SrcView, typename DstView, typename Kernel>
void convolve_2d_extend_constant(SrcView const& src_view, DstView const& dst_view,
                                                          Kernel const& kernel)
{
    auto padded_image = extend_boundary(src_view, kernel.size() / 2, 
                                                  boundary_option::extend_constant);

    decltype(padded_image) convolved_image(padded_image.dimensions());
	
    detail::convolve_2d(view(padded_image), kernel, view(convolved_image));

    copy_pixels(subimage_view(view(convolved_image),
				              point_t(kernel.size() / 2, kernel.size() / 2),
				              src_view.dimensions()), dst_view);
}
} // namespace detail

/// \addtogroup ImageProcessing
/// @{
/// \breif Sharpens grayscale/RGB images using unsharp masking technique.
///
/// For each pixel I(u, v) - 
///             if(local_contrast(u, v) > minimum local constrast)
///                 I(u, v) = I(u, v) + amount * (I(u, v) - I'(u, v))
///             else
///                 I(u, v) = I (u, v)
/// where, minimum local contrast is specified using the parameter threshold.
///        I' is a smoother version of I, made by gaussian bluring.
/// 
/// RGB color images are converted to lab color space, USM algorithm is applied to L* channel
/// (a*and b* channels remain unchanged), then lab image is converted back to RGB color space.
/// 
/// Reference - Principles of Digital Image Processing - Fundamental Techniques
/// by Wilhelm Burger, Mark J.Burge.
/// 
/// \param src_view  - Source image view.
/// \param dst_view  - Destination image view.
/// \param sigma     - Standard deviation for generating gaussian kernel.
/// \param amount    - Sharpening amount or weight.
/// \param threshold - Minimum local contrast for each pixel, 
///                    specified in the range 0 to 1, to apply sharpening.
/// \tparam SrcView  - Source image view type.
/// \tparam DstView  - Destination image view type.
template<typename SrcView, typename DstView>
void sharpen(SrcView const& src_view, DstView const& dst_view, double sigma = 1,
	                                  double amount = 1, double threshold = 0.0)
{
    gil_function_requires<ImageViewConcept<SrcView>>();
    gil_function_requires<MutableImageViewConcept<DstView>>();

    using SrcColorSpace = typename color_space_type<SrcView>::type;
    using DstColorSpace = typename color_space_type<DstView>::type;

    static_assert(color_spaces_are_compatible<SrcColorSpace, DstColorSpace>::value,
                 "Source and destination views must have same color space.");

    static_assert(is_same<rgb_t, SrcColorSpace>::value ||
                  is_same<gray_t, SrcColorSpace>::value,
                 "Incompatible Color space used.");

    BOOST_ASSERT(src_view.dimensions() == dst_view.dimensions());

    int const kernel_size = static_cast<int>(std::ceil(sigma) * 6 + 1);
    detail::kernel_2d<float> highpass_kernel = generate_gaussian_kernel<float>
                                                (kernel_size, static_cast<float>(sigma));

    //highpass kernel = impulse kernel - lowpass kernel. Produces the mask.
    for_each(highpass_kernel.begin(), highpass_kernel.end(), [](float& x) { x *= -1; });
    highpass_kernel[kernel_size / 2 * (kernel_size + 1)] += 1;

    typename std::conditional<
        is_same<rgb_t, SrcColorSpace>::value,
        lab32f_image_t, gray32f_image_t>::type float32_input_image(src_view.dimensions()),
                                               float32_output_image(src_view.dimensions());

    copy_and_convert_pixels(src_view, view(float32_input_image));
    copy_and_convert_pixels(src_view, view(float32_output_image));

    detail::convolve_2d_extend_constant(nth_channel_view(view(float32_input_image), 0),
                                        nth_channel_view(view(float32_output_image), 0),
                                        highpass_kernel);

    if (threshold > 0.0f)
    {
        detail::threshold_mask(view(float32_output_image), threshold);
    }

    transform_pixels(view(float32_input_image),
                     view(float32_output_image),
                     view(float32_output_image), [&amount](auto orignal, auto mask) {
                                                           decltype(orignal) temp(orignal);
                                                           temp[0] += mask[0] * 
                                                                      static_cast<float>(amount);
                                                           return  temp; });

    float32_t channel_max = (is_same<rgb_t, SrcColorSpace>::value) ? 100 : 1;
    float32_t channel_min = 0;
    for_each_pixel(nth_channel_view(view(float32_output_image), 0), 
                                                      [&channel_max, &channel_min](auto& pixel) {
                                                       if (pixel[0] > channel_max)
                                                           pixel[0] = channel_max;
                                                       if (pixel[0] < channel_min)
                                                           pixel[0] = channel_min; });

    if (is_same<rgb_t, SrcColorSpace>::value)
    {
        clip_lab_color(view(float32_output_image));
    }

    copy_and_convert_pixels(view(float32_output_image), dst_view);
}
/// @}

}}     //namespace boost::gil
#endif //BOOST_GIL_IMAGE_PROCESSING_SHARPENING_HPP
