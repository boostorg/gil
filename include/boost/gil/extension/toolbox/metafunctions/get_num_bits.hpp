/*
    Copyright 2012 Christian Henning, Andreas Pokorny, Lubomir Bourdev
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_TOOLBOX_METAFUNCTIONS_GET_NUM_BITS_HPP
#define BOOST_GIL_EXTENSION_TOOLBOX_METAFUNCTIONS_GET_NUM_BITS_HPP

////////////////////////////////////////////////////////////////////////////////////////
/// \file get_num_bits.hpp
/// \brief get_num_bits metafunction.
/// \author Christian Henning, Andreas Pokorny, Lubomir Bourdev \n
///
/// \date 2012 \n
///
////////////////////////////////////////////////////////////////////////////////////////

#include <boost/gil/channel.hpp>

namespace boost{ namespace gil {

/// get_num_bits metafunctions
/// \brief Determines the numbers of bits for the given channel type.

template <typename T>
struct get_num_bits;

template< typename B, int I, int S, bool M >
struct get_num_bits< packed_channel_reference< B, I, S, M > >
{
    BOOST_STATIC_CONSTANT( int, value = S );
};

template<typename B,int I, int S, bool M>
struct get_num_bits< const packed_channel_reference< B, I, S, M > >
{
    BOOST_STATIC_CONSTANT( int, value = S );
};

template<typename B, int I, bool M>
struct get_num_bits< packed_dynamic_channel_reference< B, I, M > >
{
    BOOST_STATIC_CONSTANT( int, value = I );
};

template<typename b, int i, bool m>
struct get_num_bits< const packed_dynamic_channel_reference< b, i, m > >
{
    BOOST_STATIC_CONSTANT( int, value = i );
};

template< int N >
struct get_num_bits< packed_channel_value< N > >
{
    BOOST_STATIC_CONSTANT( int, value = N );
};

template< int N >
struct get_num_bits< const packed_channel_value< N > >
{
    BOOST_STATIC_CONSTANT( int, value = N );
};

} // namespace gil
} // namespace boost

#endif // BOOST_GIL_EXTENSION_TOOLBOX_METAFUNCTIONS_GET_NUM_BITS_HPP
