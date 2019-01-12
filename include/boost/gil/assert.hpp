//
// Copyright 2019 Mateusz Łoskot
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_ASSERT_HPP
#define BOOST_GIL_ASSERT_HPP

// To selectively disable BOOST_ASSERT without affecting the definition of the
// standard assert define BOOST_DISABLE_ASSERTS when <boost/assert.hpp>
// is included, regardless of whether NDEBUG is defined.
// See https://www.boost.org/libs/assert

// TODO: If BOOST_ASSERT is replaced with custom BOOST_GIL_ASSERT,
//       ensure the new macro respects Boost-wide BOOST_DISABLE_ASSERTS.

#include <boost/assert.hpp>

#endif
