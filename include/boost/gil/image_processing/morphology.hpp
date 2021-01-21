//
// Copyright 2021 Prathamesh Tagore <prathameshtagore@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_GIL_IMAGE_PROCESSING_MORPHOLOGY_HPP
#define BOOST_GIL_IMAGE_PROCESSING_MORPHOLOGY_HPP

#include <boost/gil/extension/numeric/kernel.hpp>

#include <boost/gil/gray.hpp>
#include <boost/gil/algorithm.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_processing/threshold.hpp>

namespace gil = boost::gil;
namespace boost{
    namespace gil{
        enum class morphological_operations
            {
                dilate,
                erode,
                opening,
                closing,
                morphological_gradient,
                top_hat,
                black_hat
            };
namespace detail{
template <typename SrcView, typename DstView, typename Kernel>
void morph_impl(SrcView const& src_view, DstView const& dst_view, Kernel const& kernel,const int identifier)
{
    int flip_ker_row, flip_ker_col, row_boundary, col_boundary;
    typename channel_type<typename SrcView::value_type>::type max_overlapped_element,min_overlapped_element;
    for (std::ptrdiff_t view_row = 0; view_row < src_view.height(); ++view_row)
    {
        for (std::ptrdiff_t view_col = 0; view_col < src_view.width(); ++view_col)
        {
            max_overlapped_element = (std::numeric_limits<typename channel_type<SrcView>::type>::min)();
            min_overlapped_element = (std::numeric_limits<typename channel_type<SrcView>::type>::max)();
            for (std::size_t kernel_row = 0; kernel_row < kernel.size(); ++kernel_row)
            {
                flip_ker_row = kernel.size() - 1 - kernel_row;      // row index of flipped kernel

                for (std::size_t kernel_col = 0; kernel_col < kernel.size(); ++kernel_col)
                {
                    flip_ker_col = kernel.size() - 1 - kernel_col; // column index of flipped kernel

                    // index of input signal, used for checking boundary
                    row_boundary = view_row + (kernel.center_y() - flip_ker_row);
                    col_boundary = view_col + (kernel.center_x() - flip_ker_col);

                    // ignore input samples which are out of bound
                    if (row_boundary >= 0 && row_boundary < src_view.height() &&
                        col_boundary >= 0 && col_boundary < src_view.width())
                    {
                        if(src_view(col_boundary, row_boundary) > max_overlapped_element)
                            max_overlapped_element = src_view(col_boundary, row_boundary);
                        if(src_view(col_boundary, row_boundary) < min_overlapped_element)
                            min_overlapped_element = src_view(col_boundary, row_boundary);
                    }
                }
            }
            if(identifier)//identifier = 1 for dilation
                dst_view(view_col, view_row) = max_overlapped_element;
            else //identifier = 0 for erosion
                dst_view(view_col, view_row) = min_overlapped_element; 
        }
        }
    }

template <typename SrcView, typename DstView,typename Kernel>
void morph(SrcView const& src_view, DstView const& dst_view,Kernel const& ker_mat,const int identifier)
{
    using namespace boost::gil;
    BOOST_ASSERT(ker_mat.size() != 0 && src_view.dimensions() == dst_view.dimensions());
    gil::gray8_image_t intermediate_img(src_view.dimensions());
    for (std::size_t i = 0; i < src_view.num_channels(); i++)
    {
        morph_impl(
            nth_channel_view(src_view, i),
            nth_channel_view(view(intermediate_img), i),
            ker_mat,identifier
        );
    }
    gil::transform_pixels(view(intermediate_img),dst_view,[](const gil::gray8_pixel_t& p){return p[0];});
}

//This function calculates the difference between pixel values of first image_view and second
//image_view.This function can be used for rgb as well as grayscale images.
template <typename SrcView, typename DstView ,typename DiffView>
void difference_impl(SrcView const& src_view, DstView const& dst_view, DiffView const& diff_view)
{
    for (std::ptrdiff_t view_row = 0; view_row < src_view.height(); ++view_row)
        for (std::ptrdiff_t view_col = 0; view_col < src_view.width(); ++view_col)
            diff_view(view_col, view_row) = src_view(view_col,view_row) - dst_view(view_col,view_row);
}

template <typename SrcView, typename DstView, typename DiffView>
void difference(SrcView const& src_view, DstView const& dst_view , DiffView const& diff_view)
{

    BOOST_ASSERT(src_view.dimensions() == dst_view.dimensions());

    for (std::size_t i = 0; i < src_view.num_channels(); i++)
    {
        difference_impl(
            nth_channel_view(src_view, i),
            nth_channel_view(dst_view, i),
            nth_channel_view(diff_view, i)
        );
    }
}
}//namespace boost::gil::detail

// Dilation:Give the maximum overlapped value to the pixel overlapping with the center element of 
//structuring element.We can vary the number of times dilation happens by varying the 
// argument 'iterations' in the dilate function.
template <typename SrcView, typename DstView,typename Kernel>
void dilate(SrcView const& src_view, DstView const& dst_view,Kernel const& ker_mat,const int iterations)
{
    boost::gil::transform_pixels(src_view,dst_view,[](const boost::gil::gray8_pixel_t& p){return p[0];});
    for(int i=0;i<iterations;++i)
        morph(dst_view,dst_view,ker_mat,1);
}

//Erosion:Give the minimum overlapped value to the pixel overlapping with the center element of 
//structuring element.We can vary the number of times erosion happens by varying the 
//argument 'iterations' in the erode function.
template <typename SrcView, typename DstView,typename Kernel>
void erode(SrcView const& src_view, DstView const& dst_view,Kernel const& ker_mat,const int iterations)
{
    boost::gil::transform_pixels(src_view,dst_view,[](const boost::gil::gray8_pixel_t& p){return p[0];});
    for(int i=0;i<iterations;++i)
        morph(dst_view,dst_view,ker_mat,0);
}

//Opening:Opening is just another name of erosion followed by dilation.
//It is useful in removing noise.
template <typename SrcView, typename DstView,typename Kernel>
void opening(SrcView const& src_view, DstView const& dst_view,Kernel const& ker_mat)
{
    erode(src_view,dst_view,ker_mat,1);
    dilate(dst_view,dst_view,ker_mat,1);
}

//Closing:Closing is reverse of Opening, Dilation followed by Erosion.
//It is useful in closing small holes inside the foreground objects, or small black points 
//on the object.
template <typename SrcView, typename DstView,typename Kernel>
void closing(SrcView const& src_view, DstView const& dst_view,Kernel const& ker_mat)
{
    dilate(src_view,dst_view,ker_mat,1);
    erode(dst_view,dst_view,ker_mat,1);
}

//Morphological Gradient:It is the difference between dilation and erosion of an image.
//The result will look like the outline of the object.
template <typename SrcView, typename DstView, typename Kernel>
void morphological_gradient(SrcView const& src_view, DstView const& dst_view, Kernel const& ker_mat)
{
    using namespace boost::gil;
    gray8_image_t int_dilate(src_view.dimensions()),int_erode(src_view.dimensions());
    dilate(src_view,view(int_dilate),ker_mat,1);
    erode(src_view,view(int_erode),ker_mat,1);
    difference(view(int_dilate),view(int_erode),dst_view);
}

//Top Hat:It is the difference between input image and Opening of the image.
template <typename SrcView, typename DstView, typename Kernel>
void top_hat(SrcView const& src_view, DstView const& dst_view, Kernel const& ker_mat)
{
    using namespace boost::gil;
    gray8_image_t int_opening(src_view.dimensions());
    opening(src_view,view(int_opening),ker_mat);
    difference(src_view,view(int_opening),dst_view);
}

//Black Hat:It is the difference between the closing of the input image and input image.
template <typename SrcView, typename DstView, typename Kernel>
void black_hat(SrcView const& src_view, DstView const& dst_view, Kernel const& ker_mat)
{
    using namespace boost::gil;
    gray8_image_t int_closing(src_view.dimensions());
    closing(src_view,view(int_closing),ker_mat);
    difference(view(int_closing), src_view,dst_view);
}
    }
}//namespace boost::gil
#endif //BOOST_GIL_IMAGE_PROCESSING_MORPHOLOGY_HPP
