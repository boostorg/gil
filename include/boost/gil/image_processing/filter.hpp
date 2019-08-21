//
// Copyright 2019 Miral Shah <miralshah2211@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_GIL_IMAGE_PROCESSING_FILTER_HPP
#define BOOST_GIL_IMAGE_PROCESSING_FILTER_HPP

#include <cstddef>
#include <vector>

#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

namespace boost { namespace gil {

template <typename SrcView, typename DstView>
void box_filter(
    SrcView const& src_view,
    DstView const& dst_view,
    std::size_t kernel_size,
    long int anchor = -1,
    bool normalize=true,
    convolve_boundary_option option = convolve_option_extend_zero
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

    convolve_1d<pixel<float, typename SrcView::value_type::layout_t>>(src_view, kernel, dst_view, option);
}

template <typename SrcView, typename DstView>
void blur(
    SrcView const& src_view,
    DstView const& dst_view,
    std::size_t kernel_size,
    long int anchor = -1,
    convolve_boundary_option option = convolve_option_extend_zero
)
{
    box_filter(src_view, dst_view, kernel_size, anchor, true, option);
}

}} //namespace boost::gil

#endif // !BOOST_GIL_IMAGE_PROCESSING_FILTER_HPP
