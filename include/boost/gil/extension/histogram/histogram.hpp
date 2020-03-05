//
// Copyright 2020 Nedjima Belgacem <gb_nedjima@esi.dz>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_EXTENSION_HISTOGRAM_HISTOGRAM_HPP
#define BOOST_GIL_EXTENSION_HISTOGRAM_HISTOGRAM_HPP

#include <boost/gil.hpp>

namespace boost {
namespace gil {

template<typename ColorSpace, typename PixelType = gray8_pixel_t>
class histogram
{
    int _histogram[1 << (8 * sizeof(PixelType))];
public:
    histogram() { std::fill(begin(), end(), 0); }
    int& operator[](const ColorSpace& color) {
        PixelType dst;
        color_convert(color, dst);
        return _histogram[dst];
    }
    int operator[](const ColorSpace& color) const {
        PixelType dst;
        color_convert(color, dst);
        return _histogram[dst];
    }
    constexpr size_t size() { return 1 << (8 * sizeof(PixelType)); };
    int* begin() { return _histogram; }
    int* end() { return _histogram + size(); }
};

}
}

#endif