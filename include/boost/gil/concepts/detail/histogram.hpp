//
// Copyright 2020 Debabrata Mandal <mandaldebabrata123@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//

#ifndef BOOST_GIL_CONCEPTS_HISTOGRAM_HPP
#define BOOST_GIL_CONCEPTS_HISTOGRAM_HPP

#include <type_traits>

namespace boost { namespace gil {

namespace detail{

// Used to check if histogram container implements operator[] and operator++
template <typename T, typename = int>
struct is_indexable : std::false_type {} ;

template <typename T>
struct is_indexable
        <
            T,
            decltype((void) ++declval<T>()[0] , 0)
        > : std::true_type {} ;

// Used to check if histogram container is resizable i.e. implements resize() member function
template <typename T, typename = int>
struct is_resizable : std::false_type {} ;

template <typename T>
struct is_resizable
        <
            T,
            decltype((void) declval<T>().resize(1), 0)
        > : std::true_type {} ;

// Used to check if histogram container is key-value type and the key is of std::integral type
template <typename T, typename = void>
struct has_integral_key_type : std::false_type {} ;

template <typename T>
struct has_integral_key_type
        <
            T,
            typename std::enable_if<std::is_integral<typename T::key_type >::value >::type 
        > : std::true_type {} ;

} // namespace detail

}} // namespace boost::gil

#endif
