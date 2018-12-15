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
    using image_t = gil::bit_aligned_image5_type
        <
            16, 16, 16, 8, 8,
            gil::devicen_layout_t<5>
        >::type;

    static_assert(
        gil::pixel_bit_size<image_t::view_t::reference>::value == 64,
        "64-bit pixel-size image required");

    return 0;
}
