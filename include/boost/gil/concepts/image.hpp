//
// Copyright 2005-2007 Adobe Systems Incorporated
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_CONCEPTS_IMAGE_HPP
#define BOOST_GIL_CONCEPTS_IMAGE_HPP

#include <boost/gil/concepts/basic.hpp>
#include <boost/gil/concepts/concept_check.hpp>
#include <boost/gil/concepts/fwd.hpp>
#include <boost/gil/concepts/image_view.hpp>
#include <boost/gil/concepts/point.hpp>

#include <boost/mpl/size.hpp>

#if BOOST_GCC >= 40700
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif

namespace boost { namespace gil {

/// \ingroup ImageConcept
/// \brief N-dimensional container of values
///
/// \code
/// concept RandomAccessNDImageConcept<typename Img> : Regular<Img>
/// {
///     typename view_t; where MutableRandomAccessNDImageViewConcept<view_t>;
///     typename const_view_t = view_t::const_t;
///     typename point_t      = view_t::point_t;
///     typename value_type   = view_t::value_type;
///     typename allocator_type;
///
///     Img::Img(point_t dims, std::size_t alignment=1);
///     Img::Img(point_t dims, value_type fill_value, std::size_t alignment);
///
///     void Img::recreate(point_t new_dims, std::size_t alignment=1);
///     void Img::recreate(point_t new_dims, value_type fill_value, std::size_t alignment);
///
///     const point_t&        Img::dimensions() const;
///     const const_view_t&   const_view(const Img&);
///     const view_t&         view(Img&);
/// };
/// \endcode
template <typename Img>
struct RandomAccessNDImageConcept
{
    void constraints()
    {
        gil_function_requires<Regular<Img>>();

        typedef typename Img::view_t       view_t;
        gil_function_requires<MutableRandomAccessNDImageViewConcept<view_t> >();

        typedef typename Img::const_view_t const_view_t;
        typedef typename Img::value_type   pixel_t;

        typedef typename Img::point_t        point_t;
        gil_function_requires<PointNDConcept<point_t> >();

        const_view_t cv = const_view(img);
        ignore_unused_variable_warning(cv);
        view_t v  = view(img);
        ignore_unused_variable_warning(v);

        pixel_t fill_value;
        point_t pt=img.dimensions();
        Img im1(pt);
        Img im2(pt,1);
        Img im3(pt,fill_value,1);
        img.recreate(pt);
        img.recreate(pt,1);
        img.recreate(pt,fill_value,1);
    }
    Img img;
};


/// \ingroup ImageConcept
/// \brief 2-dimensional container of values
///
/// \code
/// concept RandomAccess2DImageConcept<RandomAccessNDImageConcept Img>
/// {
///     typename x_coord_t = const_view_t::x_coord_t;
///     typename y_coord_t = const_view_t::y_coord_t;
///
///     Img::Img(x_coord_t width, y_coord_t height, std::size_t alignment=1);
///     Img::Img(x_coord_t width, y_coord_t height, value_type fill_value, std::size_t alignment);
///
///     x_coord_t Img::width() const;
///     y_coord_t Img::height() const;
///
///     void Img::recreate(x_coord_t width, y_coord_t height, std::size_t alignment=1);
///     void Img::recreate(x_coord_t width, y_coord_t height, value_type fill_value, std::size_t alignment);
/// };
/// \endcode
template <typename Img>
struct RandomAccess2DImageConcept
{
    void constraints()
    {
        gil_function_requires<RandomAccessNDImageConcept<Img> >();
        typedef typename Img::x_coord_t  x_coord_t;
        typedef typename Img::y_coord_t  y_coord_t;
        typedef typename Img::value_type value_t;

        gil_function_requires<MutableRandomAccess2DImageViewConcept<typename Img::view_t> >();

        x_coord_t w=img.width();
        y_coord_t h=img.height();
        value_t fill_value;
        Img im1(w,h);
        Img im2(w,h,1);
        Img im3(w,h,fill_value,1);
        img.recreate(w,h);
        img.recreate(w,h,1);
        img.recreate(w,h,fill_value,1);
    }
    Img img;
};

/// \ingroup ImageConcept
/// \brief 2-dimensional image whose value type models PixelValueConcept
///
/// \code
/// concept ImageConcept<RandomAccess2DImageConcept Img>
/// {
///     where MutableImageViewConcept<view_t>;
///     typename coord_t  = view_t::coord_t;
/// };
/// \endcode
template <typename Img>
struct ImageConcept
{
    void constraints()
    {
        gil_function_requires<RandomAccess2DImageConcept<Img> >();
        gil_function_requires<MutableImageViewConcept<typename Img::view_t> >();
        typedef typename Img::coord_t        coord_t;
        BOOST_STATIC_ASSERT(num_channels<Img>::value == mpl::size<typename color_space_type<Img>::type>::value);

        BOOST_STATIC_ASSERT((is_same<coord_t, typename Img::x_coord_t>::value));
        BOOST_STATIC_ASSERT((is_same<coord_t, typename Img::y_coord_t>::value));
    }
    Img img;
};

}} // namespace boost::gil

#if BOOST_GCC >= 40700
#pragma GCC diagnostic pop
#endif

#endif
