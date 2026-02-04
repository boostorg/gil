//
// Copyright 2005-2007 Adobe Systems Incorporated
// Copyright 2026 Samuel Debionne
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_CONCEPTS_POINT_HPP
#define BOOST_GIL_CONCEPTS_POINT_HPP

#include <boost/gil/detail/config.hpp>

#if !defined(BOOST_GIL_HAS_CONCEPTS)
#   error C++ 20 standard required.
#endif

#include <concepts>
#include <cstddef>

#if defined(BOOST_CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic ignored "-Wunused-local-typedefs"
#endif

#if defined(BOOST_GCC) && (BOOST_GCC >= 40900)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif

namespace boost { namespace gil {

// Forward declarations
template <BOOST_GIL_CONSTRAINT(std::regular) T>
class point;

template <std::size_t K, typename T>
T const& axis_value(point<T> const& p);

template <std::size_t K, typename T>
T& axis_value(point<T>& p);

/// \brief N-dimensional point concept
/// \code
/// concept PointNDConcept<typename T> : Regular<T>
/// {
///     // the type of a coordinate along each axis
///     template <size_t K>
///     struct axis; where Metafunction<axis>;
///
///     const size_t num_dimensions;
///
///     // accessor/modifier of the value of each axis.
///
///     template <size_t K>
///     typename axis<K>::type const& T::axis_value() const;
///
///     template <size_t K>
///     typename axis<K>::type& T::axis_value();
/// };
/// \endcode
/// \ingroup PointConcept
///
template <typename P>
concept PointNDConcept = requires(P point, typename P::template axis<0>::coord_t ft, typename P::template axis<P::num_dimensions - 1>::coord_t lt)
{
    typename P::value_type;
    requires std::regular<typename P::value_type>;

    P::num_dimensions;
    ft = gil::axis_value<0>(point);
    axis_value<0>(point) = ft;
    lt = axis_value<P::num_dimensions - 1>(point);
    axis_value<P::num_dimensions - 1>(point) = lt;
};

/// \brief 2-dimensional point concept
/// \code
/// concept Point2DConcept<typename T> : PointNDConcept<T>N
/// {
///     where num_dimensions == 2;
///     where SameType<axis<0>::type, axis<1>::type>;
///
///     typename value_type = axis<0>::type;
///
///     value_type const& operator[](T const&, size_t i);
///     value_type& operator[](T&, size_t i);
///
///     value_type x,y;
/// };
/// \endcode
/// \ingroup PointConcept
///
template <typename P>
concept Point2DConcept = PointNDConcept<P> && requires(P point)
{
    requires P::num_dimensions == 2;
    point.x = point.y;
    point[0] = point[1];
};

}} // namespace boost::gil

#if defined(BOOST_CLANG)
#pragma clang diagnostic pop
#endif

#if defined(BOOST_GCC) && (BOOST_GCC >= 40900)
#pragma GCC diagnostic pop
#endif

#endif
