//
// Copyright 2005-2007 Adobe Systems Incorporated
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_EXTENSION_NUMERIC_ALGORITHM_HPP
#define BOOST_GIL_EXTENSION_NUMERIC_ALGORITHM_HPP

#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>

#include <boost/gil/metafunctions.hpp>
#include <boost/gil/pixel_iterator.hpp>

#include <boost/assert.hpp>

#include <algorithm>
#include <iterator>
#include <numeric>
#include <type_traits>

namespace boost { namespace gil {

/// \brief Reference proxy associated with a type that has a \p "reference" member type alias.
///
/// The reference proxy is the reference type, but with stripped-out C++ reference.
/// Models PixelConcept.
template <typename T>
struct pixel_proxy : std::remove_reference<typename T::reference> {};

/// \brief std::for_each for a pair of iterators
template <typename Iterator1, typename Iterator2, typename BinaryFunction>
BinaryFunction for_each(Iterator1 first1, Iterator1 last1, Iterator2 first2, BinaryFunction f)
{
    while (first1 != last1)
        f(*first1++, *first2++);
    return f;
}

template <typename SrcIterator, typename DstIterator>
inline
auto assign_pixels(SrcIterator src, SrcIterator src_end, DstIterator dst) -> DstIterator
{
    for_each(src, src_end, dst,
        pixel_assigns_t
        <
            typename pixel_proxy<typename std::iterator_traits<SrcIterator>::value_type>::type,
            typename pixel_proxy<typename std::iterator_traits<DstIterator>::value_type>::type
        >());
    return dst + (src_end - src);
}

namespace detail {

template <std::size_t Size>
struct inner_product_k_t
{
    template
    <
        class InputIterator1,
        class InputIterator2,
        class T,
        class BinaryOperation1,
        class BinaryOperation2
    >
    static T apply(
        InputIterator1 first1,
        InputIterator2 first2, T init,
        BinaryOperation1 binary_op1,
        BinaryOperation2 binary_op2)
    {
        init = binary_op1(init, binary_op2(*first1, *first2));
        return inner_product_k_t<Size - 1>::template apply(
            first1 + 1, first2 + 1, init, binary_op1, binary_op2);
    }
};

template <>
struct inner_product_k_t<0>
{
    template
    <
        class InputIterator1,
        class InputIterator2,
        class T,
        class BinaryOperation1,
        class BinaryOperation2
    >
    static T apply(
        InputIterator1 first1,
        InputIterator2 first2,
        T init,
        BinaryOperation1 binary_op1,
        BinaryOperation2 binary_op2)
    {
        return init;
    }
};

} // namespace detail

/// static version of std::inner_product
template
<
    std::size_t Size,
    class InputIterator1,
    class InputIterator2,
    class T,
    class BinaryOperation1,
    class BinaryOperation2
>
BOOST_FORCEINLINE
T inner_product_k(
    InputIterator1 first1,
    InputIterator2 first2,
    T init,
    BinaryOperation1 binary_op1,
    BinaryOperation2 binary_op2)
{
    return detail::inner_product_k_t<Size>::template apply(
        first1, first2, init, binary_op1, binary_op2);
}

/// \brief 1D un-guarded cross-correlation with a variable-size kernel
template
<
    typename PixelAccum,
    typename SrcIterator,
    typename KernelIterator,
    typename Size,
    typename DstIterator
>
inline
auto correlate_pixels_n(
    SrcIterator src_begin,
    SrcIterator src_end,
    KernelIterator kernel_begin,
    Size kernel_size,
    DstIterator dst_begin)
    -> DstIterator
{
    using src_pixel_ref_t = typename pixel_proxy
        <
            typename std::iterator_traits<SrcIterator>::value_type
        >::type;
    using dst_pixel_ref_t = typename pixel_proxy
        <
            typename std::iterator_traits<DstIterator>::value_type
        >::type;
    using kernel_value_t = typename std::iterator_traits<KernelIterator>::value_type;

    PixelAccum accum_zero;
    pixel_zeros_t<PixelAccum>()(accum_zero);
    while (src_begin != src_end)
    {
        pixel_assigns_t<PixelAccum, dst_pixel_ref_t>()(
            std::inner_product(
                src_begin,
                src_begin + kernel_size,
                kernel_begin,
                accum_zero,
                pixel_plus_t<PixelAccum, PixelAccum, PixelAccum>(),
                pixel_multiplies_scalar_t<src_pixel_ref_t, kernel_value_t, PixelAccum>()),
            *dst_begin);

        ++src_begin;
        ++dst_begin;
    }
    return dst_begin;
}

/// \brief 1D un-guarded cross-correlation with a fixed-size kernel
template
<
    std::size_t Size,
    typename PixelAccum,
    typename SrcIterator,
    typename KernelIterator,
    typename DstIterator
>
inline
auto correlate_pixels_k(
    SrcIterator src_begin,
    SrcIterator src_end,
    KernelIterator kernel_begin,
    DstIterator dst_begin)
    -> DstIterator
{
    using src_pixel_ref_t = typename pixel_proxy
        <
            typename std::iterator_traits<SrcIterator>::value_type
        >::type;
    using dst_pixel_ref_t = typename pixel_proxy
        <
            typename std::iterator_traits<DstIterator>::value_type
        >::type;
    using kernel_type = typename std::iterator_traits<KernelIterator>::value_type;

    PixelAccum accum_zero;
    pixel_zeros_t<PixelAccum>()(accum_zero);
    while (src_begin != src_end)
    {
        pixel_assigns_t<PixelAccum, dst_pixel_ref_t>()(
            inner_product_k<Size>(
                src_begin,
                kernel_begin,
                accum_zero,
                pixel_plus_t<PixelAccum, PixelAccum, PixelAccum>(),
                pixel_multiplies_scalar_t<src_pixel_ref_t, kernel_type, PixelAccum>()),
            *dst_begin);

        ++src_begin;
        ++dst_begin;
    }
    return dst_begin;
}

/// \brief destination is set to be product of the source and a scalar
/// \tparam PixelAccum - TODO
/// \tparam SrcView Models ImageViewConcept
/// \tparam DstView Models MutableImageViewConcept
template <typename PixelAccum, typename SrcView, typename Scalar, typename DstView>
inline
void view_multiplies_scalar(SrcView const& src_view, Scalar const& scalar, DstView const& dst_view)
{
    static_assert(std::is_scalar<Scalar>::value, "Scalar is not scalar");
    BOOST_ASSERT(src_view.dimensions() == dst_view.dimensions());
    using src_pixel_ref_t = typename pixel_proxy<typename SrcView::value_type>::type;
    using dst_pixel_ref_t = typename pixel_proxy<typename DstView::value_type>::type;
    using y_coord_t = typename SrcView::y_coord_t;

    y_coord_t const height = src_view.height();
    for (y_coord_t y = 0; y < height; ++y)
    {
        typename SrcView::x_iterator it_src = src_view.row_begin(y);
        typename DstView::x_iterator it_dst = dst_view.row_begin(y);
        typename SrcView::x_iterator it_src_end = src_view.row_end(y);
        while (it_src != it_src_end)
        {
            pixel_assigns_t<PixelAccum, dst_pixel_ref_t>()(
                pixel_multiplies_scalar_t<src_pixel_ref_t, Scalar, PixelAccum>()(*it_src, scalar),
                *it_dst);

            ++it_src;
            ++it_dst;
        }
    }
}

}} // namespace boost::gil

#endif
