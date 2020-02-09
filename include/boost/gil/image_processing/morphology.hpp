//
// Copyright 2019 Ayush Bansal <abansal1008@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_GIL_IMAGE_PROCESSING_MORPHOLOGY_HPP
#define BOOST_GIL_IMAGE_PROCESSING_MORPHOLOGY_HPP

#include <boost/assert.hpp>
#include <boost/gil.hpp>

namespace boost { namespace gil {

// SE refers to Structuring Element
// Result type after convolution with the structuring element
enum class morphology_operator{
    erode,
    dilate
};

namespace detail {

template <typename SrcView, typename DstView, typename StructElement>
void convolve_with_struct_element(SrcView const &src_view, DstView &dst_view,
    StructElement const &struct_elem, std::ptrdiff_t src_y, std::ptrdiff_t src_x,
    morphology_operator const &op)
{
    // Template argument validation
    gil_function_requires<ImageViewConcept<SrcView>>();
    gil_function_requires<MutableImageViewConcept<DstView>>();

    using source_channel_t = typename channel_type<SrcView>::type;

    // Initial min and max values to be replaced after apply morphplogical operator
    auto min = (std::numeric_limits<source_channel_t>::max());
    auto max = (std::numeric_limits<source_channel_t>::min());

    std::size_t struct_elem_height = struct_elem.size();
    std::size_t struct_elem_width = struct_elem[0].size();

    for(std::ptrdiff_t struct_elem_y = 0; struct_elem_y < struct_elem_height; ++struct_elem_y)
    {
        // The cooresponding value to be read from src_view is calculated as
        // (src_y + struct_elem_y - struct_elem_height/2, src_x + struct_elem_x - struct_elem_width/2)
        // NOTE: This calculation is only valid when the struct_elem dimensions are odd.
        auto y_to_read = src_y + struct_elem_y - struct_elem_height/2;
        if(y_to_read < 0 || y_to_read >= src_view.height())
            continue;
        typename SrcView::x_iterator src_it = src_view.row_begin(y_to_read);

        for(std::ptrdiff_t struct_elem_x = 0; struct_elem_x < struct_elem_width; ++struct_elem_x)
        {
            // We need to consider only those positions which have 1 in Structuring Element
            if(struct_elem[struct_elem_x][struct_elem_y] == 1)
            {
                auto x_to_read = src_x + struct_elem_x - struct_elem_width/2;
                if(x_to_read < 0 || x_to_read >= src_view.width())
                    continue;
                auto src_point_val = src_it[x_to_read];
                if (src_point_val < min)
                    min = src_point_val;
                if (src_point_val > max)
                    max = src_point_val;
            }
        }
    }

    typename DstView::x_iterator dst_it = dst_view.row_begin(src_y);

    if (op == morphology_operator::erode)
        dst_it[src_x] = min;
    else if(op == morphology_operator::dilate)
        dst_it[src_x] = max;
}

template <typename SrcView, typename DstView, typename StructElement>
void morph_impl(SrcView const &src_view, DstView &dst_view, StructElement const &struct_elem, morphology_operator const &op)
{
    BOOST_ASSERT(src_view.dimensions() == dst_view.dimensions());
    BOOST_ASSERT(struct_elem.size() != 0 && struct_elem[0].size() != 0);

    std::size_t height = src_view.height();
    std::size_t width = dst_view.width();

    for(std::ptrdiff_t src_y = 0; src_y < height; ++src_y)
    {
        for(std::ptrdiff_t src_x = 0; src_x < width; ++src_x)
        {
            convolve_with_struct_element<SrcView, DstView, StructElement>(src_view, dst_view, struct_elem, src_y, src_x, op);
        }
    }
}

} //namespace boost::gil::detail

/// \addtogroup ImageProcessing
/// @{

/// \brief Performs Morphological Dilation of the given image with a given Structuring Element.
/// \param src_view - The source image view.
/// \param dst_view - The destination image view (after performing dilation).
/// \param struct_elem - The Structuing Element. Should be a 2D-array type consiting of binary int values (0 and 1).
/// \tparam SrcView type of source image. Should confirm to gil::ImgaeViewConcept.
/// \tparam DstView type of destination image. Should confirm to gil::MutableImageViewConcept.
/// \tparam StructElement type of structuring element. Should provide size() and operator[].
template <typename SrcView, typename DstView, typename StructElement>
void dilate(SrcView const &src_view, DstView &dst_view, StructElement const &struct_elem)
{
    morph_impl(src_view, dst_view, struct_elem, morphology_operator::dilate);
}

/// \brief Performs Morphological Erosion of the given image with a given Structuring Element.
/// \param src_view - The source image view.
/// \param dst_view - The destination image view (after performing dilation).
/// \param struct_elem - The Structuing Element. Should be a 2D-array type consiting of binary int values (0 and 1).
/// \tparam SrcView type of source image. Should confirm to gil::ImgaeViewConcept.
/// \tparam DstView type of destination image. Should confirm to gil::MutableImageViewConcept.
/// \tparam StructElement type of structuring element. Should provide size() and operator[].
template <typename SrcView, typename DstView, typename StructElement>
void erode(SrcView const &src_view, DstView &dst_view, StructElement const &struct_elem)
{
    morph_impl(src_view, dst_view, struct_elem, morphology_operator::erode);
}

/// @}

}} //namespace boost::gil

#endif //BOOST_GIL_IMAGE_PROCESSING_MORPHOLOGY_HPP
