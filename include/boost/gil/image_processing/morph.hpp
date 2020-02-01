//
// Copyright 2019 Ayush Bansal <abansal1008@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_GIL_IMAGE_PROCESSING_MORPH_HPP
#define BOOST_GIL_IMAGE_PROCESSING_MORPH_HPP

#include <boost/assert.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/gil.hpp>
#include <boost/multi_array.hpp>

using namespace boost::gil;

namespace boost { namespace gil {

// SE refers to Structuring Element
// Result type after convolution with the structuring element
enum class MorphOperator{
    ERODE,
    DILATE
};

template <typename SrcView, typename DstView, typename SE>
void convolve_with_SE(SrcView const &src_view, DstView &dst_view, SE const &se, std::size_t src_y, std::size_t src_x, MorphOperator const &op)
{
    // Template argument validation
    gil_function_requires<ImageViewConcept<SrcView>>();
    gil_function_requires<MutableImageViewConcept<DstView>>();

    BOOST_ASSERT(se.num_dimensions() == 2);

    using source_channel_t = typename channel_type<SrcView>::type;

    // Initial min and max values to be replaced after apply morphplogical operator
    auto min = (std::numeric_limits<source_channel_t>::max());
    auto max = (std::numeric_limits<source_channel_t>::min());

    std::size_t se_height = se.shape()[0];
    std::size_t se_width = se.shape()[1];

    // Iterating for every position in the Structuring Element
    for(std::size_t se_y = 0; se_y < se_height; ++se_y)
    {
        // The cooresponding value to be read from src_view is calculated as (src_y + se_y - se_height/2, src_x + se_x - se_width/2)
        // NOTE: This calculation is only valid when the SE dimensions are odd.
        typename SrcView::x_iterator src_it = src_view.row_begin(src_y + se_y - se_height/2);

      	for(std::size_t se_x = 0; se_x < se_width; ++se_x)
      	{
            // We need to consider only those positions which have 1 in Structuring Element
    		if(se[se_x][se_y] == 1)
    		{
      			auto src_point_val = src_it[src_x + se_x - se_width/2];
                if (src_point_val < min) min = src_point_val;
                if (src_point_val > max) max = src_point_val;
    		}
      	}
    }

    typename DstView::x_iterator dst_it = dst_view.row_begin(src_y);

    if (op == MorphOperator::ERODE) dst_it[src_x] = min;
    else if(op == MorphOperator::DILATE) dst_it[src_x] = max;
}

template <typename SrcView, typename DstView, typename SE>
void morph_impl(SrcView const &src_view, DstView &dst_view, SE const &se, MorphOperator const &op)
{
    BOOST_ASSERT(src_view.dimensions() == dst_view.dimensions());

    std::size_t height = src_view.height();
    std::size_t width = dst_view.width();

    // The offsets denote the boundaries that will be ignored by morphological operator
    std::size_t offset_height = se.shape()[0]/2;
    std::size_t offset_width = se.shape()[1]/2;

    // Iterate over every possible pixel in src_view
    for(std::size_t src_y = offset_height; src_y < height - offset_width; ++src_y)
    {
    	for(std::size_t src_x = offset_width; src_x < width - offset_width; ++src_x)
    	{
    		convolve_with_SE<SrcView, DstView, SE>(src_view, dst_view, se, src_y, src_x, op);
    	}
    }
}

template <typename SrcView, typename DstView, typename SE>
void dilate(SrcView const &src_view, DstView &dst_view, SE const &se)
{
    morph_impl(src_view, dst_view, se, MorphOperator::DILATE);
}

template <typename SrcView, typename DstView, typename SE>
void erode(SrcView const &src_view, DstView &dst_view, SE const &se)
{
    morph_impl(src_view, dst_view, se, MorphOperator::ERODE);
}

}} //namespace boost::gil

#endif //BOOST_GIL_IMAGE_PROCESSING_MORPH_HPP
