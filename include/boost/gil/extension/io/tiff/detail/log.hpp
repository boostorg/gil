/*
    Copyright 2009 Christian Henning
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef boost_gil_extension_io_tiff_detail_log_hpp_
#define boost_gil_extension_io_tiff_detail_log_hpp_

////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief
/// \author Christian Henning \n
///
/// \date   2009 \n
///
////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
#include "tiffio.h"
}

#include <iostream>

namespace boost { namespace gil {

class tiff_no_log
{
public:

    tiff_no_log()
    {
        TIFFSetErrorHandler  ( NULL );
        TIFFSetWarningHandler( NULL );
    }
};

class console_log
{
public:

    console_log()
    {
        TIFFSetErrorHandler  ( console_log::error   );
        TIFFSetWarningHandler( console_log::warning );
    }

private:

    static void error( const char* /* module */
                     , const char* fmt
                     , va_list ap
                     )
    {
        char buf[1000];
        sprintf(buf, fmt, ap);
        std::cout << "error: " << buf << std::endl;
    }

    static void warning( char const* /* module */
                       , char const* fmt
                       , va_list ap
                       )
    {
        char buf[1000];
        sprintf(buf, fmt, ap);
        std::cout << "warning: " << fmt << std::endl;
    }
};

} // namespace gil
} // namespace boost

#endif
