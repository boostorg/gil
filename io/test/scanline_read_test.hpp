#ifndef BOOST_GIL_IO_UNIT_TEST_SCANLINE_READ_TEST_HPP
#define BOOST_GIL_IO_UNIT_TEST_SCANLINE_READ_TEST_HPP

#include <boost/gil/gil_all.hpp>

#include "cmp_view.hpp"

template< typename Image
        , typename FormatTag
        >
void test_scanline_reader( const char* file_name )
{
    using namespace boost::gil;

    // read image using scanline_read_iterator
    typedef scanline_reader< typename get_read_device< const char*
                                                     , FormatTag
                                                     >::type
                           , FormatTag
                           > reader_t;

    reader_t reader = make_scanline_reader( file_name, FormatTag() );

    Image dst( reader._info._width, reader._info._height );
    byte_t* buffer = new byte_t[ reader._scanline_length ];

    scanline_read_iterator< reader_t > it  = scanline_read_iterator< reader_t >( reader, buffer );
    scanline_read_iterator< reader_t > end = scanline_read_iterator< reader_t >();

    int row = 0;
    while( it != end )
    {
        *it;

        copy_pixels( interleaved_view( reader._info._width, 1, ( typename Image::view_t::x_iterator ) buffer, reader._scanline_length )
                   , subimage_view( view( dst    ), 0, row, reader._info._width, 1 )
                   );

        ++row;
    }

    delete[] buffer;

    //compare
    Image img;
    read_image( file_name, img, FormatTag() );

    cmp_view( view( dst ), view( img ) );
}

#endif // BOOST_GIL_IO_UNIT_TEST_SCANLINE_READ_TEST_HPP
