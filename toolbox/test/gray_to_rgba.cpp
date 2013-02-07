#include <boost/test/unit_test.hpp>

#include <boost/type_traits/is_same.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/toolbox/color_converters/gray_to_rgba.hpp>

using namespace boost;
using namespace gil;

BOOST_AUTO_TEST_SUITE( gray_to_rgba_test )

BOOST_AUTO_TEST_CASE( gray_to_rgba_test )
{
    gray8_pixel_t a( 45 );
    rgba8_pixel_t b;

    color_convert( a, b );

}

BOOST_AUTO_TEST_SUITE_END()
