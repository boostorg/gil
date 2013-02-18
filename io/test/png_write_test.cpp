//#define BOOST_TEST_MODULE png_read_test_module
#include <boost/test/unit_test.hpp>

#define BOOST_GIL_IO_ADD_FS_PATH_SUPPORT
#define BOOST_GIL_IO_ENABLE_GRAY_ALPHA

#include <boost/cstdint.hpp>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem/convenience.hpp>

#include <boost/gil/extension/io_new/png_all.hpp>

#include <iostream>

#include "color_space_write_test.hpp"
#include "paths.hpp"
#include "scanline_read_test.hpp"

using namespace std;
using namespace boost;
using namespace gil;
using namespace boost::gil::detail;
namespace fs = boost::filesystem;

typedef png_tag tag_t;

BOOST_AUTO_TEST_SUITE( png_test )

BOOST_AUTO_TEST_CASE( rgb_color_space_write_test )
{
    color_space_write_test< tag_t >( png_out + "rgb_color_space_test.png"
                                   , png_out + "bgr_color_space_test.png"
                                   );
}

BOOST_AUTO_TEST_SUITE_END()
