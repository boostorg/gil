/*
    Copyright 2013 Christian Henning
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).
*/

/*************************************************************************************************/

#ifndef BOOST_GIL_EXTENSION_TOOLBOX_IMAGE_TYPES_SUBSAMPLED_IMAGE_HPP
#define BOOST_GIL_EXTENSION_TOOLBOX_IMAGE_TYPES_SUBSAMPLED_IMAGE_HPP

////////////////////////////////////////////////////////////////////////////////////////
/// \file subsampled_image.hpp
/// \brief Subsampled Image extension
/// \author Christian Henning \n
///
/// \date 2013 \n
///
////////////////////////////////////////////////////////////////////////////////////////

#include <boost/gil/image.hpp>

namespace boost{ namespace gil {

typedef boost::gil::point2< std::ptrdiff_t > point_t;

////////////////////////////////////////////////////////////////////////////////////////
/// \class subsampled_image_deref_fn
/// \ingroup PixelLocatorModel PixelBasedModel
/// \brief Used for virtual_2D_locator
///
////////////////////////////////////////////////////////////////////////////////////////
template< typename Locator >
struct subsampled_image_deref_fn
{
    typedef Locator plane_locator_t;

    typedef subsampled_image_deref_fn    const_t;
    typedef typename Locator::value_type value_type;
    typedef value_type                   reference;
    typedef value_type                   const_reference;
    typedef point_t                      argument_type;
    typedef reference                    result_type;

    static const bool is_mutable = false;

    /// default constructor
    subsampled_image_deref_fn() {}

    /// constructor
    subsampled_image_deref_fn( const Locator& y_locator
                             , const Locator& v_locator
                             , const Locator& u_locator
                             )
    : _y_locator( y_locator )
    , _v_locator( v_locator )
    , _u_locator( u_locator )
    {}

    /// operator()
    typename result_type operator()( const point_t& p ) const
    {
        boost::uint8 y = _y_locator.xy_at( p );

        return value_type();
    }

    /// 
    const Locator& y_locator() const { return _y_locator; }
    const Locator& v_locator() const { return _v_locator; }
    const Locator& u_locator() const { return _u_locator; }

private:
    
    Locator _y_locator;
    Locator _v_locator;
    Locator _u_locator;
};


////////////////////////////////////////////////////////////////////////////////////////
/// \class subsampled_image_locator_type
/// \ingroup PixelLocatorModel PixelBasedModel
/// \brief 
///
////////////////////////////////////////////////////////////////////////////////////////
template< typename Locator >
struct subsampled_image_locator
{
    typedef virtual_2d_locator< subsampled_image_deref_fn< Locator > // Deref
                              , false                                // IsTransposed
                              > type;
};


////////////////////////////////////////////////////////////////////////////////////////
/// \class subsampled_image_view
/// \ingroup ImageViewModel PixelBasedModel
/// \brief A lightweight object that interprets a subsampled image.Models ImageViewConcept,PixelBasedConcept,HasDynamicXStepTypeConcept,HasDynamicYStepTypeConcept,HasTransposedTypeConcept
///
////////////////////////////////////////////////////////////////////////////////////////
template< typename Locator > // subsampled_image_locator< ... >::type
class subsampled_image_view : public image_view< Locator >
{
public:

    typedef typename Locator::deref_fn_t         deref_fn_t;
    typedef typename deref_fn_t::plane_locator_t plane_locator_t;

    typedef subsampled_image_view const_t;

    typedef image_view< plane_locator_t > plane_view_t;

    /// default constructor
    subsampled_image_view()
    : image_view< Locator >()
    {}

    /// constructor
    subsampled_image_view( const point_t& y_dimensions
                         , const point_t& v_dimensions
                         , const point_t& u_dimensions
                         , const Locator& locator
                         )
    : image_view< Locator >( y_dimensions, locator )
    , _y_dimensions( y_dimensions )
    , _v_dimensions( v_dimensions )
    , _u_dimensions( u_dimensions )
    {}
    
    /// copy constructor
    template< typename Subsampled_View >
    subsampled_image_view( const Subsampled_View& v )
    : image_view< locator_t >( v )
    {}

    const point_t& y_dimension() const { return _y_dimensions; }
    const point_t& v_dimension() const { return _v_dimensions; }
    const point_t& u_dimension() const { return _u_dimensions; }

    const plane_locator_t& y_plane() const { return get_deref_fn().y_locator(); }
    const plane_locator_t& v_plane() const { return get_deref_fn().v_locator(); }
    const plane_locator_t& u_plane() const { return get_deref_fn().u_locator(); }

    const plane_view_t y_plane_view() const { return plane_view_t( _y_dimensions, y_plane() ); }
    const plane_view_t v_plane_view() const { return plane_view_t( _v_dimensions, v_plane() ); }
    const plane_view_t u_plane_view() const { return plane_view_t( _u_dimensions, u_plane() ); }


private:

    const deref_fn_t& get_deref_fn() const { return this->pixels().deref_fn(); }

private:

    template< typename Locator > friend class subsampled_image_view;

    point_t _y_dimensions;
    point_t _v_dimensions;
    point_t _u_dimensions;
};


////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup ImageModel PixelBasedModel
/// \brief container interface over image view. Models ImageConcept, PixelBasedConcept
///
/// A subsampled image holds a bunch of planes which don't need to have the same resolution.
///
////////////////////////////////////////////////////////////////////////////////////////
template< typename Pixel
        , typename Allocator = std::allocator< unsigned char >
        >
class subsampled_image
{
public:
    
    typedef typename channel_type< Pixel >::type channel_t;
    typedef pixel< channel_t, gray_layout_t> pixel_t;

    typedef image< pixel_t, false, Allocator > plane_image_t;

    typedef typename plane_image_t::view_t plane_view_t;
    typedef typename plane_image_t::const_view_t plane_const_view_t;
    typedef typename plane_view_t::locator plane_locator_t;

    typedef typename subsampled_image_locator< plane_locator_t >::type locator_t;

    typedef typename plane_image_t::coord_t x_coord_t;
    typedef typename plane_image_t::coord_t y_coord_t;

    typedef subsampled_image_view< locator_t > view_t;
    typedef typename view_t::const_t           const_view_t;


    /// constructor
    subsampled_image( const x_coord_t   y_width
                    , const y_coord_t   y_height
                    , const x_coord_t   v_width
                    , const y_coord_t   v_height
                    , const x_coord_t   u_width
                    , const y_coord_t   u_height
                    , const std::size_t y_alignment = 0
                    , const std::size_t v_alignment = 0
                    , const std::size_t u_alignment = 0
                   )
    : _y_plane( y_width, y_height, y_alignment, Allocator() )
    , _v_plane( v_width, v_height, v_alignment, Allocator() )
    , _u_plane( u_width, u_height, u_alignment, Allocator() )
    {
        init( point_t( y_width, y_height )
            , point_t( v_width, v_height )
            , point_t( u_width, u_height )
            );
    }

public:

    view_t _view;

private:

    void init( const point_t& y_dimensions
             , const point_t& v_dimensions
             , const point_t& u_dimensions
             )
    {
        typedef subsampled_image_deref_fn< plane_locator_t > defer_fn_t;

        defer_fn_t deref_fn( view( _y_plane ).xy_at( 0, 0 )
                           , view( _v_plane ).xy_at( 0, 0 )
                           , view( _u_plane ).xy_at( 0, 0 )
                           );

        // init a virtual_2d_locator
        locator_t locator( point_t( 0, 0 ) // p
                         , point_t( 1, 1 ) // step
                         , deref_fn
                         );

        _view = view_t( y_dimensions
                      , v_dimensions
                      , u_dimensions
                      , locator
                      );
    }
    

private:

    plane_image_t _y_plane;
    plane_image_t _v_plane;
    plane_image_t _u_plane;
};

/////////////////////////////////////////////////////////////////////////////////////////
/// \name view, const_view
/// \brief Get an image view from an subsampled_image
/// \ingroup ImageModel
/// \brief Returns the non-constant-pixel view of an image
/////////////////////////////////////////////////////////////////////////////////////////
template< typename Pixel >
inline
const typename subsampled_image< Pixel >::view_t& view( subsampled_image< Pixel >& img )
{
    return img._view;
}

template< typename Pixel >
inline
const typename subsampled_image< Pixel >::const_view_t const_view( subsampled_image< Pixel >& img )
{
    return static_cast< const typename subsampled_image< Pixel >::const_view_t>( img._view );
}

/////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup ImageViewSTLAlgorithmsFillPixels
/// \brief std::fill for subsampled_image views
/////////////////////////////////////////////////////////////////////////////////////////
template< typename Locator
        , typename Pixel
        >
void fill_pixels( const subsampled_image_view< Locator >& view
                , const Pixel&                            value
                )
{
    typedef typename subsampled_image<Pixel>::plane_view_t::value_type channel_t;

    fill_pixels( view.y_plane_view(), channel_t( at_c<0>( value )));
    fill_pixels( view.v_plane_view(), channel_t( at_c<1>( value )));
    fill_pixels( view.u_plane_view(), channel_t( at_c<2>( value )));
}


} // namespace gil
} // namespace boost

#endif // BOOST_GIL_EXTENSION_TOOLBOX_IMAGE_TYPES_SUBSAMPLED_IMAGE_HPP