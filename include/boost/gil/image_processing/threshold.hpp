//
// Copyright 2019 Miral Shah <miralshah2211@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_GIL_IMAGE_PROCESSING_THRESHOLD_HPP
#define BOOST_GIL_IMAGE_PROCESSING_THRESHOLD_HPP

#include <limits>
#include <boost/gil/image.hpp>

namespace boost { namespace gil {


namespace detail {

template
<
    typename SourceChannelT,
    typename ResultChannelT,
    typename SrcView,
    typename DstView,
    typename Operator
>
void threshold_impl(SrcView const& src_view, DstView const& dst_view, Operator const& threshold_op)
{
    gil_function_requires<ImageViewConcept<SrcView>>();
    gil_function_requires<MutableImageViewConcept<DstView>>();
    gil_function_requires<ColorSpacesCompatibleConcept
    <
        typename color_space_type<SrcView>::type,
        typename color_space_type<DstView>::type>
    >();
    static_assert(color_spaces_are_compatible
    <
        typename color_space_type<SrcView>::type,
        typename color_space_type<DstView>::type
    >::value, "Source and destination views must have pixels with the same color space");

    //iterate over the image chaecking each pixel value for the threshold
    for (std::ptrdiff_t y = 0; y < src_view.height(); y++)
    {
        typename SrcView::x_iterator src_it = src_view.row_begin(y);
        typename DstView::x_iterator dst_it = dst_view.row_begin(y);

        for (std::ptrdiff_t x = 0; x < src_view.width(); x++)
        {
            static_transform(src_it[x], dst_it[x], threshold_op);
        }
    }
}

} //namespace boost::gil::detail

enum class threshold_direction { regular, inverse };
enum class threshold_optimal_value { otsu, triangle };
enum class threshold_truncate_mode { threshold, zero };

/*!
Takes an image view and performes binary thresholding operation on each chennel.
If direction is regular:
values greater than or equal to threshold_value will be set to max_value else set to 0
If direction is inverse:
values greater than or equal to threshold_value will be set to 0 else set to max_value
*/
template <typename SrcView, typename DstView>
void threshold_binary
(
    SrcView const& src_view,
    DstView const& dst_view,
    typename channel_type<DstView>::type threshold_value,
    typename channel_type<DstView>::type max_value,
    threshold_direction direction = threshold_direction::regular
)
{
    //deciding output channel type and creating functor
    typedef typename channel_type<SrcView>::type source_channel_t;
    typedef typename channel_type<DstView>::type result_channel_t;

    if (direction == threshold_direction::regular)
    {
        detail::threshold_impl<source_channel_t, result_channel_t>(src_view, dst_view,
            [threshold_value, max_value](source_channel_t px) -> result_channel_t {
                return px > threshold_value ? max_value : 0;
            });
    }
    else
    {
        detail::threshold_impl<source_channel_t, result_channel_t>(src_view, dst_view,
            [threshold_value, max_value](source_channel_t px) -> result_channel_t {
                return px > threshold_value ? 0 : max_value;
            });
    }
}

/*!
Takes an image view and performes binary thresholding operation on each chennel.
If direction is regular:
values more or equal to threshold_value will be set to maximum numeric limit of channel else 0
If direction is inverse:
values more than or equal to threshold_value will be set to 0 else maximum numeric limit of channel
*/
template <typename SrcView, typename DstView>
void threshold_binary
(
    SrcView const& src_view,
    DstView const& dst_view,
    typename channel_type<DstView>::type threshold_value,
    threshold_direction direction = threshold_direction::regular
)
{
    //deciding output channel type and creating functor
    typedef typename channel_type<SrcView>::type source_channel_t;
    typedef typename channel_type<DstView>::type result_channel_t;

    result_channel_t max_value = std::numeric_limits<result_channel_t>::max();

    threshold_binary(src_view, dst_view, threshold_value, max_value, direction);
}

/*!
Takes an image view and performes truncating threshold operation on each chennel.
If mode is truncate and direction is regular:
values greater than threshold_value will be set to threshold_value else no change
If mode is truncate and direction is inverse:
values less than threshold_value will be set to threshold_value else no change
If mode is zeo and direction is regular:
values less than threshold_value will be set to 0 else no change
If mode is zero and direction is inverse:
values more than threshold_value will be set to 0 else no change
*/
template <typename SrcView, typename DstView>
void threshold_truncate
(
    SrcView const& src_view,
    DstView const& dst_view,
    typename channel_type<DstView>::type threshold_value,
    threshold_truncate_mode mode = threshold_truncate_mode::threshold,
    threshold_direction direction = threshold_direction::regular
)
{
    //deciding output channel type and creating functor
    typedef typename channel_type<SrcView>::type source_channel_t;
    typedef typename channel_type<DstView>::type result_channel_t;

    std::function<result_channel_t(source_channel_t)> threshold_logic;

    if (mode == threshold_truncate_mode::threshold)
    {
        if (direction == threshold_direction::regular)
        {
            detail::threshold_impl<source_channel_t, result_channel_t>(src_view, dst_view,
                [threshold_value](source_channel_t px) -> result_channel_t {
                    return px > threshold_value ? threshold_value : px;
                });
        }
        else
        {
            detail::threshold_impl<source_channel_t, result_channel_t>(src_view, dst_view,
                [threshold_value](source_channel_t px) -> result_channel_t {
                    return px > threshold_value ? px : threshold_value;
                });
        }
    }
    else
    {
        if (direction == threshold_direction::regular)
        {
            detail::threshold_impl<source_channel_t, result_channel_t>(src_view, dst_view,
                [threshold_value](source_channel_t px) -> result_channel_t {
                    return px > threshold_value ? px : 0;
                });
        }
        else
        {
            detail::threshold_impl<source_channel_t, result_channel_t>(src_view, dst_view,
                [threshold_value](source_channel_t px) -> result_channel_t {
                    return px > threshold_value ? 0 : px;
                });
        }
    }
}

}} //namespace boost::gil

#endif //BOOST_GIL_IMAGE_PROCESSING_THRESHOLD_HPP
