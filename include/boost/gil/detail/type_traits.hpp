//
// Copyright 2017-2019 Peter Dimov.
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_DETAIL_TYPE_TRAITS_HPP
#define BOOST_GIL_DETAIL_TYPE_TRAITS_HPP

#include <boost/config.hpp>

#include <type_traits>

namespace boost { namespace gil { namespace detail {

#if defined(BOOST_LIBSTDCXX_VERSION) && BOOST_LIBSTDCXX_VERSION < 50000

template<class T>
struct is_trivially_default_constructible
    : std::integral_constant
    <
        bool,
        std::is_default_constructible<T>::value &&
        std::has_trivial_default_constructor<T>::value
    >
{};


template<class T>
struct is_trivially_destructible
    : std::integral_constant
    <
        bool,
        std::is_destructible<T>::value &&
        std::has_trivial_destructor<T>::value
    >
{};

#else

using std::is_trivially_default_constructible;
using std::is_trivially_destructible;

#endif

}}} //namespace boost::gil::detail

#endif
