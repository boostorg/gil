//
// Copyright 2005-2007 Adobe Systems Incorporated
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_CONCEPTS_PIXEL_DEREFERENCE_HPP
#define BOOST_GIL_CONCEPTS_PIXEL_DEREFERENCE_HPP

#include <boost/gil/concepts/basic.hpp>
#include <boost/gil/concepts/concept_check.hpp>
#include <boost/gil/concepts/fwd.hpp>
#include <boost/gil/concepts/pixel.hpp>
#include <boost/gil/concepts/detail/type_traits.hpp>

#include <boost/concept_check.hpp>
#include <boost/type_traits.hpp>

#include <cstddef>

#if BOOST_GCC >= 40700
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif

namespace boost { namespace gil {

/// \ingroup PixelDereferenceAdaptorConcept
/// \brief Represents a unary function object that can be invoked upon dereferencing a pixel iterator.
///
/// This can perform an arbitrary computation, such as color conversion or table lookup.
/// \code
/// concept PixelDereferenceAdaptorConcept<boost::UnaryFunctionConcept D>
///     : DefaultConstructibleConcept<D>, CopyConstructibleConcept<D>, AssignableConcept<D>
/// {
///     typename const_t; where PixelDereferenceAdaptorConcept<const_t>;
///     typename value_type; where PixelValueConcept<value_type>;
///     typename reference;         // may be mutable
///     typename const_reference;   // must not be mutable
///     static const bool D::is_mutable;
///
///     where Convertible<value_type,result_type>;
/// };
/// \endcode
template <typename D>
struct PixelDereferenceAdaptorConcept
{
    void constraints()
    {
        gil_function_requires
        <
            boost::UnaryFunctionConcept
            <
                D,
                typename detail::remove_const_and_reference<typename D::result_type>::type,
                typename D::argument_type
            >
        >();
        gil_function_requires<boost::DefaultConstructibleConcept<D>>();
        gil_function_requires<boost::CopyConstructibleConcept<D>>();
        gil_function_requires<boost::AssignableConcept<D>>();

        gil_function_requires<PixelConcept<typename detail::remove_const_and_reference<typename D::result_type>::type>>();

        typedef typename D::const_t const_t;
        gil_function_requires<PixelDereferenceAdaptorConcept<const_t>>();
        typedef typename D::value_type value_type;
        gil_function_requires<PixelValueConcept<value_type>>();
        typedef typename D::reference reference;                // == PixelConcept (if you remove const and reference)
        typedef typename D::const_reference const_reference;    // == PixelConcept (if you remove const and reference)

        const bool is_mutable = D::is_mutable;
        ignore_unused_variable_warning(is_mutable);
    }
    D d;
};

template <typename P>
struct PixelDereferenceAdaptorArchetype
{
    typedef P argument_type;
    typedef P result_type;
    typedef PixelDereferenceAdaptorArchetype const_t;
    typedef typename remove_reference<P>::type value_type;
    typedef typename add_reference<P>::type reference;
    typedef reference const_reference;
    static const bool is_mutable = false;
    P operator()(P) const { throw; }
};

}} // namespace boost::gil

#if BOOST_GCC >= 40700
#pragma GCC diagnostic pop
#endif

#endif
