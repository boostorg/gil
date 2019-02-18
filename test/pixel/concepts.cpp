//
// Copyright 2019 Mateusz Loskot <mateusz at loskot dot net>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include <boost/gil.hpp>

#include <boost/concept_check.hpp>
#include <boost/mpl/vector_c.hpp>

namespace mpl = boost::mpl;
namespace gil = boost::gil;
using boost::function_requires;

int main()
{
    function_requires<gil::PixelConcept<gil::gray8_pixel_t>>();
    function_requires<gil::PixelConcept<gil::rgb8_pixel_t>>();
    function_requires<gil::PixelConcept<gil::bgr8_pixel_t>>();

    using rgb565_pixel_t = gil::packed_pixel_type
        <
            std::uint16_t,
            mpl::vector3_c<unsigned, 5, 6, 5>,
            gil::rgb_layout_t
        >::type;
    function_requires<gil::PixelConcept<rgb565_pixel_t>>();
    function_requires<gil::PixelValueConcept<rgb565_pixel_t>>();

    using bgr556_pixel_t = gil::packed_pixel_type
        <
            std::uint16_t,
            mpl::vector3_c<unsigned, 5, 6, 5>,
            gil::bgr_layout_t>::type;
    function_requires<gil::PixelConcept<bgr556_pixel_t>>();
    function_requires<gil::PixelValueConcept<bgr556_pixel_t>>();

    function_requires<gil::PixelsCompatibleConcept<rgb565_pixel_t, bgr556_pixel_t>>();

    return 0;
}
