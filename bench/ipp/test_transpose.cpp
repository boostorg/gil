// Copyright (C) 2020 Samuel Debionne, ESRF.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <ipp.h>

#include <boost/gil.hpp>
#include <boost/core/lightweight_test.hpp>

static void test_transpose()
{
    using namespace boost::gil;

    size_t dim = 1024;

    gray8_image_t in(dim, dim);
    gray8_image_t out(dim, dim);

    generate_pixels(view(in), [i = 0]() mutable -> std::uint8_t { return ++i; });

    IppiSize srcRoi = { dim, dim };
    ippiTranspose_8u_C1R(
        boost::gil::interleaved_view_get_raw_data(const_view(in)), (int) in.width(),
        boost::gil::interleaved_view_get_raw_data(view(out)), (int) out.width(),
        srcRoi);

    BOOST_TEST(equal_pixels(transposed_view(const_view(in)), const_view(out)));
}

static void test_transpose_inplace()
{
    using namespace boost::gil;

    size_t dim = 1024;

    gray8_image_t in(dim, dim);
    
    generate_pixels(view(in), [i = 0]() mutable -> std::uint8_t { return ++i; });

    gray8_image_t in_ref(in);

    IppiSize srcRoi = { dim, dim };
    ippiTranspose_8u_C1IR(
        boost::gil::interleaved_view_get_raw_data(view(in)), (int) in.width(),
        srcRoi);

    BOOST_TEST(equal_pixels(transposed_view(const_view(in_ref)), const_view(in)));
}

int main()
{
    test_transpose();
    test_transpose_inplace();

    return ::boost::report_errors();
}