//
// Copyright 2005-2007 Adobe Systems Incorporated
// Copyright 2019 Miral Shah <miralshah2211@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_GIL_THRESHOLD_HPP
#define BOOST_GIL_THRESHOLD_HPP

#include <boost/gil/image.hpp>

namespace boost { namespace gil {


//threshold takes an image view and perform thresholding operation on each channel
//and returns and image view.
template <typename SrcView, typename DstView>
void threshold_binary(SrcView const& src, DstView const& dst, int threshold, int max)
{
    //template argument validation
    gil_function_requires<ImageViewConcept<SrcView>>();
    gil_function_requires<MutableImageViewConcept<DstView>>();
    gil_function_requires<ColorSpacesCompatibleConcept<
            typename color_space_type<SrcView>::type,
            typename color_space_type<DstView>::type>>();

    //deciding output channel type and creating functor
    typedef typename channel_type<SrcView>::type source_channel_t;
    typedef typename channel_type<DstView>::type result_channel_t;

    //iterate over the image chaecking each pixel value for the threshold
    for (size_t y = 0; y < src.height(); y++)
    {
        typename SrcView::x_iterator src_it = src.row_begin(y);
        typename DstView::x_iterator dst_it = dst.row_begin(y);

        for (size_t x = 0; x < src.width(); x++)
        {
            // static_transform(src_it[x], dst_it[x], calling_function);
            static_transform(src_it[x], dst_it[x], [threshold, max](source_channel_t px) -> result_channel_t{ return px >= threshold ? max : 0; });
        }
    }
}

//threshold takes an image view and perform thresholding operation on each channel
//and returns and image view.
template <typename SrcView, typename DstView>
void threshold_binary_inverse(SrcView const& src, DstView const& dst, int threshold, int max)
{
    //template argument validation
    gil_function_requires<ImageViewConcept<SrcView>>();
    gil_function_requires<MutableImageViewConcept<DstView>>();
    gil_function_requires<ColorSpacesCompatibleConcept<
            typename color_space_type<SrcView>::type,
            typename color_space_type<DstView>::type>>();

    //deciding output channel type and creating functor
    typedef typename channel_type<SrcView>::type source_channel_t;
    typedef typename channel_type<DstView>::type result_channel_t;

    //iterate over the image chaecking each pixel value for the threshold
    for (size_t y = 0; y < src.height(); y++)
    {
        typename SrcView::x_iterator src_it = src.row_begin(y);
        typename DstView::x_iterator dst_it = dst.row_begin(y);

        for (size_t x = 0; x < src.width(); x++)
        {
            // static_transform(src_it[x], dst_it[x], calling_function);
            static_transform(src_it[x], dst_it[x], [threshold, max](source_channel_t px) -> result_channel_t{ return px >= threshold ? 0 : max; });
        }
    }
}

}} //namespace boost::gil

#endif //BOOST_GIL_THRESHOLD_HPP
