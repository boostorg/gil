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
#include <boost/config/no_tr1/cmath.hpp>

#include <cstddef>

namespace boost { namespace gil {

/// \addtogroup PointModel
///
/// Example:
/// \code
/// point2<std::ptrdiff_t> p(3,2);
/// assert((p[0] == p.x) && (p[1] == p.y));
/// assert(axis_value<0>(p) == 3);
/// assert(axis_value<1>(p) == 2);
/// \endcode

/// \brief 2D point both axes of which have the same dimension type
/// \ingroup PointModel
/// Models: Point2DConcept
template <typename T>
class point2
{
public:
    using value_type = T;

    template<std::size_t D>
    struct axis
    {
        using coord_t = value_type;
    };

    static constexpr std::size_t num_dimensions = 2;

    point2() = default;
    point2(T px, T py) : x(px), y(py) {}
    point2(point2 const& p) : x(p.x), y(p.y) {}

    point2& operator=(point2 const& p)
    {
        x = p.x;
        y = p.y;
        return *this;
    }

    point2 operator<<(std::ptrdiff_t shift) const
    {
        return point2(x << shift, y << shift);
    }

    point2 operator>>(std::ptrdiff_t shift) const
    {
        return point2(x >> shift, y >> shift);
    }

    point2& operator+=(point2 const& p)
    {
        x += p.x;
        y += p.y;
        return *this;
    }

    point2& operator-=(point2 const& p)
    {
        x -= p.x;
        y -= p.y;
        return *this;
    }

    point2& operator/=(double t)
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
    static T point2<T>::* const mem_array[num_dimensions];
};

template <typename T>
T point2<T>::* const point2<T>::mem_array[point2<T>::num_dimensions] =
{
  & point2<T>::x,
  & point2<T>::y
};

/// \ingroup PointModel
template <typename T>
BOOST_FORCEINLINE
bool operator==(const point2<T>& p1, const point2<T>& p2)
{
    return (p1.x == p2.x && p1.y == p2.y);
}

/// \ingroup PointModel
template <typename T>
BOOST_FORCEINLINE
bool operator!=(const point2<T>& p1, const point2<T>& p2)
{
    return p1.x != p2.x || p1.y != p2.y;
}

/// \ingroup PointModel
template <typename T>
BOOST_FORCEINLINE
point2<T> operator+(const point2<T>& p1, const point2<T>& p2)
{
    return point2<T>(p1.x + p2.x, p1.y + p2.y);
}

/// \ingroup PointModel
template <typename T>
BOOST_FORCEINLINE
point2<T> operator-(const point2<T>& p)
{
    return point2<T>(-p.x, -p.y);
}

/// \ingroup PointModel
template <typename T>
BOOST_FORCEINLINE
point2<T> operator-(const point2<T>& p1, const point2<T>& p2)
{
    return point2<T>(p1.x - p2.x, p1.y - p2.y);
}

/// \ingroup PointModel
template <typename T>
BOOST_FORCEINLINE
point2<double> operator/(const point2<T>& p, double t)
{
    return (t < 0 || 0 < t)
        ? point2<double>(p.x / t, p.y / t)
        : point2<double>(0, 0);
}

/// \ingroup PointModel
template <typename T>
BOOST_FORCEINLINE
point2<T> operator*(const point2<T>& p, std::ptrdiff_t t)
{
    return point2<T>(p.x * t, p.y * t);
}

/// \ingroup PointModel
template <typename T>
BOOST_FORCEINLINE
point2<T> operator*(std::ptrdiff_t t, const point2<T>& p)
{
    return point2<T>(p.x * t, p.y * t);
}

/// \ingroup PointModel
template <std::size_t K, typename T>
BOOST_FORCEINLINE
T const& axis_value(const point2<T>& p) { return p[K]; }

/// \ingroup PointModel
template <std::size_t K, typename T>
BOOST_FORCEINLINE
T& axis_value(point2<T>& p) { return p[K]; }

/// \addtogroup PointAlgorithm
///
/// Example:
/// \code
/// assert(iround(point2<double>(3.1, 3.9)) == point2<std::ptrdiff_t>(3,4));
/// \endcode

/// \ingroup PointAlgorithm
inline point2<std::ptrdiff_t> iround(const point2<float>& p)
{
    return point2<std::ptrdiff_t>(iround(p.x), iround(p.y));
}

/// \ingroup PointAlgorithm
inline point2<std::ptrdiff_t> iround(const point2<double>& p)
{
    return point2<std::ptrdiff_t>(iround(p.x), iround(p.y));
}

/// \ingroup PointAlgorithm
inline point2<std::ptrdiff_t> ifloor(const point2<float>& p)
{
    return point2<std::ptrdiff_t>(ifloor(p.x), ifloor(p.y));
}

/// \ingroup PointAlgorithm
inline point2<std::ptrdiff_t> ifloor(const point2<double>& p)
{
    return point2<std::ptrdiff_t>(ifloor(p.x), ifloor(p.y));
}

/// \ingroup PointAlgorithm
inline point2<std::ptrdiff_t> iceil(const point2<float>& p)
{
    return point2<std::ptrdiff_t>(iceil(p.x), iceil(p.y));
}

/// \ingroup PointAlgorithm
inline point2<std::ptrdiff_t> iceil(const point2<double>& p)
{
    return point2<std::ptrdiff_t>(iceil(p.x), iceil(p.y));
}

}}  // namespace boost::gil

#endif
