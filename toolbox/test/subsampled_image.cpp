// Copyright 2013 Christian Henning
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \brief Unit test for subsampled_image type.

#include <boost/test/unit_test.hpp>

#include <boost/gil/gil_all.hpp>

#include <boost/gil/extension/toolbox/color_spaces/ycbcr.hpp>
#include <boost/gil/extension/toolbox/image_types/subsampled_image.hpp>


using namespace std;
using namespace boost;
using namespace gil;

BOOST_AUTO_TEST_SUITE( subsampled_image_test_suite )

BOOST_AUTO_TEST_CASE( subsampled_image_test )
{
    {
        ycbcr_601_8_pixel_t a( 10, 20, 30 );
        rgb8_pixel_t b;
        bgr8_pixel_t c;

        color_convert( a, b );
        color_convert( a, c );
        BOOST_ASSERT( static_equal( b, c ));

        color_convert( b, a );
    }

    {
        typedef rgb8_pixel_t pixel_t;
        typedef subsampled_image< pixel_t > image_t;

        image_t img( 640, 480 );

        fill_pixels( view( img )
                   , pixel_t( 10, 20, 30 )
                   );
    }

    {
        typedef ycbcr_601_8_pixel_t pixel_t;
        typedef subsampled_image< pixel_t > image_t;

        std::size_t y_width     = 640;
        std::size_t y_height    = 480;
        std::size_t vx_ssfactor =   2;
        std::size_t vy_ssfactor =   2;
        std::size_t ux_ssfactor =   2;
        std::size_t uy_ssfactor =   2;

        std::size_t image_size = ( y_width * y_height )
                               + ( y_width / vx_ssfactor ) * ( y_height / vy_ssfactor )
                               + ( y_width / ux_ssfactor ) * ( y_height / uy_ssfactor );

        vector< unsigned char > data( image_size );

        image_t::view_t v = subsampled_view< pixel_t >( y_width
                                                      , y_height
                                                      , &data.front()
                                                      , vx_ssfactor
                                                      , vy_ssfactor
                                                      , ux_ssfactor
                                                      , uy_ssfactor
                                                      );
        rgb8_pixel_t p = *v.xy_at( 0, 0 );
    }
}

BOOST_AUTO_TEST_SUITE_END()


