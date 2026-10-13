//
// Copyright 2026 Samuel Debionne <samuel.debionne@esrf.fr>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include <boost/gil.hpp>

#include <boost/core/lightweight_test.hpp>

#include <cstdint>
#include <memory>

namespace gil = boost::gil;

// Test cases of out-of-bounds access to the last pixel of bit-aligned images,
// see https://github.com/boostorg/gil/issues/808
// A channel reference must only touch the bytes containing its bits, not the
// whole bit field. The main goal of these test cases is to trigger any memory
// error detectors (e.g. AddressSanitizer).

// rgb121 pixels are 4 bits wide, stored in a 16-bit bit field
using rgb121_image_t = gil::bit_aligned_image3_type<1, 2, 1, gil::rgb_layout_t>::type;

void test_write_read_every_pixel()
{
    for (std::ptrdiff_t w = 1; w <= 9; ++w)
    {
        for (std::ptrdiff_t h = 1; h <= 3; ++h)
        {
            rgb121_image_t image(w, h);
            auto view = gil::view(image);
            for (std::ptrdiff_t y = 0; y < h; ++y)
            {
                for (std::ptrdiff_t x = 0; x < w; ++x)
                {
                    auto p = view(x, y);
                    gil::get_color(p, gil::red_t()) = (x + y) % 2;
                    gil::get_color(p, gil::green_t()) = (x + y) % 4;
                    gil::get_color(p, gil::blue_t()) = (x + y + 1) % 2;
                }
            }
            for (std::ptrdiff_t y = 0; y < h; ++y)
            {
                for (std::ptrdiff_t x = 0; x < w; ++x)
                {
                    auto p = view(x, y);
                    BOOST_TEST_EQ(int(gil::get_color(p, gil::red_t())), (x + y) % 2);
                    BOOST_TEST_EQ(int(gil::get_color(p, gil::green_t())), (x + y) % 4);
                    BOOST_TEST_EQ(int(gil::get_color(p, gil::blue_t())), (x + y + 1) % 2);
                }
            }
        }
    }
}

void test_copy_pixels()
{
    for (std::ptrdiff_t w = 1; w <= 9; ++w)
    {
        rgb121_image_t src(w, 1);
        auto view = gil::view(src);
        for (std::ptrdiff_t x = 0; x < w; ++x)
        {
            gil::get_color(view(x, 0), gil::red_t()) = x % 2;
            gil::get_color(view(x, 0), gil::green_t()) = x % 4;
            gil::get_color(view(x, 0), gil::blue_t()) = (x + 1) % 2;
        }

        rgb121_image_t dst(w, 1);
        gil::copy_pixels(gil::const_view(src), gil::view(dst));
        BOOST_TEST(gil::equal_pixels(gil::const_view(src), gil::const_view(dst)));
    }
}

void test_packed_channel_reference_last_byte()
{
    // Channel occupying bits [5, 8) of a 16-bit bit field fits in the first byte
    std::unique_ptr<unsigned char[]> buffer(new unsigned char[1]{0});

    gil::packed_channel_reference<std::uint16_t, 5, 3, true> ref(buffer.get());
    ref = 5;
    BOOST_TEST_EQ(int(ref), 5);
    BOOST_TEST_EQ(int(buffer[0]), 5 << 5);

    gil::packed_channel_reference<std::uint16_t, 5, 3, false> cref(buffer.get());
    BOOST_TEST_EQ(int(cref), 5);

    gil::packed_dynamic_channel_reference<std::uint16_t, 3, true> dref(buffer.get(), 5);
    dref = 3;
    BOOST_TEST_EQ(int(dref), 3);
    BOOST_TEST_EQ(int(buffer[0]), 3 << 5);
}

int main()
{
    test_write_read_every_pixel();
    test_copy_pixels();
    test_packed_channel_reference_last_byte();

    return ::boost::report_errors();
}
