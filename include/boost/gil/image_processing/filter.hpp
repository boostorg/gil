//
// Copyright 2019 Miral Shah <miralshah2211@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_GIL_IMAGE_PROCESSING_FILTER_HPP
#define BOOST_GIL_IMAGE_PROCESSING_FILTER_HPP

#include <boost/gil/extension/numeric/algorithm.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <utility>
#include <vector>



namespace boost { namespace gil {

template <typename SrcView, typename DstView>
void box_filter(
    SrcView const& src_view,
    DstView const& dst_view,
    std::size_t kernel_size,
    long int anchor = -1,
    bool normalize=true,
    boundary_option option = boundary_option::extend_zero
)
{
    gil_function_requires<ImageViewConcept<SrcView>>();
    gil_function_requires<MutableImageViewConcept<DstView>>();
    static_assert(color_spaces_are_compatible
    <
        typename color_space_type<SrcView>::type,
        typename color_space_type<DstView>::type
    >::value, "Source and destination views must have pixels with the same color space");

    std::vector<float> kernel_values;
    if (normalize) { kernel_values.resize(kernel_size, 1.0f / float(kernel_size)); }
    else { kernel_values.resize(kernel_size, 1.0f); }

    if (anchor == -1) anchor = static_cast<int>(kernel_size / 2);
    kernel_1d<float> kernel(kernel_values.begin(), kernel_size, anchor);

    detail::convolve_1d
    <
        pixel<float, typename SrcView::value_type::layout_t>
    >(src_view, kernel, dst_view, option);
}

template <typename SrcView, typename DstView>
void blur(
    SrcView const& src_view,
    DstView const& dst_view,
    std::size_t kernel_size,
    long int anchor = -1,
    boundary_option option = boundary_option::extend_zero
)
{
    box_filter(src_view, dst_view, kernel_size, anchor, true, option);
}


namespace detail
{
template <typename SrcView, typename DstView>
void filter_median_impl(SrcView const& src_view, DstView const& dst_view, std::size_t kernel_size)
{
    std::size_t half_kernel_size = kernel_size / 2;

    // deciding output channel type and creating functor
    using src_channel_t = typename channel_type<SrcView>::type;

    std::vector<src_channel_t> values;
    values.reserve(kernel_size * kernel_size);

    for (std::ptrdiff_t y = 0; y < src_view.height(); y++)
    {
        typename DstView::x_iterator dst_it = dst_view.row_begin(y);

        for (std::ptrdiff_t x = 0; x < src_view.width(); x++)
        {
            auto sub_view = subimage_view(
                src_view,
                x - half_kernel_size, y - half_kernel_size,
                kernel_size,
                kernel_size
            );
            values.assign(sub_view.begin(), sub_view.end());

            std::nth_element(values.begin(), values.begin() + (values.size() / 2), values.end());
            dst_it[x] = values[values.size() / 2];
        }
    }
}
} // namespace detail

template <typename SrcView, typename DstView>
void median_filter(SrcView const& src_view, DstView const& dst_view, std::size_t kernel_size)
{
    static_assert(color_spaces_are_compatible
    <
        typename color_space_type<SrcView>::type,
        typename color_space_type<DstView>::type
    >::value, "Source and destination views must have pixels with the same color space");

    std::size_t half_kernel_size = kernel_size / 2;
    auto extended_img = extend_boundary(
        src_view,
        half_kernel_size,
        boundary_option::extend_constant
    );
    auto extended_view = subimage_view(
            view(extended_img),
            half_kernel_size,
            half_kernel_size,
            src_view.width(),
            src_view.height()
        );

    for (std::size_t channel = 0; channel < extended_view.num_channels(); channel++)
    {
        detail::filter_median_impl(
            nth_channel_view(extended_view, channel),
            nth_channel_view(dst_view, channel),
            kernel_size
        );
    }
}

namespace detail {

template<typename KernelT>
void get_1d_gaussian_kernel(
    KernelT& kernel,
    long int kernel_size,
    double sigma)
{
    if ((kernel_size%2) == 0)
        throw std::invalid_argument("kernel dimensions should be odd");

    const double exp_denom = 2 * sigma * sigma;
    auto center = kernel_size / 2;
    for (long int x = 0; x <= center; x++)
    {
        const auto delta_x = center - x;
        const double power = (delta_x * delta_x) / exp_denom;
        const double numerator = std::exp(-power);
        const float value = static_cast<float>(numerator/std::sqrt(M_PI * exp_denom));
        kernel[x] = value;
        kernel[kernel_size-1-x] = value;
    }
}

} // namespace detail

template <typename SrcView, typename DstView>
void gaussian_filter(
    SrcView src_view,
    DstView dst_view,
    long int kernel_size,
    double sigma,
    boundary_option option = boundary_option::extend_zero)
{
    gil_function_requires<ImageViewConcept<SrcView>>();
    gil_function_requires<MutableImageViewConcept<DstView>>();
    static_assert(color_spaces_are_compatible
    <
        typename color_space_type<SrcView>::type,
        typename color_space_type<DstView>::type
    >::value, "Source and destination views must have pixels with the same color space");

    std::vector<float> kernel_values(kernel_size);
    detail::getGaussianKernel(kernel_values, kernel_size, sigma, normalize);
    auto center = static_cast<int>(kernel_size/2);
    kernel_1d<float> kernel(kernel_values.begin(), kernel_size, center);

    detail::convolve_1d
    <
        pixel<float, typename SrcView::value_type::layout_t>
    >(src_view, kernel, dst_view, option);
}

}} //namespace boost::gil

#endif // !BOOST_GIL_IMAGE_PROCESSING_FILTER_HPP
