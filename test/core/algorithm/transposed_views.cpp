//
// Copyright 2026 Samuel Debionne, ESRF
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include <boost/gil.hpp>

#include <boost/core/lightweight_test.hpp>

#include <cstdint>
#include <type_traits>

namespace gil = boost::gil;

// Fills the view with distinct values
template <typename View>
void fill_distinct(View const& view)
{
    int i = 0;
    for (std::ptrdiff_t y = 0; y < view.height(); ++y)
        for (std::ptrdiff_t x = 0; x < view.width(); ++x, ++i)
            gil::static_for_each(view(x, y), [&i](auto&& c) {
                using channel_t = typename gil::channel_traits<std::decay_t<decltype(c)>>::value_type;
                c = gil::channel_convert<channel_t>(static_cast<std::uint8_t>(i * 7 + 3));
            });
}

template <typename View1, typename View2>
bool equal_by_pixel(View1 const& v1, View2 const& v2)
{
    if (v1.dimensions() != v2.dimensions())
        return false;
    for (std::ptrdiff_t y = 0; y < v1.height(); ++y)
        for (std::ptrdiff_t x = 0; x < v1.width(); ++x)
            if (v1(x, y) != v2(x, y))
                return false;
    return true;
}

template <typename Pixel>
struct invert_fn
{
    Pixel operator()(Pixel const& p) const
    {
        Pixel r;
        gil::static_transform(p, r, [](auto const& c) {
            using channel_t = typename gil::channel_traits<std::decay_t<decltype(c)>>::value_type;
            return gil::channel_invert(channel_t(c));
        });
        return r;
    }
};

template <typename View1, typename View2>
bool equal_inverted_by_pixel(View1 const& v1, View2 const& v2)
{
    for (std::ptrdiff_t y = 0; y < v1.height(); ++y)
        for (std::ptrdiff_t x = 0; x < v1.width(); ++x)
            if (invert_fn<typename View1::value_type>{}(v1(x, y)) != v2(x, y))
                return false;
    return true;
}

// Image with extra hidden rows: accessing the last pixels of a bit-aligned image whose
// bit field spans several bytes reads past the end of the pixels (a GIL issue unrelated
// to the algorithms tested here). 32 rows of at least 1 bit cover a 32-bit bit field.
template <typename Image>
struct padded_image
{
    padded_image(std::ptrdiff_t width, std::ptrdiff_t height)
        : img(width, height + 32), width(width), height(height)
    {}

    typename Image::view_t view() { return gil::subimage_view(gil::view(img), 0, 0, width, height); }
    typename Image::const_view_t const_view() const
    {
        return gil::subimage_view(gil::const_view(img), 0, 0, width, height);
    }

    Image img;
    std::ptrdiff_t width;
    std::ptrdiff_t height;
};

template <typename Image>
void test_copy_and_transform(std::ptrdiff_t width, std::ptrdiff_t height)
{
    padded_image<Image> in(width, height);
    fill_distinct(in.view());
    auto const src = in.const_view();

    // transposed src -> regular dst
    {
        padded_image<Image> out(height, width);
        gil::copy_pixels(gil::transposed_view(src), out.view());
        BOOST_TEST(equal_by_pixel(gil::transposed_view(src), out.view()));

        padded_image<Image> out2(height, width);
        gil::transform_pixels(gil::transposed_view(src), out2.view(), invert_fn<typename Image::value_type>{});
        BOOST_TEST(equal_inverted_by_pixel(gil::transposed_view(src), out2.view()));
    }

    // regular src -> transposed dst
    {
        padded_image<Image> out(height, width);
        gil::copy_pixels(src, gil::transposed_view(out.view()));
        BOOST_TEST(equal_by_pixel(src, gil::transposed_view(out.view())));

        padded_image<Image> out2(height, width);
        gil::transform_pixels(src, gil::transposed_view(out2.view()), invert_fn<typename Image::value_type>{});
        BOOST_TEST(equal_inverted_by_pixel(src, gil::transposed_view(out2.view())));
    }

    // transposed src -> transposed dst
    {
        padded_image<Image> out(width, height);
        gil::copy_pixels(gil::transposed_view(src), gil::transposed_view(out.view()));
        BOOST_TEST(equal_by_pixel(src, out.view()));

        padded_image<Image> out2(width, height);
        gil::transform_pixels(gil::transposed_view(src), gil::transposed_view(out2.view()), invert_fn<typename Image::value_type>{});
        BOOST_TEST(equal_inverted_by_pixel(src, out2.view()));
    }

    // rotated src -> regular dst
    {
        padded_image<Image> out(height, width);
        gil::copy_pixels(gil::rotated90cw_view(src), out.view());
        BOOST_TEST(equal_by_pixel(gil::rotated90cw_view(src), out.view()));

        gil::copy_pixels(gil::rotated90ccw_view(src), out.view());
        BOOST_TEST(equal_by_pixel(gil::rotated90ccw_view(src), out.view()));
    }

    // transposed subimage src -> regular dst
    if (width > 2 && height > 2)
    {
        auto const sub = gil::subimage_view(src, 1, 1, width - 2, height - 2);
        padded_image<Image> out(height - 2, width - 2);
        gil::copy_pixels(gil::transposed_view(sub), out.view());
        BOOST_TEST(equal_by_pixel(gil::transposed_view(sub), out.view()));
    }

    // transposed src -> regular dst with color conversion
    {
        gil::bgr8_image_t out(height, width);
        gil::copy_and_convert_pixels(gil::transposed_view(src), gil::view(out));
        BOOST_TEST(equal_by_pixel(
            gil::color_converted_view<gil::bgr8_pixel_t>(gil::transposed_view(src)), gil::view(out)));
    }
}

template <typename Image>
void test_sizes()
{
    // Sizes that are and are not multiples of the block size, smaller and larger than a panel
    std::ptrdiff_t const sizes[] = {1, 2, 3, 4, 5, 7, 8, 13, 16, 33, 64, 67, 130};
    for (auto w : sizes)
        for (auto h : sizes)
            test_copy_and_transform<Image>(w, h);
}

// Byte-aligned packed pixels
using rgb565_image_t = gil::packed_image3_type<std::uint16_t, 5, 6, 5, gil::rgb_layout_t>::type;
// Bit-aligned packed pixels (several pixels per byte, or pixels straddling bytes)
using gray1_image_t = gil::bit_aligned_image1_type<1, gil::gray_layout_t>::type;
using rgb121_image_t = gil::bit_aligned_image3_type<1, 2, 1, gil::rgb_layout_t>::type;
using rgb565_bit_aligned_image_t = gil::bit_aligned_image3_type<5, 6, 5, gil::rgb_layout_t>::type;
using rgb343_image_t = gil::bit_aligned_image3_type<3, 4, 3, gil::rgb_layout_t>::type;

int main()
{
    test_sizes<gil::gray8_image_t>();
    test_sizes<gil::gray32f_image_t>();
    test_sizes<gil::rgb8_image_t>();
    test_sizes<gil::rgba16_image_t>();
    test_sizes<gil::rgb8_planar_image_t>();
    test_sizes<rgb565_image_t>();
    test_sizes<gray1_image_t>();
    test_sizes<rgb121_image_t>();
    test_sizes<rgb565_bit_aligned_image_t>();
    test_sizes<rgb343_image_t>();

    return ::boost::report_errors();
}
