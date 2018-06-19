/*
    Copyright 2005-2007 Adobe Systems Incorporated
   
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).

    See http://opensource.adobe.com/gil for most recent version including documentation.
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_GRAY_HPP
#define BOOST_GIL_GRAY_HPP

////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Support for grayscale color space and variants
/// \author Lubomir Bourdev and Hailin Jin \n
///         Adobe Systems Incorporated
/// \date 2005-2007 \n Last updated on March 8, 2006
////////////////////////////////////////////////////////////////////////////////////////

#include <boost/gil/config.hpp>
#include <boost/gil/utility.hpp>

#include <boost/mpl/range_c.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/type_traits.hpp>

namespace boost { namespace gil {

/// \ingroup ColorNameModel
/// \brief Gray
struct gray_color_t {};

/// \ingroup ColorSpaceModel
typedef mpl::vector1<gray_color_t> gray_t;

/// \ingroup LayoutModel
typedef layout<gray_t> gray_layout_t;

} }  // namespace boost::gil

#endif

