//
// Copyright 2005-2007 Adobe Systems Incorporated
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_POINT_HPP
#define BOOST_GIL_POINT_HPP

#include <boost/gil/utilities.hpp>

#include <boost/config.hpp>

#include <cstddef>

namespace boost { namespace gil {

/// \addtogroup PointModel
///
/// Example:
/// \code
/// point<std::ptrdiff_t> p(3,2);
/// assert((p[0] == p.x) && (p[1] == p.y));
/// assert(axis_value<0>(p) == 3);
/// assert(axis_value<1>(p) == 2);
/// \endcode

/// \brief 2D point both axes of which have the same dimension type
/// \ingroup PointModel
/// Models: Point2DConcept
template <typename T>
class point
{
public:
    using value_type = T;

    template<std::size_t D>
    struct axis
    {
        using coord_t = value_type;
    };

    static constexpr std::size_t num_dimensions = 2;

    point() = default;
    point(T px, T py) : x(px), y(py) {}

    point operator<<(std::ptrdiff_t shift) const
    {
        return point(x << shift, y << shift);
    }

    point operator>>(std::ptrdiff_t shift) const
    {
        return point(x >> shift, y >> shift);
    }

    point& operator+=(point const& p)
    {
        x += p.x;
        y += p.y;
        return *this;
    }

    point& operator-=(point const& p)
    {
        x -= p.x;
        y -= p.y;
        return *this;
    }

    point& operator/=(double t)
    {
        if (t < 0 || 0 < t) { x /= t; y /= t; }
        return *this;
    }

    T const& operator[](std::size_t i) const
    {
        return this->*mem_array[i];
    }

    T& operator[](std::size_t i)
    {
        return this->*mem_array[i];
    }

    T x{0};
    T y{0};

private:
    // this static array of pointers to member variables makes operator[] safe
    // and doesn't seem to exhibit any performance penalty.
    static T point<T>::* const mem_array[num_dimensions];
};

/// Alias template for backward compatibility with Boost <=1.68.
template <typename T>
using point2 = point<T>;

/// Common type to represent 2D dimensions or in-memory size of image or view.
/// @todo TODO: rename to dims_t or dimensions_t for purpose clarity?
using point_t = point<std::ptrdiff_t>;

template <typename T>
T point<T>::* const point<T>::mem_array[point<T>::num_dimensions] =
{
  & point<T>::x,
  & point<T>::y
};

/// \ingroup PointModel
template <typename T>
BOOST_FORCEINLINE
bool operator==(const point<T>& p1, const point<T>& p2)
{
    return (p1.x == p2.x && p1.y == p2.y);
}

/// \ingroup PointModel
template <typename T>
BOOST_FORCEINLINE
bool operator!=(const point<T>& p1, const point<T>& p2)
{
    return p1.x != p2.x || p1.y != p2.y;
}

/// \ingroup PointModel
template <typename T>
BOOST_FORCEINLINE
point<T> operator+(const point<T>& p1, const point<T>& p2)
{
    return point<T>(p1.x + p2.x, p1.y + p2.y);
}

/// \ingroup PointModel
template <typename T>
BOOST_FORCEINLINE
point<T> operator-(const point<T>& p)
{
    return point<T>(-p.x, -p.y);
}

/// \ingroup PointModel
template <typename T>
BOOST_FORCEINLINE
point<T> operator-(const point<T>& p1, const point<T>& p2)
{
    return point<T>(p1.x - p2.x, p1.y - p2.y);
}

/// \ingroup PointModel
template <typename T>
BOOST_FORCEINLINE
point<double> operator/(const point<T>& p, double t)
{
    return (t < 0 || 0 < t)
        ? point<double>(p.x / t, p.y / t)
        : point<double>(0, 0);
}

/// \ingroup PointModel
template <typename T>
BOOST_FORCEINLINE
point<T> operator*(const point<T>& p, std::ptrdiff_t t)
{
    return point<T>(p.x * t, p.y * t);
}

/// \ingroup PointModel
template <typename T>
BOOST_FORCEINLINE
point<T> operator*(std::ptrdiff_t t, const point<T>& p)
{
    return point<T>(p.x * t, p.y * t);
}

/// \ingroup PointModel
template <std::size_t K, typename T>
BOOST_FORCEINLINE
T const& axis_value(const point<T>& p) { return p[K]; }

/// \ingroup PointModel
template <std::size_t K, typename T>
BOOST_FORCEINLINE
T& axis_value(point<T>& p) { return p[K]; }

/// \addtogroup PointAlgorithm
///
/// Example:
/// \code
/// assert(iround(point<double>(3.1, 3.9)) == point<std::ptrdiff_t>(3,4));
/// \endcode

/// \ingroup PointAlgorithm
inline point<std::ptrdiff_t> iround(const point<float>& p)
{
    return point<std::ptrdiff_t>(iround(p.x), iround(p.y));
}

/// \ingroup PointAlgorithm
inline point<std::ptrdiff_t> iround(const point<double>& p)
{
    return point<std::ptrdiff_t>(iround(p.x), iround(p.y));
}

/// \ingroup PointAlgorithm
inline point<std::ptrdiff_t> ifloor(const point<float>& p)
{
    return point<std::ptrdiff_t>(ifloor(p.x), ifloor(p.y));
}

/// \ingroup PointAlgorithm
inline point<std::ptrdiff_t> ifloor(const point<double>& p)
{
    return point<std::ptrdiff_t>(ifloor(p.x), ifloor(p.y));
}

/// \ingroup PointAlgorithm
inline point<std::ptrdiff_t> iceil(const point<float>& p)
{
    return point<std::ptrdiff_t>(iceil(p.x), iceil(p.y));
}

/// \ingroup PointAlgorithm
inline point<std::ptrdiff_t> iceil(const point<double>& p)
{
    return point<std::ptrdiff_t>(iceil(p.x), iceil(p.y));
}

}} // namespace boost::gil

#endif
