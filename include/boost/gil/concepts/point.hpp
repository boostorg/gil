//
// Copyright 2005-2007 Adobe Systems Incorporated
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_CONCEPTS_POINT_HPP
#define BOOST_GIL_CONCEPTS_POINT_HPP

#include <boost/gil/concepts/basic.hpp>
#include <boost/gil/concepts/concept_check.hpp>

#include <cstddef>

#if BOOST_GCC >= 40700
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif

namespace boost { namespace gil {

// Forward declarations
template <typename T>
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
struct PointNDConcept
{
    void constraints()
    {
        gil_function_requires<Regular<P>>();

        typedef typename P::value_type value_type;
        static const std::size_t N = P::num_dimensions;
        ignore_unused_variable_warning(N);
        typedef typename P::template axis<0>::coord_t FT;
        typedef typename P::template axis<N-1>::coord_t LT;
        FT ft = gil::axis_value<0>(point);
        axis_value<0>(point) = ft;
        LT lt = axis_value<N-1>(point);
        axis_value<N-1>(point) = lt;

        //value_type v=point[0];
        //ignore_unused_variable_warning(v);
    }
    P point;
};

/// \brief 2-dimensional point concept
/// \code
/// concept Point2DConcept<typename T> : PointNDConcept<T>
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
struct Point2DConcept
{
    void constraints()
    {
        gil_function_requires<PointNDConcept<P>>();
        BOOST_STATIC_ASSERT(P::num_dimensions == 2);
        point.x = point.y;
        point[0] = point[1];
    }
    P point;
};

}} // namespace boost::gil

#if BOOST_GCC >= 40700
#pragma GCC diagnostic pop
#endif

#endif
