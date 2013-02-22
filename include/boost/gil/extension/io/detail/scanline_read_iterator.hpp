/*
    Copyright 2012 Christian Henning
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_IO_IMAGE_READ_ITERATOR_HPP
#define BOOST_GIL_EXTENSION_IO_IMAGE_READ_ITERATOR_HPP

////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief
/// \author Christian Henning \n
///
/// \date 2012 \n
///
////////////////////////////////////////////////////////////////////////////////////////

#include <vector>

#include <boost/gil/extension/io/detail/io_error.hpp>

namespace boost { namespace gil {

#if BOOST_WORKAROUND(BOOST_MSVC, >= 1400) 
#pragma warning(push) 
#pragma warning(disable:4512) //assignment operator could not be generated 
#endif

/// Input iterator to read images.
template< typename Reader >
class scanline_read_iterator
{
public:

    typedef typename Reader::backend_t backend_t;

    typedef std::input_iterator_tag iterator_category;
    typedef byte_t* value_type;
    typedef value_type const* pointer;
    typedef value_type const& reference;
    typedef int difference_type;
    
public:

    /// Constructor with preallocated image. Reading starts at first scanline of source image.
    scanline_read_iterator( Reader& reader
                          , int pos = 0
                          )
    : _pos( pos )
    , _read_scanline( true )
    , _skip_scanline( true )
    , _reader( reader )
    {
        init();
    }

    /// Dereference Operator
    reference operator*()
    {
        if( _read_scanline == true )
        {
            _reader.read( _buffer_start
                        , _pos
                        );
        }

        _skip_scanline = false;
        _read_scanline = false;

        return _buffer_start;
    }

    /// Pointer-To-Memper Operator.
    pointer operator->() const
    {
        return &(operator*());
    }

    /// Pre-Increment Operator
    scanline_read_iterator< Reader >& operator++()
    {
        if( _skip_scanline == true )
        {
            _skip();
        }

        ++_pos;

        _skip_scanline = true;
        _read_scanline = true;

        return (*this);
    }

    bool operator ==( const scanline_read_iterator< Reader >& rhs ) const
    {
        return _pos == rhs._pos;
    }

    bool operator !=( const scanline_read_iterator< Reader >& rhs ) const
    {
        return _pos < rhs._pos;
    }

private:

    void init()
    {
        _buffer = std::vector< byte_t >( _reader._scanline_length );
        _buffer_start = &_buffer.front();
    }

    void _skip()
    {
        _reader.skip( _buffer_start
                    , _pos
                    );
    }

private:

    mutable int _pos;

    mutable bool _read_scanline;
    mutable bool _skip_scanline;

    Reader& _reader;

    std::vector< byte_t > _buffer;
    byte_t*               _buffer_start;
};

#if BOOST_WORKAROUND(BOOST_MSVC, >= 1400) 
#pragma warning(pop) 
#endif 

} // namespace gil
} // namespace boost

#endif // BOOST_GIL_EXTENSION_IO_IMAGE_READ_ITERATOR_HPP
