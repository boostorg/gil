//
// Copyright 2019 Mateusz Loskot <mateusz at loskot dot net>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include <boost/gil/channel.hpp>
#include <boost/gil/packed_pixel.hpp>
#include <boost/gil/rgb.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/vector_c.hpp>
namespace gil = boost::gil;
namespace mpl = boost::mpl;

int main()
{
    using packed_channel_references_vector =
        typename gil::detail::packed_channel_references_vector_type
        <
            std::uint8_t,
            mpl::vector3_c<int, 1, 2, 1>
        >::type;

    using packed_pixel = gil::packed_pixel
        <
            std::uint8_t,
            packed_channel_references_vector,
            gil::bgr_layout_t
        >;

    // Verify packed_pixel members

    static_assert(std::is_same<packed_pixel::layout_t, gil::bgr_layout_t>::value,
        "layout should be bgr");

    static_assert(std::is_same<packed_pixel::value_type, packed_pixel>::value,
        "value_type member should be of the same type as the packed_pixel specialization");

    static_assert(std::is_reference<packed_pixel::reference>::value,
        "reference member should be a reference");

    static_assert(std::is_reference<packed_pixel::const_reference>::value,
        "const_reference member should be a reference");

    static_assert(std::is_same<decltype(packed_pixel::is_mutable), bool const>::value &&
        packed_pixel::is_mutable,
        "is_mutable should be boolean");

    // Verify metafunctions

    using channel_references_t = packed_channel_references_vector::type;

    static_assert(mpl::size<channel_references_t>::value == 3,
        "packed_channel_references_vector_type should define three references to channel start bits");

    using channel1_ref_t = mpl::at_c<channel_references_t, 0>::type;
    static_assert(channel1_ref_t::num_bits == 1,
        "1st channel of bgr121 pixel should be of 1-bit size");

    using channel2_ref_t = mpl::at_c<channel_references_t, 1>::type;
    static_assert(channel2_ref_t::num_bits == 2,
        "2nd channel of bgr121 pixel should be of 2-bit size");

    using channel3_ref_t = mpl::at_c<channel_references_t, 2>::type;
    static_assert(channel3_ref_t::num_bits == 1,
        "3rd channel of bgr121 pixel should be of 1-bit size");

    static_assert(std::is_same
        <
            channel1_ref_t,
            gil::packed_channel_reference<std::uint8_t, 0, 1, true> const
        >::value,
        "1st element of packed_channel_references_vector should be packed_channel_reference of 1st channel");

    static_assert(std::is_same
        <
            channel2_ref_t,
            gil::packed_channel_reference<std::uint8_t, 1, 2, true> const
        >::value,
        "2nd element of packed_channel_references_vector should be packed_channel_reference of 2nd channel");

    static_assert(std::is_same
        <
            channel3_ref_t,
            gil::packed_channel_reference<std::uint8_t, 3, 1, true> const
        >::value,
        "3rd element of packed_channel_references_vector should be packed_channel_reference of 3rd channel");

    // double check intermediate metafunction packed_channel_reference_type
    static_assert(std::is_same
        <
            gil::detail::packed_channel_reference_type<std::uint8_t, mpl::int_<0>, mpl::int_<1>>::type,
            channel1_ref_t
        >::value,
        "packed_channel_reference_type should return packed_channel_reference");
    static_assert(std::is_same
        <
            gil::detail::packed_channel_reference_type<std::uint8_t, mpl::int_<0>, mpl::int_<1>>::type,
            gil::packed_channel_reference<std::uint8_t, 0, 1, true> const
        >::value,
        "packed_channel_reference_type should return packed_channel_reference");

    return 0;
}
