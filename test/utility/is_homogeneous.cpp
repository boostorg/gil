//
// Copyright 2013 Christian Henning
// Copyright 2018 Mateusz Loskot <mateusz at loskot dot net>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include <boost/gil.hpp> // TODO: IWYU

namespace gil = boost::gil;

int main()
{
    static_assert(
        gil::is_homogeneous<gil::rgb8_pixel_t>::value,
        "homogeneous rgb8_pixel_t required");

    static_assert(
        gil::is_homogeneous<gil::cmyk16c_planar_ref_t>::value,
        "homogeneous cmyk16c_planar_ref_t required");

    using image_t = gil::bit_aligned_image1_type<4, gil::gray_layout_t>::type;
    static_assert(
        gil::is_homogeneous<image_t::view_t::reference >::value,
        "homogeneous single-channel bit-aligned image required");

    return 0;
}
