#include <boost/test/unit_test.hpp>

#include <boost/type_traits/is_same.hpp>

#include <boost/gil/extension/toolbox/color_spaces/cmyka.hpp>

using namespace boost;
using namespace gil;

BOOST_AUTO_TEST_SUITE( cmyka_test )

BOOST_AUTO_TEST_CASE( cmyka_test )
{
    cmyka8_pixel_t a( 10, 20, 30, 40, 50 );
    rgba8_pixel_t  b;
    cmyka8_pixel_t c;

    color_convert( a, b );

    // no rgba to cmyka conversion implemented
    //color_convert( b, c );
    //BOOST_ASSERT( at_c<0>(a) == at_c<0>(c) );
}

BOOST_AUTO_TEST_SUITE_END()
