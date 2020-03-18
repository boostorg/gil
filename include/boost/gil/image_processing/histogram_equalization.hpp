//
// Copyright 2019 Debabrata Mandal <mandaldebabrata123@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_GIL_IMAGE_PROCESSING_HISTOGRAM_EQUALIZATION_HPP
#define BOOST_GIL_IMAGE_PROCESSING_HISTOGRAM_EQUALIZATION_HPP

#include <boost/gil/image.hpp>

#include <array>

namespace boost { namespace gil {

namespace detail {

/*
Histogram calculation :- 
1. Takes the number of channels as template parameter
2. min and max are containers of size num_channels; store the maximum and minimum pixel value seen per channel

Algorithm :- 
1. If it is a 8bit channeled image we do normal histogram computation
2. If it is greater than 8 bits or signed we return a clipped and scaled version of the histogram between min and max
   pixel values that is seen in the image.
3. This clipping method produces no difference in output image when used for histogram equalization
   but maight not be useful in general 
4. The reason why the above is correct can be shown by using the histogram equalization formula
5. We assume a non-uniform binning width of max(sizeof(channel_type), max - min) / 256 for simplicity
*/
template <typename SrcView,std::size_t channels>
void histogram_impl(SrcView const& src_view,
    std::array<std::array<std::size_t, 256>, channels>& histogram,
    std::array<typename channel_type<SrcView>::type, channels>& min,
    std::array<typename channel_type<SrcView>::type, channels>& max)
{
    using source_channel_t = typename channel_type<SrcView>::type;
    
    //Checking channel sizes i.e. for channel sizes more than 1 byte or signed values,
    //need to find max and min pixel intensities and scale the values to [0,255] appropriately
    if(sizeof(source_channel_t) > 1 || std::is_signed<source_channel_t>::value)
    {
        //Per channel min and max values found independently
        for(std::ptrdiff_t src_y = 0; src_y < src_view.height(); ++src_y)
        {
            typename SrcView::x_iterator src_it = src_view.row_begin(src_y);

            for(std::ptrdiff_t src_x = 0; src_x < src_view.width(); ++src_x)
            {
                std::ptrdiff_t c = 0;
                //finding the new minimum across channels using channel level algorithms
                static_for_each(src_it[src_x],[&](source_channel_t px) {
                    min[c] = px < min[c] ? px : min[c];
                    c++;
                });
                c = 0;
                //finding the new maximum across channels using channel level algorithms
                static_for_each(src_it[src_x],[&](source_channel_t px) {
                    max[c] = px > max[c] ? px : max[c];
                    c++;
                });
            }
        }

        //Histogram calculation after scaling intensities to lie in [0,255]
        for(std::ptrdiff_t src_y = 0; src_y < src_view.height(); ++src_y)
        {
            typename SrcView::x_iterator src_it = src_view.row_begin(src_y);

            for(std::ptrdiff_t src_x = 0; src_x < src_view.width(); ++src_x)
            {
                std::ptrdiff_t c = 0;
                static_for_each(src_it[src_x],[&](source_channel_t px) {
                    histogram[c][((px - min[c]) * 255) / (max[c] - min[c])]++;
                    c++;
                });
            }
        }
    }
    else
    {
        //Default intitialzation when unsigned 8 bit pixel depths
        min.fill(0);
        max.fill(255);
        //Histogram calculation for default case
        for(std::ptrdiff_t src_y = 0; src_y < src_view.height(); ++src_y)
        {
            typename SrcView::x_iterator src_it = src_view.row_begin(src_y);

            for(std::ptrdiff_t src_x = 0; src_x < src_view.width(); ++src_x)
            {
                std::ptrdiff_t c = 0;
                static_for_each(src_it[src_x],[&](source_channel_t px) {
                    histogram[c][px]++;
                    c++;
                });
            }
        }
    }
}

/*
Cumulative Histogram computation :-
1. Takes container class as input an d num_channels as template parameter
Algorithm : Iteratively compute the 1D cumulative histogram
*/
template <std::size_t channels>
void histogram_cdf_impl(
    std::array<std::array<std::size_t, 256>, channels>& hist_cdf)
{
    //Compute per channel Cumulative Density Function 
    for(std::size_t c = 0; c < channels; ++c)
    {
        for(std::size_t i = 1; i < 256; ++i)
        {
            hist_cdf[c][i] += hist_cdf[c][i-1];
        }
    }
}

}//namespace boost::gil::detail

/*
Histogram equalization function :-
1. Calls histogram_impl and histogram_cdf_impl
2. Uses std::array as container class to store histogram, min, max values per channel
3. The process described involves loss of significant digits when scaling and rescaling back

Algoithm :-
1. If histogram A is to be equalized compute the cumulative histogram of A.
2. Let CFD(A) refer to the cumulative histogram of A
3. For a uniform histogram A', CDF(A') = A'
4. We need to transfrom A to A' such that
5. CDF(A') = CDF(A) => A' = CDF(A)
6. Hence the pixel transform , px => histogram_of_ith_channel[px].
*/
template <typename SrcView, typename DstView>
void histogram_equalization(SrcView const& src_view, DstView const& dst_view)
{
    gil_function_requires<ImageViewConcept<SrcView>>();
    gil_function_requires<MutableImageViewConcept<DstView>>();
    static_assert(color_spaces_are_compatible
    <
        typename color_space_type<SrcView>::type,
        typename color_space_type<DstView>::type
    >::value, "Source and destination views must have same color space");
    
    // Defining channel type
    using source_channel_t = typename channel_type<SrcView>::type;
    using x_coord_t = typename SrcView::x_coord_t;
    using y_coord_t = typename SrcView::y_coord_t;

    x_coord_t const width = src_view.width();
    y_coord_t const height = src_view.height();
    std::size_t const channels = num_channels<SrcView>::value;

    //Initializations for min and max arrays to be filled while finding per channel extremas
    std::array<source_channel_t, channels>  min{
                std::numeric_limits<source_channel_t>::max()},
                                            max{
                std::numeric_limits<source_channel_t>::min()};

    std::array<std::array<std::size_t, 256>, channels> histogram{};

    //Passing channel size as template parameter
    detail::histogram_impl<SrcView,channels>(src_view, histogram, min, max);

    detail::histogram_cdf_impl<channels>(histogram);

    std::size_t num_pixels = (height * width);

    for(std::ptrdiff_t src_y = 0; src_y < height; ++src_y)
    {
        for(std::ptrdiff_t src_x = 0; src_x < width; ++src_x)
        {
            typename SrcView::x_iterator src_it = src_view.row_begin(src_y);
            typename DstView::x_iterator dst_it = dst_view.row_begin(src_y);

            for(std::size_t c = 0; c < channels; ++c)
            {
                // Performing the pixel transformation and rescaling back to  channel range
                dst_it[src_x][c] = (histogram[c][src_it[src_x][c]] * (max[c] - min[c])) / num_pixels;
            }
        }
    }
}

}} //namespace boost::gil

#endif // !BOOST_GIL_IMAGE_PROCESSING_HISTOGRAM_EQUALIZATION_HPP
