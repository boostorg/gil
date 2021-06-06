//
// Copyright 2019 Mateusz Loskot <mateusz at loskot dot net>
// Copyright 2021 Prathamesh Tagore <prathameshtagore@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include <boost/gil.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/assert.hpp>

#include <initializer_list>
#include <type_traits>

namespace boost { namespace gil {

namespace test { namespace fixture {

template <typename T>
auto create_kernel(std::initializer_list<T> const& values)
    -> gil::kernel_1d<T>
{
    static_assert(std::is_arithmetic<T>::value,
        "kernel value type should be integral or floating-point type");
    BOOST_ASSERT_MSG((values.size() - 1) % 2 == 0, "expected odd number of kernel values");
    gil::kernel_1d<T> kernel(values.begin(), values.size(), (values.size() - 1) / 2);
    return kernel;
}

// Adds an offset similar to 1D row convolution with kernel {0, 0, 0, 0, 0, 0, 1, 0, 0} 
// (for offset = 2) having its anchor point at 5th element and boundary option as "extend_zero".
template <typename SrcView, typename DstView>
void row_conv1D_offset_img_generator(SrcView src_view, DstView dst_view, int const offset,
    std::ptrdiff_t start_row = 0, std::ptrdiff_t start_col = 0, std::ptrdiff_t end_row = -1,
    std::ptrdiff_t end_col = -1)
{
    BOOST_ASSERT(src_view.dimensions() == dst_view.dimensions());
    static_assert(color_spaces_are_compatible
    <
        typename color_space_type<SrcView>::type,
        typename color_space_type<DstView>::type
    >::value, "Source and destination views must have pixels with the same color space");

    if (end_row == -1)
        end_row = src_view.height();
    if (end_col == -1)
        end_col = src_view.width();
    for (std::ptrdiff_t y = start_row; y < end_row; ++y)
    {
        auto src_it = src_view.row_begin(y);
        auto dst_it = dst_view.row_begin(y);
        for (std::ptrdiff_t x = offset + start_col; x < end_col; ++x)
            dst_it[x] = src_it[x - offset];
    }
}

// Adds an offset similar to 1D column convolution with kernel {0, 0, 0, 0, 0, 0, 1, 0, 0} 
// (for offset = 2) having its anchor point at 5th element and boundary option as "extend_zero".
template <typename SrcView, typename DstView>
void col_conv1D_offset_img_generator(SrcView src_view, DstView dst_view, int const offset,
    std::ptrdiff_t start_row = 0, std::ptrdiff_t start_col = 0, std::ptrdiff_t end_row = -1,
    std::ptrdiff_t end_col = -1)
{
    BOOST_ASSERT(src_view.dimensions() == dst_view.dimensions());
    static_assert(color_spaces_are_compatible
    <
        typename color_space_type<SrcView>::type,
        typename color_space_type<DstView>::type
    >::value, "Source and destination views must have pixels with the same color space");

    if (end_row == -1)
        end_row = src_view.height();
    if (end_col == -1)
        end_col = src_view.width();
    for (std::ptrdiff_t x = start_col; x < end_col; ++x)
    {
        auto src_it = src_view.col_begin(x);
        auto dst_it = dst_view.col_begin(x);
        for (std::ptrdiff_t y = offset + start_row; y < end_row; ++y)
            dst_it[y] = src_it[y - offset];
    }
}

}}}} // namespace boost::gil::test::fixture
