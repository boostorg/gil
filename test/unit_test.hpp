//
// Copyright 2018 Mateusz Loskot <mateusz at loskot dot net>
//
// Distribtted under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_TEST_UNIT_TEST_HPP
#define BOOST_GIL_TEST_UNIT_TEST_HPP

#include <boost/config.hpp>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4702) // unreachable code
#endif

#if defined(BOOST_CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#pragma clang diagnostic ignored "-Wsign-conversion"
#endif

#if defined(BOOST_GCC) && (BOOST_GCC >= 40900)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wshadow"
#endif

#include <boost/test/unit_test.hpp>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#if defined(BOOST_CLANG)
#pragma clang diagnostic pop
#endif

#if defined(BOOST_GCC) && (BOOST_GCC >= 40900)
#pragma GCC diagnostic pop
#endif

#include <boost/core/ignore_unused.hpp>
#include <boost/core/typeinfo.hpp>

#include <boost/gil/color_base_algorithm.hpp> // static_for_each
#include <boost/gil/iterator_from_2d.hpp>
#include <boost/gil/packed_pixel.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/gil/planar_pixel_reference.hpp>
#include <boost/gil/promote_integral.hpp>

#include <cstdint>
#include <ostream>

namespace btt = boost::test_tools;
namespace but = boost::unit_test;

namespace boost { namespace gil {

namespace detail {

struct print_color_base
{
    std::ostream& os_;
    std::size_t element_index_{0};
    print_color_base(std::ostream& os) : os_(os) {}

    template <typename Element>
    void operator()(Element& c)
    {
        typename ::boost::gil::promote_integral<Element>::type const v(c);
        if (element_index_ > 0) os_ << ", ";
        os_ << "v" << element_index_ << "=" << v;
        ++element_index_;
    }
};

} // namespace detail

// Make `point` printable for BOOST_TEST()
template <typename T>
std::ostream& operator<<(std::ostream& os, point<T> const& p)
{
    os << "point<" << boost::core::demangled_name(typeid(T)) << ">";
    os << "(" << p.x << ", " << p.y << ")" << std::endl;
    return os;
}

// Make `pixel` printable for BOOST_TEST()
template <typename ChannelValue, typename Layout>
std::ostream& operator<<(std::ostream& os, pixel<ChannelValue, Layout> const& p)
{
    os << "pixel<"
       << "Channel=" << boost::core::demangled_name(typeid(ChannelValue))
       << ", Layout=" << boost::core::demangled_name(typeid(Layout))
       << ">(";

    static_for_each(p, detail::print_color_base{os});
    os << ")" << std::endl;
    return os;
}

template <typename BitField, typename ChannelRefs, typename Layout>
std::ostream& operator<<(std::ostream& os, gil::packed_pixel<BitField, ChannelRefs, Layout> const& p)
{
    os << "packed_pixel<"
       << "BitField=" << boost::core::demangled_name(typeid(BitField))
       << ", ChannelRefs="  << boost::core::demangled_name(typeid(ChannelRefs))
       << ", Layout=" << boost::core::demangled_name(typeid(Layout))
        << ">(" << (std::uint64_t)p._bitfield << ")";
    return os;
}

}} // namespace boost::gil

#endif
