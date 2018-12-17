//
// Copyright 2012 Christian Henning, Andreas Pokorny, Lubomir Bourdev
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_UTILITY_GET_PIXEL_TYPE_HPP
#define BOOST_GIL_UTILITY_GET_PIXEL_TYPE_HPP

#include <boost/gil/utility/is_bit_aligned.hpp>

namespace boost{ namespace gil {

/// get_pixel_type metafunction
/// \brief Depending on Image this function generates either
///        the pixel type or the reference type in case
///        the image is bit_aligned.
template< typename View >
struct get_pixel_type
    : mpl::if_
        <
            typename is_bit_aligned<typename View::value_type>::type,
            typename View::reference,
            typename View::value_type
        >
{
};

} // namespace gil
} // namespace boost

#endif
