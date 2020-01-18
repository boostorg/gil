// Boost.GIL (Generic Image Library)
//
// Copyright (c) 2015, Oracle and/or its affiliates.
// Contributed and/or modified by Menelaos Karavelas, on behalf of Oracle
//
// Licensed under the Boost Software License version 1.0.
// http://www.boost.org/users/license.html
//
// Source: Boost.Geometry (aka GGL, Generic Geometry Library)
// Modifications: adapted for Boost.GIL
//  - Rename namespace boost::geometry to boost::gil
//  - Rename include guards
//  - Remove support for boost::multiprecision types
//  - Remove support for 128-bit integer types
//
#ifndef BOOST_GIL_PROMOTE_INTEGRAL_HPP
#define BOOST_GIL_PROMOTE_INTEGRAL_HPP

#include <boost/mp11/list.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/utility.hpp>
#include <iterator>
#include <climits>
#include <cstddef>
#include <type_traits>
namespace mp11 = boost::mp11;
namespace boost { namespace gil
{

namespace detail { namespace promote_integral
{

// meta-function that returns the bit size of a type
template
<
    typename T,
    bool IsFundamental = std::is_fundamental<T>::value
>
struct bit_size {};

// for fundamental types, just return CHAR_BIT * sizeof(T)
template <typename T>
struct bit_size<T, true> : std::integral_constant<std::size_t, (CHAR_BIT * sizeof(T))> {};

/*promote to larger function uses mp_cond , it checks if bit_size 
  of type is greater than MinSize , if true then type is selected
  else it checks for next type for promotion , until condition becomes true .
  in a small list of 4-5 type comparision , a simple mp_cond can work fine
  else constexpr can be used to iterate over a large list for promote_to_larger 
  function .

 */
 template
 <  
    class T,
    std::size_t MinSize, 
    bool is_unsigned
>
struct promote_to_larger{};

//specialization for unsinged type promotion
template< class T,std::size_t MinSize>
struct promote_to_larger<T,MinSize, true >
{    
    /*
     unsigned short, unsigned int, unsigned long, std::size_t, boost::ulong_long_type
    */
     using type = typename mp11::mp_cond
    < 
        mp11::mp_bool<sizeof(unsigned short)>= MinSize>,
        unsigned short,
        mp11::mp_bool<sizeof(unsigned int)>= MinSize>,
        unsigned int,
        mp11::mp_bool<sizeof(unsigned long)>=MinSize>,
        unsigned long,
        mp11::mp_bool<sizeof(std::size_t)>=MinSize>,
        std::size_t,
#if defined(BOOST_HAS_LONG_LONG)
        mp11::mp_bool<sizeof(boost::ulong_long_type)>=MinSize>,
        boost::ulong_long_type,
#endif
        mp11::mp_true,
        T
     
    // if promotion fails, keep the number T
    // (and cross fingers that overflow will not occur)
     >; 
};

template< class T,std::size_t MinSize>
struct promote_to_larger<T,MinSize,false >
{    
    /*
      short, int, long, boost::long_long_type
    */
     using type = typename mp11::mp_cond
    < 
        mp11::mp_bool<sizeof(short)>= MinSize>,
        short,
        mp11::mp_bool<sizeof(int)>= MinSize>,
        int,
        mp11::mp_bool<sizeof(long)>=MinSize>,
        long,
#if defined(BOOST_HAS_LONG_LONG)
        mp11::mp_bool<sizeof(boost::long_long_type)>=MinSize>,
        boost::long_long_type,
#endif
        mp11::mp_true,
        T
     
    // if promotion fails, keep the number T
    // (and cross fingers that overflow will not occur)
     >; 
};
}} // namespace detail::promote_integral

/*!
    \brief Meta-function to define an integral type with size
    than is (roughly) twice the bit size of T
    \ingroup utility
    \details
    This meta-function tries to promote the fundamental integral type T
    to a another integral type with size (roughly) twice the bit size of T.

    To do this, two times the bit size of T is tested against the bit sizes of:
         short, int, long, boost::long_long_type
    and the one that first matches is chosen.

    For unsigned types the bit size of T is tested against the bit
    sizes of the types above, if T is promoted to a signed type, or
    the bit sizes of
         unsigned short, unsigned int, unsigned long, std::size_t,
         boost::ulong_long_type
    if T is promoted to an unsigned type.

    By default an unsigned type is promoted to a signed type.
    This behavior is controlled by the PromoteUnsignedToUnsigned
    boolean template parameter, whose default value is "false".
    To promote an unsigned type to an unsigned type set the value of
    this template parameter to "true".

    Finally, if the passed type is either a floating-point type or a
    user-defined type it is returned as is.

    \note boost::long_long_type and boost::ulong_long_type are
    considered only if the macro BOOST_HAS_LONG_LONG is defined

*/
template
<
    typename T,
    bool PromoteUnsignedToUnsigned = false,
    bool UseCheckedInteger = false,
    bool IsIntegral = std::is_integral<T>::value
>
class promote_integral
{
private:
    static bool const is_unsigned = std::is_unsigned<T>::value;

    using bit_size_type = detail::promote_integral::bit_size<T>;

    // Define the minimum size (in bits) needed for the promoted type
    // If T is the input type and P the promoted type, then the
    // minimum number of bits for P are (below b stands for the number
    // of bits of T):
    // * if T is unsigned and P is unsigned: 2 * b
    // * if T is signed and P is signed: 2 * b - 1
    // * if T is unsigned and P is signed: 2 * b + 1
    using min_bit_size_type = typename std::conditional
        <
            (PromoteUnsignedToUnsigned && is_unsigned),
            std::integral_constant<std::size_t, (2 * bit_size_type::value)>,
            typename std::conditional
                <
                    is_unsigned,
                    std::integral_constant<std::size_t, (2 * bit_size_type::value + 1)>,
                    std::integral_constant<std::size_t, (2 * bit_size_type::value - 1)>
                >::type
        >::type;

    // Define the list of signed integral types we are going to use
    // for promotion
    using signed_integral_types = mp11::mp_list
        <
            short, int, long
#if defined(BOOST_HAS_LONG_LONG)
            , boost::long_long_type
#endif
        >;

    // Define the list of unsigned integral types we are going to use
    // for promotion
    using unsigned_integral_types = mp11::mp_list
        <
            unsigned short, unsigned int, unsigned long, std::size_t
#if defined(BOOST_HAS_LONG_LONG)
            , boost::ulong_long_type
#endif
        >;

    // Define the list of integral types that will be used for
    // promotion (depending in whether we was to promote unsigned to
    // unsigned or not)
    using integral_types = typename std::conditional
        <
            (is_unsigned && PromoteUnsignedToUnsigned),
            unsigned_integral_types,
            signed_integral_types
        >::type;

public:  
    using type = typename detail::promote_integral::promote_to_larger
        <
            T,
            min_bit_size_type::value, 
            (is_unsigned&&PromoteUnsignedToUnsigned)
        >::type;
};


template <typename T, bool PromoteUnsignedToUnsigned, bool UseCheckedInteger>
class promote_integral
    <
        T, PromoteUnsignedToUnsigned, UseCheckedInteger, false
    >
{
public:
    using type = T;
};

}} // namespace boost::gil

#endif // BOOST_GIL_PROMOTE_INTEGRAL_HPP
