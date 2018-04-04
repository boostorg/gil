/*
    Copyright 2009 Christian Henning
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef boost_gil_extension_io_pnm_detail_is_allowed_hpp_
#define boost_gil_extension_io_pnm_detail_is_allowed_hpp_

////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief
/// \author Christian Henning \n
///
/// \date 2008 \n
///
////////////////////////////////////////////////////////////////////////////////////////

namespace boost { namespace gil { namespace detail {

template< typename View >
bool is_allowed( const image_read_info< pnm_tag >& info
               , mpl::true_   // is read_and_no_convert
               )
{
    pnm_image_type::type asc_type = is_read_supported< typename get_pixel_type< View >::type
                                                     , pnm_tag
                                                     >::_asc_type;

    pnm_image_type::type bin_type = is_read_supported< typename get_pixel_type< View >::type
                                                     , pnm_tag
                                                     >::_bin_type;
    if( info._type == pnm_image_type::mono_asc_t::value )
    {
        // ascii mono images are read gray8_image_t
        return (  asc_type == pnm_image_type::gray_asc_t::value );
    }


    return (  asc_type == info._type
           || bin_type == info._type
           );
}

template< typename View >
bool is_allowed( const image_read_info< pnm_tag >& /* info */
               , mpl::false_  // is read_and_convert
               )
{
    return true;
}

} // namespace detail
} // namespace gil
} // namespace boost

#endif
