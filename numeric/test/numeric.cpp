// Copyright 2013 Krzysztof Czainski
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file numeric.cpp

/// \brief Unit test for Numeric extension

#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>

#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>

#include <boost/test/unit_test.hpp>

namespace gil = boost::gil;

template < class F, class I >
struct TestMapFn
{
    typedef gil::point2<F> point_t;
    typedef point_t result_type;
    result_type operator()( gil::point2<I> const& src ) const
    {
        F x = static_cast<F>( src.x ) - 0.5;
        F y = static_cast<F>( src.y ) - 0.5;

        return point_t( x, y );
    }
};

namespace boost { namespace gil {

// NOTE: I suggest this could be the default behavior:

template <typename T> struct mapping_traits;

template < class F, class I >
struct mapping_traits< TestMapFn<F,I> > {
    typedef typename TestMapFn<F,I>::result_type result_type;
};

template < class F, class I >
inline
point2<F> transform( TestMapFn<F,I> const& mf, point2<I> const& src )
{
    return mf(src);
}

}} // boost::gil


BOOST_AUTO_TEST_SUITE(Numeric_Tests)

BOOST_AUTO_TEST_CASE(bilinear_sampler_test)
{
    // R G B
    // G W R
    // B R G
    gil::rgb8_image_t img(3,3);
    gil::rgb8_view_t v = view(img);
    v(0,0) = v(1,2) = v(2,1) = gil::rgb8_pixel_t(128,0,0);
    v(0,1) = v(1,0) = v(2,2) = gil::rgb8_pixel_t(0,128,0);
    v(0,2) = v(2,0) = gil::rgb8_pixel_t(0,0,128);
    v(1,1) = gil::rgb8_pixel_t(128,128,128);

    gil::rgb8_image_t dimg(4,4);
    gil::rgb8c_view_t dv = const_view(dimg);

    TestMapFn<double,gil::rgb8_image_t::coord_t> mf;

    gil::resample_pixels( const_view(img), view(dimg), mf, gil::bilinear_sampler() );

    assert( gil::rgb8_pixel_t(128,0,0) == dv(0,0) );
    assert( gil::rgb8_pixel_t(64,64,0) == dv(0,1) );
    assert( gil::rgb8_pixel_t(0,64,64) == dv(0,2) );
    assert( gil::rgb8_pixel_t(0,0,128) == dv(0,3) );

    assert( gil::rgb8_pixel_t(64,64,0) == dv(1,0) );
    assert( gil::rgb8_pixel_t(64,96,32) == dv(1,1) );
    assert( gil::rgb8_pixel_t(64,64,64) == dv(1,2) );
    assert( gil::rgb8_pixel_t(64,0,64) == dv(1,3) );

    assert( gil::rgb8_pixel_t(0,64,64) == dv(2,0) );
    assert( gil::rgb8_pixel_t(64,64,64) == dv(2,1) );
    assert( gil::rgb8_pixel_t(96,64,32) == dv(2,2) );
    assert( gil::rgb8_pixel_t(64,64,0) == dv(2,3) );

    assert( gil::rgb8_pixel_t(0,0,128) == dv(3,0) );
    assert( gil::rgb8_pixel_t(64,0,64) == dv(3,1) );
    assert( gil::rgb8_pixel_t(64,64,0) == dv(3,2) );
    assert( gil::rgb8_pixel_t(0,128,0) == dv(3,3) );
}

BOOST_AUTO_TEST_SUITE_END()
