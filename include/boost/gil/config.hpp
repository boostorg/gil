/*
    Copyright 2005-2007 Adobe Systems Incorporated
   
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).

    See http://opensource.adobe.com/gil for most recent version including documentation.
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_CONFIG_HPP
#define BOOST_GIL_CONFIG_HPP

////////////////////////////////////////////////////////////////////////////////////////
/// \file               
/// \brief GIL configuration file
/// \author Lubomir Bourdev and Hailin Jin \n
///         Adobe Systems Incorporated
///
////////////////////////////////////////////////////////////////////////////////////////

#include <boost/config.hpp>

// Enable GIL_NONWORD_POINTER_ALIGNMENT_SUPPORTED if your platform supports dereferencing on non-word memory boundary.
// Enabling the flag results in performance improvement
#if !defined(__hpux) && !defined(sun) && !defined(__sun) && !defined(__osf__)
    #define GIL_NONWORD_POINTER_ALIGNMENT_SUPPORTED
#endif

#endif
