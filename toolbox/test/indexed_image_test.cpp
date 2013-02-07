// Copyright 2012 Christian Henning
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \brief Unit test for indexed_image type.

#include <boost/test/unit_test.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/toolbox/image_types/indexed_image.hpp>


using namespace std;
using namespace boost;
using namespace gil;


BOOST_AUTO_TEST_SUITE( index_image_test_suite )

BOOST_AUTO_TEST_CASE( index_image_test )
{
    {
        indexed_image< uint8_t, rgb8_pixel_t > img( 640, 480 );
        fill_pixels( view( img ), rgb8_pixel_t( 255, 0, 0 ));

        rgb8_pixel_t p = *view( img ).xy_at( 10, 10 );
    }

    {

        indexed_image< gray8_pixel_t, rgb8_pixel_t > img( 640, 480, 256 );

        //generate_pixels( img.get_indices_view()
        //               , [] () -> uint8_t
        //                {
        //                    static uint8_t i = 0;
        //                    i = ( i == 256 ) ? 0 : ++i;

        //                    return gray8_pixel_t( i );
        //                }
        //               );


        //generate_pixels( img.get_palette_view()
        //               , [] () ->rgb8_pixel_t
        //                {
        //                    static uint8_t i = 0;
        //                    i = ( i == 256 ) ? 0 : ++i;

        //                    return rgb8_pixel_t( i, i, i );
        //                }
        //               );

        int i = ( 640 * 10 + 10 ) % 256;

        gray8_pixel_t index = *img.get_indices_view().xy_at( 10   , 1 );
        rgb8_pixel_t  color = *img.get_palette_view().xy_at( index, 0 );

        rgb8_pixel_t p = *view( img ).xy_at( 10, 1 );

        i = 9;
    }

    {
        indexed_image< uint8_t, rgb8_pixel_t > img( 640, 480, 256 );

        //generate_pixels( img.get_indices_view()
        //               , [] () -> uint8_t
        //               {
        //                    static uint8_t i = 0;
        //                    i = ( i == 256 ) ? 0 : ++i;

        //                    return i;
        //               }
        //               );


        //generate_pixels( img.get_palette_view()
        //               , [] () ->rgb8_pixel_t
        //               {
        //                  static uint8_t i = 0;
        //                  i = ( i == 256 ) ? 0 : ++i;

        //                  return rgb8_pixel_t( i, i, i );
        //               }
        //               );

        int i = ( 640 * 10 + 10 ) % 256;

        uint8_t      index = *img.get_indices_view().xy_at( 10   , 1 );
        rgb8_pixel_t color = *img.get_palette_view().xy_at( index, 0 );

        rgb8_pixel_t p = *view( img ).xy_at( 10, 1 );

        i = 9;
    }

    {
        typedef indexed_image< uint8_t, rgb8_pixel_t > image_t;
        image_t img( 640, 480, 256 );

        for( image_t::y_coord_t y = 0; y < view( img ).height(); ++y )
        {
            image_t::view_t::x_iterator it = view( img ).row_begin( y );

            for( image_t::x_coord_t x = 0; x < view( img ).width(); ++x )
            {
                rgb8_pixel_t p = *it;
                it++;
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
