//
// Copyright 2026 Samuel Debionne
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_DETAIL_CONFIG_HPP
#define BOOST_GIL_DETAIL_CONFIG_HPP

#include <boost/config.hpp>

#if !defined(BOOST_NO_CXX20_HDR_CONCEPTS) && defined(__cpp_lib_concepts)
#define BOOST_GIL_HAS_CONCEPTS
#endif

#ifdef  BOOST_GIL_HAS_CONCEPTS
#define BOOST_GIL_CONSTRAINT(C) C
#else
#define BOOST_GIL_CONSTRAINT(C) typename
#endif

#endif