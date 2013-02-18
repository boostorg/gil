/********************************************************
 *
 * This test file will test palette tiled tiff writing
 *
 *******************************************************/

//#define BOOST_TEST_MODULE tiff_tiled_palette_write_test_8_16_module
#include <boost/test/unit_test.hpp>

#include "tiff_tiled_write_macros.hpp"

BOOST_AUTO_TEST_SUITE( tiff_test )

#ifdef BOOST_GIL_IO_USE_TIFF_GRAPHICSMAGICK_TEST_SUITE_IMAGES

BOOST_PP_REPEAT_FROM_TO(9, 17, GENERATE_WRITE_TILE_BIT_ALIGNED_PALETTE, palette )

#endif // BOOST_GIL_IO_USE_TIFF_GRAPHICSMAGICK_TEST_SUITE_IMAGES

BOOST_AUTO_TEST_SUITE_END()
