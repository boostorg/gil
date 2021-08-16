//
// Copyright 2005-2007 Adobe Systems Incorporated
// Copyright 2019 Miral Shah <miralshah2211@gmail.com>
// Copyright 2021 Prathamesh Tagore <prathameshtagore@gmail.com>
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

// 2D spatial seperable convolutions and cross-correlations

namespace detail {

/// \brief Computes the cross-correlation of 1D kernel with rows of an image.
/// \tparam PixelAccum - Specifies tha data type which will be used for creating buffer container 
/// utilized for holding source image pixels after applying appropriate boundary manipulations.
/// \tparam SrcView - Specifies the type of gil view of source image which is to be row correlated
/// with the kernel.
/// \tparam Kernel - Specifies the type of 1D kernel which will be row correlated with source image.
/// \tparam DstView -  Specifies the type of gil view which will store the result of row
/// correlation between source image and kernel.
/// \tparam Correlator - Specifies the type of correlator which should be used for performing 
/// correlation.
/// \param src_view - Gil view of source image used in correlation.
/// \param kernel - 1D kernel which will be correlated with source image.
/// \param dst_view - Gil view which will store the result of row correlation between "src_view"
/// and "kernel".
/// \param option - Specifies the manner in which boundary pixels of "dst_view" should be computed.
/// \param correlator - Correlator which will be used for performing correlation.
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
    boundary_option option,
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
    if (option == boundary_option::output_ignore || option == boundary_option::output_zero)
    {
        typename DstView::value_type dst_zero;
        pixel_assigns_t<PixelAccum, dst_pixel_ref_t>()(acc_zero, dst_zero);
        if (width < static_cast<x_coord_t>(kernel.size()))
        {
            if (option == boundary_option::output_zero)
                fill_pixels(dst_view, dst_zero);
        }
        else
        {
            std::vector<PixelAccum> buffer(width);
            for (y_coord_t y = 0; y < height; ++y)
            {
                assign_pixels(src_view.row_begin(y), src_view.row_end(y), &buffer.front());
                typename DstView::x_iterator it_dst = dst_view.row_begin(y);
                if (option == boundary_option::output_zero)
                    std::fill_n(it_dst, kernel.left_size(), dst_zero);
                it_dst += kernel.left_size();
                correlator(&buffer.front(), &buffer.front() + width + 1 - kernel.size(),
                           kernel.begin(), it_dst);
                it_dst += width + 1 - kernel.size();
                if (option == boundary_option::output_zero)
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
            if (option == boundary_option::extend_padded)
            {
                assign_pixels(
                    src_view.row_begin(y) - kernel.left_size(),
                    src_view.row_end(y) + kernel.right_size(),
                    it_buffer);
            }
            else if (option == boundary_option::extend_zero)
            {
                std::fill_n(it_buffer, kernel.left_size(), acc_zero);
                it_buffer += kernel.left_size();
                assign_pixels(src_view.row_begin(y), src_view.row_end(y), it_buffer);
                it_buffer += width;
                std::fill_n(it_buffer, kernel.right_size(), acc_zero);
            }
            else if (option == boundary_option::extend_constant)
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

/// \brief Provides functionality for performing 1D correlation between the kernel and a buffer
/// storing row pixels of source image. Kernel size is to be provided through constructor for all
/// instances.
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

/// \brief Provides functionality for performing 1D correlation between the kernel and a buffer
/// storing row pixels of source image. Kernel size is a template parameter and must be 
/// compulsorily specified while using.
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
/// \brief Correlate 1D variable-size kernel along the rows of image.
/// \tparam PixelAccum Specifies tha data type which will be used while creating buffer container 
/// which is utilized for holding source image pixels after applying appropriate boundary
/// manipulations.
/// \tparam SrcView Models ImageViewConcept
/// \tparam Kernel Specifies the type of 1D kernel which will be row correlated with source image.
/// \tparam DstView Models MutableImageViewConcept
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView>
BOOST_FORCEINLINE
void correlate_rows(
    SrcView const& src_view,
    Kernel const& kernel,
    DstView const& dst_view,
    boundary_option option = boundary_option::extend_zero)
{
    detail::correlate_rows_impl<PixelAccum>(
        src_view, kernel, dst_view, option, detail::correlator_n<PixelAccum>(kernel.size()));
}

/// \ingroup ImageAlgorithms
/// \brief Correlates 1D variable-size kernel along the columns of image.
/// \tparam PixelAccum Specifies tha data type which will be used for creating buffer container 
/// utilized for holding source image pixels after applying appropriate boundary manipulations.
/// \tparam SrcView Models ImageViewConcept
/// \tparam Kernel Specifies the type of 1D kernel which will be column correlated with source
/// image.
/// \tparam DstView Models MutableImageViewConcept
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView>
BOOST_FORCEINLINE
void correlate_cols(
    SrcView const& src_view,
    Kernel const& kernel,
    DstView const& dst_view,
    boundary_option option = boundary_option::extend_zero)
{
    correlate_rows<PixelAccum>(
        transposed_view(src_view), kernel, transposed_view(dst_view), option);
}

/// \ingroup ImageAlgorithms
/// \brief Convolves 1D variable-size kernel along the rows of image.
/// \tparam PixelAccum Specifies tha data type which will be used for creating buffer container 
/// utilized for holding source image pixels after applying appropriate boundary manipulations.
/// \tparam SrcView Models ImageViewConcept
/// \tparam Kernel Specifies the type of 1D kernel which will be row convoluted with source image.
/// \tparam DstView Models MutableImageViewConcept
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView>
BOOST_FORCEINLINE
void convolve_rows(
    SrcView const& src_view,
    Kernel const& kernel,
    DstView const& dst_view,
    boundary_option option = boundary_option::extend_zero)
{
    correlate_rows<PixelAccum>(src_view, reverse_kernel(kernel), dst_view, option);
}

/// \ingroup ImageAlgorithms
/// \brief Convolves 1D variable-size kernel along the columns of image.
/// \tparam PixelAccum Specifies tha data type which will be used for creating buffer container 
/// utilized for holding source image pixels after applying appropriate boundary manipulations.
/// \tparam SrcView Models ImageViewConcept
/// \tparam Kernel Specifies the type of 1D kernel which will be column convoluted with source
/// image.
/// \tparam DstView Models MutableImageViewConcept
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView>
BOOST_FORCEINLINE
void convolve_cols(
    SrcView const& src_view,
    Kernel const& kernel,
    DstView const& dst_view,
    boundary_option option = boundary_option::extend_zero)
{
    convolve_rows<PixelAccum>(
        transposed_view(src_view), kernel, transposed_view(dst_view), option);
}

/// \ingroup ImageAlgorithms
/// \brief Correlate 1D fixed-size kernel along the rows of image.
/// \tparam PixelAccum Specifies tha data type which will be used for creating buffer container 
/// utilized for holding source image pixels after applying appropriate boundary manipulations.
/// \tparam SrcView Models ImageViewConcept
/// \tparam Kernel Specifies the type of 1D kernel which will be row correlated with source image.
/// \tparam DstView Models MutableImageViewConcept
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView>
BOOST_FORCEINLINE
void correlate_rows_fixed(
    SrcView const& src_view,
    Kernel const& kernel,
    DstView const& dst_view,
    boundary_option option = boundary_option::extend_zero)
{
    using correlator = detail::correlator_k<Kernel::static_size, PixelAccum>;
    detail::correlate_rows_impl<PixelAccum>(src_view, kernel, dst_view, option, correlator{});
}

/// \ingroup ImageAlgorithms
/// \brief Correlate 1D fixed-size kernel along the columns of image
/// \tparam PixelAccum Specifies tha data type which will be used for creating buffer container 
/// utilized for holding source image pixels after applying appropriate boundary manipulations.
/// \tparam SrcView Models ImageViewConcept
/// \tparam Kernel Specifies the type of 1D kernel which will be column correlated with source
/// image.
/// \tparam DstView Models MutableImageViewConcept
template <typename PixelAccum,typename SrcView,typename Kernel,typename DstView>
BOOST_FORCEINLINE
void correlate_cols_fixed(
    SrcView const& src_view,
    Kernel const& kernel,
    DstView const& dst_view,
    boundary_option option = boundary_option::extend_zero)
{
    correlate_rows_fixed<PixelAccum>(
        transposed_view(src_view), kernel, transposed_view(dst_view), option);
}

/// \ingroup ImageAlgorithms
/// \brief Convolve 1D fixed-size kernel along the rows of image
/// \tparam PixelAccum Specifies tha data type which will be used for creating buffer container 
/// utilized for holding source image pixels after applying appropriate boundary manipulations.
/// \tparam SrcView Models ImageViewConcept
/// \tparam Kernel Specifies the type of 1D kernel which will be row convolved with source image.
/// \tparam DstView Models MutableImageViewConcept
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView>
BOOST_FORCEINLINE
void convolve_rows_fixed(
    SrcView const& src_view,
    Kernel const& kernel,
    DstView const& dst_view,
    boundary_option option = boundary_option::extend_zero)
{
    correlate_rows_fixed<PixelAccum>(src_view, reverse_kernel(kernel), dst_view, option);
}

/// \ingroup ImageAlgorithms
/// \brief Convolve 1D fixed-size kernel along the columns of image
/// \tparam PixelAccum Specifies tha data type which will be used for creating buffer container 
/// utilized for holding source image pixels after applying appropriate boundary manipulations.
/// \tparam SrcView Models ImageViewConcept
/// \tparam Kernel Specifies the type of 1D kernel which will be column convolved with source
/// image.
/// \tparam DstView Models MutableImageViewConcept
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView>
BOOST_FORCEINLINE
void convolve_cols_fixed(
    SrcView const& src_view,
    Kernel const& kernel,
    DstView const& dst_view,
    boundary_option option = boundary_option::extend_zero)
{
    convolve_rows_fixed<PixelAccum>(
        transposed_view(src_view), kernel, transposed_view(dst_view), option);
}

namespace detail
{

/// \ingroup ImageAlgorithms
/// \brief Convolve 1D variable-size kernel along both rows and columns of image
/// \tparam PixelAccum Specifies tha data type which will be used for creating buffer container 
/// utilized for holding source image pixels after applying appropriate boundary manipulations.
/// \tparam SrcView Models ImageViewConcept
/// \tparam Kernel Specifies the type of 1D kernel which will be used for 1D row and column
/// convolution.
/// \tparam DstView Models MutableImageViewConcept
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView>
BOOST_FORCEINLINE
void convolve_1d(
    SrcView const& src_view,
    Kernel const& kernel,
    DstView const& dst_view,
    boundary_option option = boundary_option::extend_zero)
{
    convolve_rows<PixelAccum>(src_view, kernel, dst_view, option);
    convolve_cols<PixelAccum>(dst_view, kernel, dst_view, option);
}

/// \brief Provides functionality for performing correlation between the kernel and buffer. 
/// Kernel size is to be provided through constructor for all instances. 
/// This correlator is specifically used in 2D correlation.
template <typename PixelAccum>
class correlator_n_2d
{
public:
    correlator_n_2d(std::size_t kernel_dimension) : _kernel_dimension(kernel_dimension) {}

    template <typename SrcIterator, typename KernelIterator, typename DstIterator>
    void operator()(
        SrcIterator src_begin,
        std::size_t src_size,
        KernelIterator kernel_begin,
        DstIterator dst_begin)
    {
        correlate_pixels_n_2d<PixelAccum>(src_begin, src_size, kernel_begin, _kernel_dimension, 
            dst_begin);
    }

private:
    std::size_t _kernel_dimension{0};
};

/// \brief Provides functionality for performing correlation between the kernel and buffer.
/// Kernel size is a template parameter and must be compulsorily specified while using.
/// This correlator is specifically used in 2D correlation.
template <std::size_t kernel_dimension, typename PixelAccum>
struct correlator_k_2d
{
    template <typename SrcIterator, typename KernelIterator, typename DstIterator>
    void operator()(
        SrcIterator src_begin,
        std::size_t src_size,
        KernelIterator kernel_begin,
        DstIterator dst_begin)
    {
        correlate_pixels_k_2d<kernel_dimension, PixelAccum>(src_begin, src_size, kernel_begin, 
            dst_begin);
    }
};

/// \brief Computes the cross-correlation of a 2D kernel with an image.
/// \tparam PixelAccum - Specifies tha data type which will be used for creating buffer container 
/// utilized for holding source image pixels after applying appropriate boundary manipulations.
/// \tparam SrcView - Specifies the type of gil view of source image which is to be correlated
/// with the kernel.
/// \tparam Kernel - Specifies the type of 2D kernel which will be correlated with source image.
/// \tparam DstView -  Specifies the type of gil view which will store the result of 
/// correlation between source image and kernel.
/// \tparam Correlator - Specifies the type of correlator which should be used for performing 
/// correlation.
/// \param src_view - Gil view of source image used in correlation.
/// \param kernel - 2D kernel which will be correlated with source image.
/// \param dst_view - Gil view which will store the result of correlation between "src_view"
/// and "kernel".
/// \param option - Specifies the manner in which boundary pixels of "dst_view" should be computed.
/// \param correlator - Correlator which will be used for performing correlation.
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView, typename Correlator>
void correlate_2d_impl(SrcView src_view, Kernel kernel, DstView dst_view, 
    boundary_option option, Correlator correlator)
{
    long unsigned int const upper_extrapolation_size = kernel.upper_size();
    long unsigned int const lower_extrapolation_size = kernel.lower_size();
    long unsigned int const left_extrapolation_size = kernel.left_size();
    long unsigned int const right_extrapolation_size = kernel.right_size();

    bool explicit_fill = 1;
    long unsigned int col = 0, row = 0;
    PixelAccum zero_pixel;
    pixel_zeros_t<PixelAccum>()(zero_pixel);

    if (option == boundary_option::output_ignore || option == boundary_option::output_zero)
    {
        using dst_pixel_ref_t = typename pixel_proxy<typename DstView::value_type>::type;
        std::vector<PixelAccum> buffer(kernel.size() * (src_view.width()));
        for (col = 0; col < src_view.width(); ++col)
        {
            assign_pixels(src_view.col_begin(col), src_view.col_begin(col) + kernel.size(), 
                buffer.begin() + col * kernel.size());
        }

        for (row = upper_extrapolation_size; row < src_view.height() - lower_extrapolation_size; ++row)
        {
            if (row - upper_extrapolation_size)
            {
                for (col = 0; col < src_view.width(); ++col)
                {
                    std::ptrdiff_t left_bound = col * kernel.size(); 
                    std::rotate(buffer.begin() + left_bound, buffer.begin() + left_bound + 1, 
                        buffer.begin() + left_bound + kernel.size());
                    buffer[left_bound + kernel.size() - 1] = 
                        src_view(col, row + lower_extrapolation_size);
                }
            }
            correlator(buffer.begin(), buffer.size(), kernel.begin(), 
                dst_view.row_begin(row) + left_extrapolation_size);

            if (option == boundary_option::output_ignore)
            {
                assign_pixels(src_view.row_begin(row), 
                    src_view.row_begin(row) + left_extrapolation_size, dst_view.row_begin(row));
                assign_pixels(src_view.row_end(row) - right_extrapolation_size, src_view.row_end(row), 
                    dst_view.row_end(row) - right_extrapolation_size);
            }
            else 
            {
                typename DstView::value_type dst_zero;
                pixel_assigns_t<PixelAccum, dst_pixel_ref_t>()(zero_pixel, dst_zero);
                std::fill_n(dst_view.row_begin(row), left_extrapolation_size, dst_zero);
                std::fill_n(dst_view.row_end(row) - right_extrapolation_size, 
                    right_extrapolation_size, dst_zero);
            }
        }

        if (option == boundary_option::output_ignore)
        {
            for (row = 0; row < upper_extrapolation_size; ++row)
                assign_pixels(src_view.row_begin(row), src_view.row_end(row), dst_view.row_begin(row));

            for (row = src_view.height() - lower_extrapolation_size; row < src_view.height(); ++row)
                assign_pixels(src_view.row_begin(row), src_view.row_end(row), dst_view.row_begin(row));
        }
        else
        {
            typename DstView::value_type dst_zero;
            pixel_assigns_t<PixelAccum, dst_pixel_ref_t>()(zero_pixel, dst_zero);
            for (row = 0; row < upper_extrapolation_size; ++row)
                std::fill_n(dst_view.row_begin(row), src_view.width(), dst_zero);
            for (row = src_view.height() - lower_extrapolation_size; row < src_view.height(); ++row)
                std::fill_n(dst_view.row_begin(row), src_view.width(), dst_zero);
        }
    }
    else 
    {
        std::vector<PixelAccum> buffer(kernel.size() * (src_view.width() + kernel.size() - 1));
        if (option == boundary_option::extend_zero)
        {
            std::fill_n(buffer.begin(), kernel.size() * left_extrapolation_size, zero_pixel);
            std::fill_n(buffer.begin() + buffer.size() - kernel.size() * right_extrapolation_size, 
                kernel.size() * right_extrapolation_size, zero_pixel);
        
            for (long unsigned int index = kernel.size() * left_extrapolation_size; 
                index < buffer.size() - kernel.size() * right_extrapolation_size; 
                index += kernel.size())
            {
                for (long unsigned int inner_index = 0; inner_index < upper_extrapolation_size; 
                    ++inner_index)
                {
                    buffer[index + inner_index] = zero_pixel;
                }
                // check abstraction of this loop.
            }
        }
        else if (option == boundary_option::extend_constant)
        {
            std::vector<PixelAccum> intermediate_buffer(kernel.size());

            assign_pixels(src_view.col_begin(0), 
                src_view.col_begin(0) + kernel.size() - upper_extrapolation_size, 
                intermediate_buffer.begin() + upper_extrapolation_size);

            std::fill_n(intermediate_buffer.begin(), upper_extrapolation_size, 
                intermediate_buffer[upper_extrapolation_size]);

            for (long unsigned int inner_index = 0; inner_index < kernel.size() * left_extrapolation_size; 
                inner_index += kernel.size())
            {
                std::copy(intermediate_buffer.begin(), intermediate_buffer.end(), 
                    buffer.begin() + inner_index);
            }

            assign_pixels(src_view.col_begin(src_view.width() - 1), 
                src_view.col_begin(src_view.width() - 1) + kernel.size() - upper_extrapolation_size, 
                intermediate_buffer.begin() + upper_extrapolation_size);

            std::fill_n(intermediate_buffer.begin(), upper_extrapolation_size, 
                intermediate_buffer[upper_extrapolation_size]);

            for (long unsigned int inner_index = buffer.size() - kernel.size() * right_extrapolation_size; 
                inner_index < buffer.size(); inner_index += kernel.size())
            {
                std::copy(intermediate_buffer.begin(), intermediate_buffer.end(), 
                    buffer.begin() + inner_index);
            }

            for (long unsigned int index = kernel.size() * left_extrapolation_size; 
                index < buffer.size() - kernel.size() * right_extrapolation_size; 
                index += kernel.size())
            {
                for (long unsigned int inner_index = 0; inner_index < upper_extrapolation_size; 
                    ++inner_index)
                {
                    // check indices throughout the algorithm.
                    buffer[index + inner_index] = 
                        src_view((index - kernel.size() * left_extrapolation_size) / kernel.size(), 0);
                }
            }
        }
        else if (option == boundary_option::extend_reflection)
        {
            explicit_fill = 0;
            long unsigned int row_bound = 
                kernel.size() - upper_extrapolation_size > upper_extrapolation_size ? 
                kernel.size() - upper_extrapolation_size : upper_extrapolation_size;

            for (col = 0; col < left_extrapolation_size; ++col)
            {
                for (row = 0; row < row_bound; ++row)
                {
                    if (row < kernel.size() - upper_extrapolation_size)
                    {
                        buffer[col * kernel.size() + upper_extrapolation_size + row] = 
                            src_view(left_extrapolation_size - col - 1, row);
                    }
                    if (row < upper_extrapolation_size)
                    {
                        buffer[col * kernel.size() + upper_extrapolation_size - row - 1] = 
                            src_view(left_extrapolation_size - col - 1, row);
                    }
                }
            }

            for (col = 0; col < src_view.width(); ++col)
            {
                for (row = 0; row < row_bound; ++row)
                {
                    if (row < kernel.size() - upper_extrapolation_size)
                    {
                        buffer[(col + left_extrapolation_size) * kernel.size() 
                            + upper_extrapolation_size + row] = src_view(col, row);
                    }
                    if (row < upper_extrapolation_size)
                    {
                        buffer[(col + left_extrapolation_size) * kernel.size() 
                            + upper_extrapolation_size - row - 1] = src_view(col, row);
                    }
                }
            }

            for (col = src_view.width() - right_extrapolation_size; col < src_view.width(); ++col)
            {
                for (row = 0; row < row_bound; ++row)
                {
                    if (row < kernel.size() - upper_extrapolation_size)
                    {
                        buffer[(col + right_extrapolation_size + left_extrapolation_size) 
                            * kernel.size() + upper_extrapolation_size + row] = 
                            src_view(src_view.width() - col + src_view.width() - right_extrapolation_size 
                            - 1, row);
                    }
                    if (row < upper_extrapolation_size)
                    {
                        buffer[(col + right_extrapolation_size + left_extrapolation_size) 
                            * kernel.size() + upper_extrapolation_size - row - 1] = 
                            src_view(src_view.width() - col + src_view.width() - right_extrapolation_size 
                            - 1, row);
                    }
                }
            }
        }
        else if (option == boundary_option::extend_padded)
        {
            // check iterator characteristics of GIL locators.
            typename SrcView::xy_locator loc_center = src_view.xy_at(0, 0);
            for (col = 0; col < left_extrapolation_size; ++col)
            {
                for (row = 0; row < kernel.size(); ++row)
                {
                    buffer[col * kernel.size() + row] = 
                        loc_center(col - left_extrapolation_size, row - upper_extrapolation_size);
                }
            }

            for (col = 0; col < src_view.width(); ++col)
            {
                loc_center = src_view.xy_at(col, 0);
                for (row = 0; row < upper_extrapolation_size; ++row)
                {
                    buffer[(left_extrapolation_size + col) * kernel.size() + row] = 
                        loc_center(0, row - upper_extrapolation_size);
                }
            }

            loc_center = src_view.xy_at(src_view.width() - 1, 0);
            for (col = 1; col <= right_extrapolation_size; ++col)
            {
                for (row = 0; row < kernel.size(); ++row)
                {
                    buffer[(col - 1 + left_extrapolation_size + src_view.width()) * kernel.size() + 
                        row] = loc_center(col, row - upper_extrapolation_size);
                }
            }
        }

        if (explicit_fill)
        {
            for (col = 0; col < src_view.width(); ++col)
            {
                // Try std::for_each and stuff like that for outside loops too.
                assign_pixels(src_view.col_begin(col), 
                    src_view.col_begin(col) + kernel.size() - upper_extrapolation_size, 
                    buffer.begin() + (left_extrapolation_size + col) * kernel.size() + 
                    upper_extrapolation_size);
            }
        }

        for (row = 0; row < src_view.height() - lower_extrapolation_size; ++row)
        {
            if (row)
            {
                for (long unsigned int temp_col = 0; temp_col < left_extrapolation_size; ++temp_col)
                {
                    std::ptrdiff_t left_bound = temp_col * kernel.size();
                    std::rotate(buffer.begin() + left_bound, buffer.begin() + left_bound + 1, 
                        buffer.begin() + left_bound + kernel.size());
                
                    if (option == boundary_option::extend_zero)
                    {
                        buffer[left_bound + kernel.size() - 1] = zero_pixel;
                    }
                    else if (option == boundary_option::extend_constant)
                    {
                        buffer[left_bound + kernel.size() - 1] = 
                            src_view(0, row + lower_extrapolation_size);
                    }
                    else if (option == boundary_option::extend_reflection)
                    {
                        buffer[left_bound + kernel.size() - 1] = 
                            src_view(left_extrapolation_size - temp_col - 1,
                            row + lower_extrapolation_size);
                        // Try reverse and copy for here
                    }
                    else if (option == boundary_option::extend_padded)
                    {
                        typename SrcView::xy_locator loc_center = 
                            src_view.xy_at(0, row + lower_extrapolation_size);
                        buffer[left_bound + kernel.size() - 1] = 
                            loc_center(temp_col - left_extrapolation_size, 0);
                    }
                }

                for (long unsigned int temp_col = 0; temp_col < right_extrapolation_size; ++temp_col)
                {
                    std::ptrdiff_t left_bound = (left_extrapolation_size + src_view.width() + 
                        temp_col) * kernel.size();
                    std::rotate(buffer.begin() + left_bound, buffer.begin() + left_bound + 1, 
                        buffer.begin() + left_bound + kernel.size());

                    if (option == boundary_option::extend_zero)
                    {
                        buffer[left_bound + kernel.size() - 1] = zero_pixel;
                    }
                    else if (option == boundary_option::extend_constant)
                    {
                        buffer[left_bound + kernel.size() - 1] = 
                            src_view(src_view.width() - 1, row + lower_extrapolation_size);
                    }
                    else if (option == boundary_option::extend_reflection)
                    {
                        buffer[left_bound + kernel.size() - 1] = 
                            src_view(src_view.width() - temp_col - 1, row + lower_extrapolation_size);
                        // Try reverse and copy for here
                    }
                    else if (option == boundary_option::extend_padded)
                    {
                        typename SrcView::xy_locator loc_center = 
                            src_view.xy_at(src_view.width() - 1, row + lower_extrapolation_size);
                        buffer[left_bound + kernel.size() - 1] = loc_center(temp_col + 1, 0);
                    }
                }

                for (long int temp_col = 0; temp_col < src_view.width(); ++temp_col)
                {
                    std::ptrdiff_t left_bound = (left_extrapolation_size + temp_col) * kernel.size(); 
                    std::rotate(buffer.begin() + left_bound, buffer.begin() + left_bound + 1, 
                        buffer.begin() + left_bound + kernel.size());
                    buffer[left_bound + kernel.size() - 1] = 
                        src_view(temp_col, row + lower_extrapolation_size);
                }
            }

            correlator(buffer.begin(), buffer.size(), kernel.begin(), dst_view.row_begin(row));
        }

        for (row = src_view.height() - lower_extrapolation_size; row < src_view.height(); ++row)
        {
            for (long unsigned int temp_col = 0; temp_col < left_extrapolation_size; ++temp_col)
            {
                std::ptrdiff_t left_bound = temp_col * kernel.size();
                std::rotate(buffer.begin() + left_bound, buffer.begin() + left_bound + 1, 
                    buffer.begin() + left_bound + kernel.size());

                if (option == boundary_option::extend_zero)
                {
                    buffer[left_bound + kernel.size() - 1] = zero_pixel;
                }
                else if (option == boundary_option::extend_constant)
                {
                    buffer[left_bound + kernel.size() - 1] = src_view(0, src_view.height() - 1);
                }
                else if (option == boundary_option::extend_reflection)
                {
                    buffer[left_bound + kernel.size() - 1] = src_view(
                        left_extrapolation_size - temp_col - 1, 
                        src_view.height() - row + src_view.height() - lower_extrapolation_size - 1);
                }
                else if (option == boundary_option::extend_padded)
                {
                    typename SrcView::xy_locator loc_center = src_view.xy_at(0, row);
                    buffer[left_bound + kernel.size() - 1] = 
                        loc_center(temp_col - left_extrapolation_size, lower_extrapolation_size);
                }
            }

            for (long unsigned int temp_col = 0; temp_col < right_extrapolation_size; ++temp_col)
            {
                std::ptrdiff_t left_bound = (left_extrapolation_size + src_view.width() + temp_col) * 
                    kernel.size();
               std::rotate(buffer.begin() + left_bound, buffer.begin() + left_bound + 1, 
                    buffer.begin() + left_bound + kernel.size());

               if (option == boundary_option::extend_zero)
                {
                    buffer[left_bound + kernel.size() - 1] = zero_pixel;
                }
                else if (option == boundary_option::extend_constant)
                {
                    buffer[left_bound + kernel.size() - 1] = 
                        src_view(src_view.width() - 1, src_view.height() - 1);
                }
                else if (option == boundary_option::extend_reflection)
                {
                    buffer[left_bound + kernel.size() - 1] = 
                        src_view(src_view.width() - temp_col - 1, 
                        src_view.height() - row + src_view.height() - lower_extrapolation_size - 1);
                }
                else if (option == boundary_option::extend_padded)
                {
                    typename SrcView::xy_locator loc_center = 
                        src_view.xy_at(src_view.width() - 1, row);
                    buffer[left_bound + kernel.size() - 1] = 
                        loc_center(temp_col + 1, lower_extrapolation_size);
                }
            }

            for (long int temp_col = 0; temp_col < src_view.width(); ++temp_col)
            {
                std::ptrdiff_t left_bound = (left_extrapolation_size + temp_col) * kernel.size(); 
                std::rotate(buffer.begin() + left_bound, buffer.begin() + left_bound + 1, 
                    buffer.begin() + left_bound + kernel.size());

                if (option == boundary_option::extend_zero)
                {
                    buffer[left_bound + kernel.size() - 1] = zero_pixel;
                }
                else if (option == boundary_option::extend_constant)
                {
                    buffer[left_bound + kernel.size() - 1] = src_view(temp_col, src_view.height() - 1);
                }
                else if (option == boundary_option::extend_reflection)
                {
                    buffer[left_bound + kernel.size() - 1] = 
                        src_view(temp_col, 
                        src_view.height() - row + src_view.height() - lower_extrapolation_size - 1);
                }
                else if (option == boundary_option::extend_padded)
                {
                    typename SrcView::xy_locator loc_center = src_view.xy_at(temp_col, row);
                    buffer[left_bound + kernel.size() - 1] = loc_center(0, lower_extrapolation_size);
                }
            }

            correlator(buffer.begin(), buffer.size(), kernel.begin(), dst_view.row_begin(row));
        }
    }
}

/// \ingroup ImageAlgorithms
/// \brief Detects whether a 2D kernel is spatial separable or not and separates it if the kernel
/// is spatially separable. 
/// \tparam Kernel Specifies the type of 2D kernel which will be considered for separation.
/// \tparam Container_1d Specifies the type of 1D container which will store the separated components
/// of input kernel if it is separable.
/// \param kernel - Kernel which is to be considered for separation.
/// \param sep_ker_vertical - Container which will store separated vertical component of 2D kernel 
/// if kernel is spatially separable.
/// \param sep_ker_horizontal - Container which will store separated horizontal component of 2D 
/// kernel if kernel is spatially separable.
template <typename Kernel, typename Container_1d>
bool separate(Kernel const kernel, Container_1d& sep_ker_vertical, Container_1d& sep_ker_horizontal)
{
    bool is_rank_1 = 1;
    sep_ker_vertical[0] = 1;
    for (std::size_t row = 1; row < kernel.size(); ++row)
    {
        float mul_factor = 0;
        if (kernel.at(0, 0))
            mul_factor = kernel.at(0, row) / kernel.at(0, 0);
        sep_ker_vertical[row] = mul_factor;
        for (std::size_t col = 0; col < kernel.size(); ++col)
        {
            auto transformed_elem = mul_factor * kernel.at(col, 0);
            if (transformed_elem != kernel.at(col, row))
            {
                is_rank_1 = 0;
                break;
            }
        }
        if (is_rank_1 == 0)
            break;
    }
    if (is_rank_1)
    {
        for (std::size_t col = 0; col < kernel.size(); ++col)
            sep_ker_horizontal[col] = kernel.at(col, 0);
    }
    return is_rank_1;
}

} // namespace detail

/// \ingroup ImageAlgorithms
/// \brief Correlate 2D variable-size kernel along an image.
/// \tparam PixelAccum Specifies tha data type which will be used while creating buffer container 
/// which is utilized for holding source image pixels after applying appropriate boundary
/// manipulations.
/// \tparam SrcView Models ImageViewConcept.
/// \tparam Kernel Specifies the type of 2D kernel which will be correlated with source image.
/// \tparam DstView Models MutableImageViewConcept.
/// \param src_view - Gil view of source image.
/// \param kernel - Variable size 2D kernel which will be correlated with src_view.
/// \param dst_view - Gil view of destination image.
/// \param option - Specifies the manner in which boundary pixels of dst_view will be computed.
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView>
void correlate_2d(SrcView src_view, Kernel kernel, DstView dst_view, 
    boundary_option option = boundary_option::extend_zero)
{
    BOOST_ASSERT(src_view.dimensions() == dst_view.dimensions());
    BOOST_ASSERT(kernel.size() != 0);

    gil_function_requires<ImageViewConcept<SrcView>>();
    gil_function_requires<MutableImageViewConcept<DstView>>();
    static_assert(color_spaces_are_compatible
    <
        typename color_space_type<SrcView>::type,
        typename color_space_type<DstView>::type
    >::value, "Source and destination views must have pixels with the same color space");

    // Improve this interface for less wastage of memory.
    std::vector<typename Kernel::value_type> sep_ker_vertical(kernel.size());
    std::vector<typename Kernel::value_type> sep_ker_horizontal(kernel.size());
    if (detail::separate(kernel, sep_ker_vertical, sep_ker_horizontal))
    {
        kernel_1d<typename Kernel::value_type> ver_kernel(sep_ker_vertical.begin(), kernel.size(), 
            kernel.center_y());
        kernel_1d<typename Kernel::value_type> hor_kernel(sep_ker_horizontal.begin(), kernel.size(), 
            kernel.center_x());

        gil::image<PixelAccum> dummy_img(dst_view.dimensions());
        auto dummy_view = gil::view(dummy_img);

        correlate_rows<PixelAccum>(src_view, hor_kernel, dummy_view, option);
        correlate_cols<PixelAccum>(dummy_view, ver_kernel, dst_view, option);
        return;
    }

    detail::correlate_2d_impl<PixelAccum>(src_view, kernel, dst_view, option, detail::correlator_n_2d<PixelAccum>(
        kernel.size()));
}

/// \ingroup ImageAlgorithms
/// \brief Correlate 2D fixed-size kernel along an image.
/// \tparam PixelAccum Specifies tha data type which will be used for creating buffer container 
/// utilized for holding source image pixels after applying appropriate boundary manipulations.
/// \tparam SrcView Models ImageViewConcept.
/// \tparam Kernel Specifies the type of 2D kernel which will be correlated with source image.
/// \tparam DstView Models MutableImageViewConcept.
/// \param src_view - Gil view of source image.
/// \param kernel - Fixed size 2D kernel which will be correlated with src_view.
/// \param dst_view - Gil view of destination image.
/// \param option - Specifies the manner in which boundary pixels of dst_view will be computed.
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView>
void correlate_2d_fixed(SrcView src_view, Kernel kernel, DstView dst_view, 
    boundary_option option = boundary_option::extend_zero)
{
    BOOST_ASSERT(src_view.dimensions() == dst_view.dimensions());
    BOOST_ASSERT(kernel.size() != 0);

    gil_function_requires<ImageViewConcept<SrcView>>();
    gil_function_requires<MutableImageViewConcept<DstView>>();
    static_assert(color_spaces_are_compatible
    <
        typename color_space_type<SrcView>::type,
        typename color_space_type<DstView>::type
    >::value, "Source and destination views must have pixels with the same color space");

    std::vector<typename Kernel::value_type> sep_ker_vertical(kernel.size());
    std::vector<typename Kernel::value_type> sep_ker_horizontal(kernel.size());
    if (detail::separate(kernel, sep_ker_vertical, sep_ker_horizontal))
    {
        kernel_1d_fixed<typename Kernel::value_type, Kernel::static_size> ver_kernel(
            sep_ker_vertical.begin(), kernel.center_y());
        kernel_1d_fixed<typename Kernel::value_type, Kernel::static_size> hor_kernel(
            sep_ker_horizontal.begin(), kernel.center_x());

        gil::image<PixelAccum> dummy_img(dst_view.dimensions());
        auto dummy_view = gil::view(dummy_img);
        correlate_rows_fixed<PixelAccum>(src_view, hor_kernel, dummy_view, option);
        correlate_cols_fixed<PixelAccum>(dummy_view, ver_kernel, dst_view, option);
        return;
    }
    
    using correlator = detail::correlator_k_2d<Kernel::static_size, PixelAccum>;
    detail::correlate_2d_impl<PixelAccum>(src_view, kernel, dst_view, option, correlator{});
}

/// \ingroup ImageAlgorithms
/// \brief Convolves 2D variable-size kernel along an image.
/// \tparam PixelAccum Specifies tha data type which will be used for creating buffer container 
/// utilized for holding source image pixels after applying appropriate boundary manipulations.
/// \tparam SrcView Models ImageViewConcept.
/// \tparam Kernel Specifies the type of 2D kernel which will be convoluted with source image.
/// \tparam DstView Models MutableImageViewConcept.
/// \param src_view - Gil view of source image.
/// \param kernel - Variable size 2D kernel which will be convolved with src_view.
/// \param dst_view - Gil view of destination image.
/// \param option - Specifies the manner in which boundary pixels of dst_view will be computed.
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView>
void convolve_2d(SrcView src_view, Kernel kernel, DstView dst_view, 
    boundary_option option = boundary_option::extend_zero)
{
    BOOST_ASSERT(src_view.dimensions() == dst_view.dimensions());
    BOOST_ASSERT(kernel.size() != 0);

    gil_function_requires<ImageViewConcept<SrcView>>();
    gil_function_requires<MutableImageViewConcept<DstView>>();
    static_assert(color_spaces_are_compatible
    <
        typename color_space_type<SrcView>::type,
        typename color_space_type<DstView>::type
    >::value, "Source and destination views must have pixels with the same color space");

    correlate_2d<PixelAccum>(src_view, detail::reverse_kernel_2d(kernel), dst_view, option);
    // check reverse kernel.
}

/// \ingroup ImageAlgorithms
/// \brief Convolve 2D fixed-size kernel along an image
/// \tparam PixelAccum Specifies tha data type which will be used for creating buffer container 
/// utilized for holding source image pixels after applying appropriate boundary manipulations.
/// \tparam SrcView Models ImageViewConcept.
/// \tparam Kernel Specifies the type of 2D kernel which will be convolved with source image.
/// \tparam DstView Models MutableImageViewConcept.
/// \param src_view - Gil view of source image.
/// \param kernel - Fixed size 2D kernel which will be convolved with src_view.
/// \param dst_view - Gil view of destination image.
/// \param option - Specifies the manner in which boundary pixels of dst_view will be computed.
template <typename PixelAccum, typename SrcView, typename Kernel, typename DstView>
void convolve_2d_fixed(SrcView src_view, Kernel kernel, DstView dst_view, 
    boundary_option option = boundary_option::extend_zero)
{
    BOOST_ASSERT(src_view.dimensions() == dst_view.dimensions());
    BOOST_ASSERT(kernel.size() != 0);

    gil_function_requires<ImageViewConcept<SrcView>>();
    gil_function_requires<MutableImageViewConcept<DstView>>();
    static_assert(color_spaces_are_compatible
    <
        typename color_space_type<SrcView>::type,
        typename color_space_type<DstView>::type
    >::value, "Source and destination views must have pixels with the same color space");

    correlate_2d_fixed<PixelAccum>(src_view, detail::reverse_kernel_2d(kernel), dst_view, option);
    // check reverse kernel.
}

}} // namespace boost::gil

#endif
