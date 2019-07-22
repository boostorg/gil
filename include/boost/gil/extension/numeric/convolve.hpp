//
// Copyright 2005-2007 Adobe Systems Incorporated
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_EXTENSION_NUMERIC_CONVOLVE_HPP
#define BOOST_GIL_EXTENSION_NUMERIC_CONVOLVE_HPP

#include <boost/gil/extension/numeric/algorithm.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>

#include <boost/gil/algorithm.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/gil/metafunctions.hpp>

#include <boost/assert.hpp>

#include <algorithm>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <vector>

namespace boost { namespace gil {

// 2D seperable convolutions and correlations

/// \ingroup ImageAlgorithms
/// \brief Boundary options for 1D correlations and convolutions
/// Use it as `enum class` with mutually exclusive constants, not as bitfields.
/// \todo TODO: Change to enum class?
enum /*class*/ convolve_boundary_option
{
    convolve_option_output_ignore,  /// do nothing to the output
    convolve_option_output_zero,    /// set the output to zero
    convolve_option_extend_padded,  /// assume the source boundaries to be padded already
    convolve_option_extend_zero,    /// assume the source boundaries to be zero
    convolve_option_extend_constant /// assume the source boundaries to be the boundary value
};

namespace detail {

/// \brief Compute the correlation of 1D kernel with the rows of an image
/// \tparam PixelAccum - TODO
/// \tparam SrcView Models ImageViewConcept
/// \tparam Kernel - TODO
/// \tparam DstView Models MutableImageViewConcept
/// \tparam Correlator - TODO
/// \param src_view
/// \param kernel - TODO
/// \param dst_view Destination where new computed values of pixels are assigned to
/// \param option - TODO
/// \param correlator - TODO
template
<
    typename PixelAccum,
    typename SrcView,
    typename Kernel,
    typename DstView,
    typename Correlator
>
void correlate_rows_impl(
    SrcView const& src_view,
    Kernel const& kernel,
    DstView const& dst_view,
    convolve_boundary_option option,
    Correlator correlator)
{
    BOOST_ASSERT(src_view.dimensions() == dst_view.dimensions());
    BOOST_ASSERT(kernel.size() != 0);

    if(kernel.size() == 1)
    {
        // Reduces to a multiplication
        view_multiplies_scalar<PixelAccum>(src_view, *kernel.begin(), dst_view);
        return;
    }

    using src_pixel_ref_t = typename pixel_proxy<typename SrcView::value_type>::type;
    using dst_pixel_ref_t = typename pixel_proxy<typename DstView::value_type>::type;
    using x_coord_t = typename SrcView::x_coord_t;
    using y_coord_t = typename SrcView::y_coord_t;

    x_coord_t const width = src_view.width();
    y_coord_t const height = src_view.height();
    if (width == 0)
        return;

    PixelAccum acc_zero;
    pixel_zeros_t<PixelAccum>()(acc_zero);
    if (option == convolve_option_output_ignore || option == convolve_option_output_zero)
    {
        typename DstView::value_type dst_zero;
        pixel_assigns_t<PixelAccum, dst_pixel_ref_t>()(acc_zero, dst_zero);
        if (width < static_cast<x_coord_t>(kernel.size()))
        {
            if (option == convolve_option_output_zero)
                fill_pixels(dst_view, dst_zero);
        }
        else
        {
            std::vector<PixelAccum> buffer(width);
            for (y_coord_t y = 0; y < height; ++y)
            {
                assign_pixels(src_view.row_begin(y), src_view.row_end(y), &buffer.front());
                typename DstView::x_iterator it_dst = dst_view.row_begin(y);
                if (option == convolve_option_output_zero)
                    std::fill_n(it_dst, kernel.left_size(), dst_zero);
                it_dst += kernel.left_size();
                correlator(&buffer.front(), &buffer.front() + width + 1 - kernel.size(),
                           kernel.begin(), it_dst);
                it_dst += width + 1 - kernel.size();
                if (option == convolve_option_output_zero)
                    std::fill_n(it_dst, kernel.right_size(), dst_zero);
            }
        }
    }
    else
    {
        std::vector<PixelAccum> buffer(width + kernel.size() - 1);
        for (y_coord_t y = 0; y < height; ++y)
        {
            PixelAccum *it_buffer = &buffer.front();
            if (option == convolve_option_extend_padded)
            {
                assign_pixels(
                    src_view.row_begin(y) - kernel.left_size(),
                    src_view.row_end(y) + kernel.right_size(),
                    it_buffer);
            }
            else if (option == convolve_option_extend_zero)
            {
                std::fill_n(it_buffer, kernel.left_size(), acc_zero);
                it_buffer += kernel.left_size();
                assign_pixels(src_view.row_begin(y), src_view.row_end(y), it_buffer);
                it_buffer += width;
                std::fill_n(it_buffer, kernel.right_size(), acc_zero);
            }
            else if (option == convolve_option_extend_constant)
            {
                PixelAccum filler;
                pixel_assigns_t<src_pixel_ref_t, PixelAccum>()(*src_view.row_begin(y), filler);
                std::fill_n(it_buffer, kernel.left_size(), filler);
                it_buffer += kernel.left_size();
                assign_pixels(src_view.row_begin(y), src_view.row_end(y), it_buffer);
                it_buffer += width;
                pixel_assigns_t<src_pixel_ref_t, PixelAccum>()(src_view.row_end(y)[-1], filler);
                std::fill_n(it_buffer, kernel.right_size(), filler);
            }

            correlator(
                &buffer.front(), &buffer.front() + width,
                kernel.begin(),
                dst_view.row_begin(y));
        }
    }
}

template <typename PixelAccum>
class correlator_n
{
public:
    correlator_n(std::size_t size) : size_(size) {}

    template <typename SrcIterator, typename KernelIterator, typename DstIterator>
    void operator()(
        SrcIterator src_begin,
        SrcIterator src_end,
        KernelIterator kernel_begin,
        DstIterator dst_begin)
    {
        correlate_pixels_n<PixelAccum>(src_begin, src_end, kernel_begin, size_, dst_begin);
    }

private:
    std::size_t size_{0};
};

template <std::size_t Size, typename PixelAccum>
struct correlator_k
{
    template <typename SrcIterator, typename KernelIterator, typename DstIterator>
    void operator()(
        SrcIterator src_begin,
        SrcIterator src_end,
        KernelIterator kernel_begin,
        DstIterator dst_begin)
    {
        correlate_pixels_k<Size, PixelAccum>(src_begin, src_end, kernel_begin, dst_begin);
    }
};

} // namespace detail

/// \ingroup ImageAlgorithms
/// \brief Correlate 1D variable-size kernel along the rows of image
/// \tparam PixelAccum TODO
/// \tparam SrcView Models ImageViewConcept
/// \tparam Kernel TODO
/// \tparam DstView Models MutableImageViewConcept
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView>
BOOST_FORCEINLINE
void correlate_rows(
    SrcView const& src_view,
    Kernel const& kernel,
    DstView const& dst_view,
    convolve_boundary_option option = convolve_option_extend_zero)
{
    detail::correlate_rows_impl<PixelAccum>(
        src_view, kernel, dst_view, option, detail::correlator_n<PixelAccum>(kernel.size()));
}

/// \ingroup ImageAlgorithms
/// \brief Correlate 1D variable-size kernel along the columns of image
/// \tparam PixelAccum TODO
/// \tparam SrcView Models ImageViewConcept
/// \tparam Kernel TODO
/// \tparam DstView Models MutableImageViewConcept
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView>
BOOST_FORCEINLINE
void correlate_cols(
    SrcView const& src_view,
    Kernel const& kernel,
    DstView const& dst_view,
    convolve_boundary_option option = convolve_option_extend_zero)
{
    correlate_rows<PixelAccum>(
        transposed_view(src_view), kernel, transposed_view(dst_view), option);
}

/// \ingroup ImageAlgorithms
/// \brief Convolve 1D variable-size kernel along the rows of image
/// \tparam PixelAccum TODO
/// \tparam SrcView Models ImageViewConcept
/// \tparam Kernel TODO
/// \tparam DstView Models MutableImageViewConcept
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView>
BOOST_FORCEINLINE
void convolve_rows(
    SrcView const& src_view,
    Kernel const& kernel,
    DstView const& dst_view,
    convolve_boundary_option option = convolve_option_extend_zero)
{
    correlate_rows<PixelAccum>(src_view, reverse_kernel(kernel), dst_view, option);
}

/// \ingroup ImageAlgorithms
/// \brief Convolve 1D variable-size kernel along the columns of image
/// \tparam PixelAccum TODO
/// \tparam SrcView Models ImageViewConcept
/// \tparam Kernel TODO
/// \tparam DstView Models MutableImageViewConcept
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView>
BOOST_FORCEINLINE
void convolve_cols(
    SrcView const& src_view,
    Kernel const& kernel,
    DstView const& dst_view,
    convolve_boundary_option option = convolve_option_extend_zero)
{
    convolve_rows<PixelAccum>(
        transposed_view(src_view), kernel, transposed_view(dst_view), option);
}

/// \ingroup ImageAlgorithms
/// \brief Convolve 1D variable-size kernel along both rows and columns of image
/// \tparam PixelAccum TODO
/// \tparam SrcView Models ImageViewConcept
/// \tparam Kernel TODO
/// \tparam DstView Models MutableImageViewConcept
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView>
BOOST_FORCEINLINE
void convolve(
    SrcView const& src_view,
    Kernel const& kernel,
    DstView const& dst_view,
    convolve_boundary_option option = convolve_option_extend_zero)
{
    convolve_rows<PixelAccum>(src_view, kernel, dst_view, option);
    convolve_cols<PixelAccum>(dst_view, kernel, dst_view, option);
}

/// \ingroup ImageAlgorithms
/// \brief Correlate 1D fixed-size kernel along the rows of image
/// \tparam PixelAccum TODO
/// \tparam SrcView Models ImageViewConcept
/// \tparam Kernel TODO
/// \tparam DstView Models MutableImageViewConcept
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView>
BOOST_FORCEINLINE
void correlate_rows_fixed(
    SrcView const& src_view,
    Kernel const& kernel,
    DstView const& dst_view,
    convolve_boundary_option option = convolve_option_extend_zero)
{
    using correlator = detail::correlator_k<Kernel::static_size, PixelAccum>;
    detail::correlate_rows_impl<PixelAccum>(src_view, kernel, dst_view, option, correlator{});
}

/// \ingroup ImageAlgorithms
/// \brief Correlate 1D fixed-size kernel along the columns of image
/// \tparam PixelAccum TODO
/// \tparam SrcView Models ImageViewConcept
/// \tparam Kernel TODO
/// \tparam DstView Models MutableImageViewConcept
template <typename PixelAccum,typename SrcView,typename Kernel,typename DstView>
BOOST_FORCEINLINE
void correlate_cols_fixed(
    SrcView const& src_view,
    Kernel const& kernel,
    DstView const& dst_view,
    convolve_boundary_option option = convolve_option_extend_zero)
{
    correlate_rows_fixed<PixelAccum>(
        transposed_view(src_view), kernel, transposed_view(dst_view), option);
}

/// \ingroup ImageAlgorithms
/// \brief Convolve 1D fixed-size kernel along the rows of image
/// \tparam PixelAccum TODO
/// \tparam SrcView Models ImageViewConcept
/// \tparam Kernel TODO
/// \tparam DstView Models MutableImageViewConcept
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView>
BOOST_FORCEINLINE
void convolve_rows_fixed(
    SrcView const& src_view,
    Kernel const& kernel,
    DstView const& dst_view,
    convolve_boundary_option option = convolve_option_extend_zero)
{
    correlate_rows_fixed<PixelAccum>(src_view, reverse_kernel(kernel), dst_view, option);
}

/// \ingroup ImageAlgorithms
/// \brief Convolve 1D fixed-size kernel along the columns of image
/// \tparam PixelAccum TODO
/// \tparam SrcView Models ImageViewConcept
/// \tparam Kernel TODO
/// \tparam DstView Models MutableImageViewConcept
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView>
BOOST_FORCEINLINE
void convolve_cols_fixed(
    SrcView const& src_view,
    Kernel const& kernel,
    DstView const& dst_view,
    convolve_boundary_option option = convolve_option_extend_zero)
{
    convolve_rows_fixed<PixelAccum>(
        transposed_view(src_view), kernel, transposed_view(dst_view), option);
}

}} // namespace boost::gil

#endif
