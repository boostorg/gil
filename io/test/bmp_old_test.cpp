//#define BOOST_TEST_MODULE bmp_old_test_module
#include <boost/test/unit_test.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io_new/bmp_io_old.hpp>

#include "mandel_view.hpp"
#include "paths.hpp"

using namespace std;
using namespace boost;
using namespace gil;

BOOST_AUTO_TEST_SUITE( bmp_test )

#ifdef BOOST_GIL_IO_TEST_ALLOW_READING_IMAGES

BOOST_AUTO_TEST_CASE( old_read_dimensions_test )
{
    point2< ptrdiff_t > dim = bmp_read_dimensions( bmp_filename );

    BOOST_CHECK_EQUAL( dim.x, 1000 );
    BOOST_CHECK_EQUAL( dim.y, 600  );
}

BOOST_AUTO_TEST_CASE( old_read_image_test )
{
    rgb8_image_t img;
    bmp_read_image( bmp_filename, img );

    BOOST_CHECK_EQUAL( img.width() , 1000 );
    BOOST_CHECK_EQUAL( img.height(), 600  );
}

BOOST_AUTO_TEST_CASE( old_read_and_convert_image_test )
{
    rgb8_image_t img;
    bmp_read_and_convert_image( bmp_filename, img );

    BOOST_CHECK_EQUAL( img.width() , 1000 );
    BOOST_CHECK_EQUAL( img.height(), 600  );
}

BOOST_AUTO_TEST_CASE( old_read_view_test )
{
    rgb8_image_t img( 1000, 600 );
    bmp_read_view( bmp_filename, view( img ) );
}

BOOST_AUTO_TEST_CASE( old_read_and_convert_view_test )
{
    rgb8_image_t img( 1000, 600 );
    bmp_read_and_convert_view( bmp_filename, view( img ) );
}

BOOST_AUTO_TEST_CASE( old_write_view_test )
{
#ifdef BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
    bmp_write_view( bmp_out + "old_write_view_test.bmp"
                  , create_mandel_view( 1000, 600
                                      , rgb8_pixel_t( 0,   0, 255 )
                                      , rgb8_pixel_t( 0, 255,   0 )
                                      )
                  );
#endif // BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
}

BOOST_AUTO_TEST_CASE( old_dynamic_image_test )
{
    typedef mpl::vector< gray8_image_t
                       , gray16_image_t
                       , rgb8_image_t
                       , rgba8_image_t
                       > my_img_types;


    any_image< my_img_types > runtime_image;

    bmp_read_image( bmp_filename.c_str()
                  , runtime_image
                  );

#ifdef BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
    bmp_write_view( bmp_out + "old_dynamic_image_test.bmp"
                  , view( runtime_image )
                  );
#endif // BOOST_GIL_IO_TEST_ALLOW_WRITING_IMAGES
}
#endif // BOOST_GIL_IO_TEST_ALLOW_READING_IMAGES

BOOST_AUTO_TEST_SUITE_END()
