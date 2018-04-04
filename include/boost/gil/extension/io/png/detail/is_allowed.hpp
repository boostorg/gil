/*
    Copyright 2008 Christian Henning, Lubomir Bourdev
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef boost_gil_extension_io_png_detail_is_allowed_hpp_
#define boost_gil_extension_io_png_detail_is_allowed_hpp_

////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief
/// \author Christian Henning \n
///
/// \date 2008 \n
///
////////////////////////////////////////////////////////////////////////////////////////

#include <boost/mpl/for_each.hpp>

namespace boost { namespace gil { namespace detail {

template< typename View >
bool is_allowed( const image_read_info< png_tag >& info
               , mpl::true_   // is read_and_no_convert
               )
{
    typedef typename get_pixel_type< View >::type pixel_t;

    typedef typename channel_traits< 
                typename element_type< pixel_t >::type >::value_type channel_t;

    const png_num_channels::type dst_num_channels = num_channels< pixel_t >::value;
    const png_bitdepth::type     dst_bit_depth    = detail::unsigned_integral_num_bits< channel_t >::value;

    return   dst_num_channels == info._num_channels
          && dst_bit_depth    == info._bit_depth;
}

template< typename View >
bool is_allowed( const image_read_info< png_tag >& /* info */
               , mpl::false_  // is read_and_convert
               )
{
    return true;
}

} // namespace detail
} // namespace gil
} // namespace boost

#endif
