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

#include <boost/gil/extension/io/detail/io_error.hpp>

namespace boost { namespace gil {

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
    
    /// todo
    //typedef ptrdiff_t difference_type;

public:

    /// Default Constructor, usually used to create an end iterator.
    scanline_read_iterator()
    : _pos( -1 )
    , _read_scanline( true )
    , _skip_scanline( true )
    , _reader( NULL )
    , _buffer( NULL )
    {}

    /// Constructor with preallocated image. Reading starts at first scanline of source image.
    scanline_read_iterator( Reader& reader
                          , byte_t* buffer
                          )
    : _pos( 0 )
    , _read_scanline( true )
    , _skip_scanline( true )
    , _reader( &reader )
    , _buffer( buffer  )
    {
        init();
    }

    /// Constructor with preallocated image. Reading starts at first scanline of source image.
    scanline_read_iterator( Reader& reader )
    : _pos( 0 )
    , _read_scanline( true )
    , _skip_scanline( true )
    , _reader( &reader )
    , _buffer( NULL )
    {
        init();
    }

    /// Constructor with preallocated image. Reading starts at pos scanline of source image.
    scanline_read_iterator( std::size_t pos
                          , Reader&     reader
                          , byte_t*     buffer
                          )
    : _pos( pos )
    , _read_scanline( true )
    , _skip_scanline( true )
    , _reader( &reader )
    , _buffer( buffer  )
    {
        init();

        if( this->_pos >= this->_reader->_info._height )
        {
            throw std::runtime_error( "Trying to read past image." );
        }

        if( pos > 0 && _buffer == NULL )
        {
            throw std::runtime_error( "Cannot proceed without initializing read buffer." );
        }

        for( std::size_t i = 0; i < pos; ++i )
        {
            _skip();
        }
    }

    //
    // Destructor
    //
    ~scanline_read_iterator()
    {
        if( _reader )
        {
            _reader->clean_up();
        }
    }

    /// Set reader. Do clean up before if necessary.
    void set_reader( Reader& reader )
    {
        if( _reader )
        {
            _reader->clean_up();

            _pos = 0;
        }

        _reader = &reader;

        init();
    }

    /// Set reader. Do clean up before if necessary.
    void set_buffer( byte_t* buffer )
    {
        _buffer = buffer;
    }

    /// Dereference Operator
    reference operator*()
    {
        if( _reader == NULL ) { io_error( "Reader cannot be null for this operation." ); }
        if( _buffer == NULL ) { io_error( "Buffer cannot be null for this operation." ); }

        if( _read_scanline == true )
        {
            _reader->read( _buffer, _pos );

            increase_pos();
        }

        _skip_scanline = false;
        _read_scanline = false;

        return _buffer;
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

            increase_pos();
        }

        _skip_scanline = true;
        _read_scanline = true;

        return (*this);
    }

    /// Compare passed iterator to this.
    bool equal( const scanline_read_iterator< Reader >& rhs ) const
    {
        return (_reader == rhs._reader) && ( _buffer == rhs._buffer );
    }

    bool operator ==( const scanline_read_iterator< Reader >& rhs ) const
    {
        return _pos == rhs._pos;
    }

    bool operator !=( const scanline_read_iterator< Reader >& rhs ) const
    {
        return _pos != rhs._pos;
    }

    /// Return backend.
    const backend_t& backend()
    {
        if( _reader )
        {
            return *_reader;
        }

        io_error( "Reader cannot be null for this operation." );
    }

private:

    void init()
    {
        // this needs to be done by the scanline_reader. Otherwise the user wont know the scanline length.
        //if( _reader )
        //{
        //    _reader->initialize();
        //}
    }

    void _skip()
    {
        if( _reader )
        {
            _reader->skip( _buffer, _pos );
        }
    }

    void increase_pos()
    {
        if( this->_reader == NULL ) { io_error("Reader cannot be null for this operation."); }

        if( _pos < static_cast< int >( this->_reader->_info._height ) - 1 )
        {
            ++_pos;
        }
        else
        {
            _pos = -1;
        }
    }

private:

    mutable int _pos;

    mutable bool _read_scanline;
    mutable bool _skip_scanline;

    Reader* _reader;
    byte_t* _buffer;
};

} // namespace gil
} // namespace boost

#endif // BOOST_GIL_EXTENSION_IO_IMAGE_READ_ITERATOR_HPP
