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
#include <array>
#include <type_traits>
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
    using source_channel_t = typename channel_type<SrcView>::type;
    using result_channel_t = typename channel_type<DstView>::type;

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
    using result_channel_t = typename channel_type<DstView>::type;

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
    using source_channel_t = typename channel_type<SrcView>::type;
    using result_channel_t = typename channel_type<DstView>::type;

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

namespace detail{

template <typename SrcView, typename DstView>
void otsu_impl(SrcView const& src_view, DstView const& dst_view, threshold_direction direction)
{
    //deciding output channel type and creating functor
    using source_channel_t = typename channel_type<SrcView>::type;

    std::array<std::size_t, 256> histogram{};
    //initial value of min is set to maximum possible value to compare histogram data
    //initial value of max is set to minimum possible value to compare histogram data
    auto min = std::numeric_limits<source_channel_t>::max(),
        max = std::numeric_limits<source_channel_t>::min();

    if (sizeof(source_channel_t) > 1 || std::is_signed<source_channel_t>::value)
    {
        //iterate over the image to find the min and max pixel values
        for (std::ptrdiff_t y = 0; y < src_view.height(); y++)
        {
            typename SrcView::x_iterator src_it = src_view.row_begin(y);
            for (std::ptrdiff_t x = 0; x < src_view.width(); x++)
            {
                if (src_it[x] < min) min = src_it[x];
                if (src_it[x] > min) min = src_it[x];
            }
        }

        //making histogram
        for (std::ptrdiff_t y = 0; y < src_view.height(); y++)
        {
            typename SrcView::x_iterator src_it = src_view.row_begin(y);

            for (std::ptrdiff_t x = 0; x < src_view.width(); x++)
            {
                histogram[((src_it[x] - min) * 255) / (max - min)]++;
            }
        }
    }
    else
    {
        //making histogram
        for (std::ptrdiff_t y = 0; y < src_view.height(); y++)
        {
            typename SrcView::x_iterator src_it = src_view.row_begin(y);

            for (std::ptrdiff_t x = 0; x < src_view.width(); x++)
            {
                histogram[src_it[x]]++;
            }
        }
    }

    //histData = histogram data
    //sum = total (background + foreground)
    //sumB = sum background
    //wB = weight background
    //wf = weight foreground
    //varMax = tracking the maximum known value of between class variance
    //mB = mu background
    //mF = mu foreground
    //varBeetween = between class variance
    //http://www.labbookpages.co.uk/software/imgProc/otsuThreshold.html
    //https://www.ipol.im/pub/art/2016/158/
    std::ptrdiff_t total_pixel = src_view.height() * src_view.width();
    std::ptrdiff_t sum_total = 0, sum_back = 0;
    std::size_t weight_back = 0, weight_fore = 0, threshold = 0;
    double var_max = 0, mean_back, mean_fore, var_intra_class;

    for (std::size_t t = 0; t < 256; t++)
    {
        sum_total += t * histogram[t];
    }

    for (int t = 0; t < 256; t++)
    {
        weight_back += histogram[t];               // Weight Background
        if (weight_back == 0) continue;

        weight_fore = total_pixel - weight_back;          // Weight Foreground
        if (weight_fore == 0) break;

        sum_back += t * histogram[t];

        mean_back = sum_back / weight_back;            // Mean Background
        mean_fore = (sum_total - sum_back) / weight_fore;    // Mean Foreground

        // Calculate Between Class Variance
        var_intra_class = weight_back * weight_fore * (mean_back - mean_fore) * (mean_back - mean_fore);

        // Check if new maximum found
        if (var_intra_class > var_max) {
            var_max = var_intra_class;
            threshold = t;
        }
    }
    if (sizeof(source_channel_t) > 1 && std::is_unsigned<source_channel_t>::value)
    {
        threshold_binary(src_view, dst_view, (threshold * (max - min) / 255) + min, direction);
    }
    else {
        threshold_binary(src_view, dst_view, threshold, direction);
    }
}
} //namespace detail

template <typename SrcView, typename DstView>
void threshold_optimal
(
    SrcView const& src_view,
    DstView const& dst_view,
    threshold_optimal_value mode = threshold_optimal_value::otsu,
    threshold_direction direction = threshold_direction::regular
)
{
    if (mode == threshold_optimal_value::otsu)
    {
        for (std::size_t i = 0; i < src_view.num_channels(); i++)
        {
            detail::otsu_impl
                (nth_channel_view(src_view, i), nth_channel_view(dst_view, i), direction);
        }
    }
}

}} //namespace boost::gil

#endif //BOOST_GIL_IMAGE_PROCESSING_THRESHOLD_HPP
