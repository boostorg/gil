
#include <boost/test/unit_test.hpp>

#include <boost/gil/gil_all.hpp>

#include <boost/gil/extension/io/detail/typedefs.hpp>
#include <boost/gil/extension/io/tiff_all.hpp>

#include "color_space_write_test.hpp"
#include "mandel_view.hpp"
#include "paths.hpp"

using namespace std;
using namespace boost::gil;

typedef tiff_tag tag_t;

BOOST_AUTO_TEST_SUITE( tiff_test )

BOOST_AUTO_TEST_CASE( rgb_color_space_write_test )
{
    color_space_write_test< tag_t >( tiff_out + "rgb_color_space_test.tif"
                                   , tiff_out + "bgr_color_space_test.tif"
                                   );
}

BOOST_AUTO_TEST_SUITE_END()
