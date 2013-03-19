// Copyright 2013 Christian Henning
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \brief Unit test for subsampled_image type.

#include "stdafx.h"

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
        ycbcr8_pixel_t a;
        rgb8_pixel_t b;

        color_convert( a, b );
        color_convert( b, a );
    }

    {
        typedef rgb8_pixel_t pixel_t;
        typedef subsampled_image< pixel_t > image_t;

        image_t img( 640, 480
                   , 640, 480 
                   , 640, 480 
                   );

        fill_pixels( view( img )
                   , pixel_t( 10, 20, 30 )
                   );
    }
}

BOOST_AUTO_TEST_SUITE_END()


