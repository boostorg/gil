/*
    Copyright 2012 Christian Henning, Andreas Pokorny, Lubomir Bourdev
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_TOOLBOX_METAFUNCTIONS_IS_HOMOGENEOUS_HPP
#define BOOST_GIL_EXTENSION_TOOLBOX_METAFUNCTIONS_IS_HOMOGENEOUS_HPP

////////////////////////////////////////////////////////////////////////////////////////
/// \file is_homogeneous.hpp
/// \brief is_homogeneous metafunction
/// \author Christian Henning, Andreas Pokorny, Lubomir Bourdev \n
///
/// \date 2012 \n
///
////////////////////////////////////////////////////////////////////////////////////////

#include <boost/mpl/at.hpp>

#include <boost/gil/pixel.hpp>

#include <type_traits>

namespace boost{ namespace gil {

/// is_homogeneous metafunctions
/// \brief Determines if a pixel types are homogeneous.

template<typename C,typename CMP, int Next, int Last> struct is_homogeneous_impl;

template<typename C,typename CMP, int Last>
struct is_homogeneous_impl<C,CMP,Last,Last> : std::true_type {};

template<typename C,typename CMP, int Next, int Last>
struct is_homogeneous_impl : mpl::and_< is_homogeneous_impl< C, CMP,Next + 1, Last >
                                      , std::is_same< CMP, typename mpl::at_c<C,Next>::type
                                      > > {};

template < typename P > struct is_homogeneous : std::false_type {};

// pixel
template < typename C, typename L > struct is_homogeneous< pixel<C,L> > : std::true_type {};
template < typename C, typename L > struct is_homogeneous<const pixel<C,L> > : std::true_type {};
template < typename C, typename L > struct is_homogeneous< pixel<C,L>& > : std::true_type {};
template < typename C, typename L > struct is_homogeneous<const pixel<C,L>& > : std::true_type {};

// planar pixel reference
template <typename Channel, typename ColorSpace>
struct is_homogeneous< planar_pixel_reference< Channel, ColorSpace > > : std::true_type {};
template <typename Channel, typename ColorSpace>
struct is_homogeneous< const planar_pixel_reference< Channel, ColorSpace > > : std::true_type {};

template<typename C,typename CMP, int I,int Last>
struct is_homogeneous_impl_p {};

// for packed_pixel
template <typename B, typename C, typename L >
struct is_homogeneous<packed_pixel< B, C, L > > 
    : is_homogeneous_impl_p< C 
                           , typename mpl::at_c< C, 0 >::type
                           , 1
                           , mpl::size< C >::type::value
                           > {};

template< typename B
        , typename C
        , typename L
        >  
struct is_homogeneous< const packed_pixel< B, C, L > > 
    : is_homogeneous_impl_p< C
                           , typename mpl::at_c<C,0>::type
                           , 1
                           , mpl::size< C >::type::value
                           > {};

// for bit_aligned_pixel_reference
template <typename B, typename C, typename L, bool M>  
struct is_homogeneous<bit_aligned_pixel_reference<B,C,L,M> > 
    : is_homogeneous_impl<C,typename mpl::at_c<C,0>::type,1,mpl::size<C>::type::value>
{};

template <typename B, typename C, typename L, bool M>  
struct is_homogeneous<const bit_aligned_pixel_reference<B,C,L,M> > 
    : is_homogeneous_impl<C,typename mpl::at_c<C,0>::type,1,mpl::size<C>::type::value>
{};

} // namespace gil
} // namespace boost

#endif // BOOST_GIL_EXTENSION_TOOLBOX_METAFUNCTIONS_IS_HOMOGENEOUS_HPP
