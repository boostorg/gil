//
// Copyright 2021 Harsit Pant <harshitpant83@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/gil/image_processing/sharpening.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/algorithm.hpp>

#include <boost/core/lightweight_test.hpp>

using namespace boost;
using namespace gil;
using namespace std;

int const width = 9;
int const height = 9;

gray8_image_t gray_img_in(width, height);
gray8_image_t gray_img_out(width, height);
gray8_image_t gray_img_expected(width, height);

rgb8_image_t rgb_img_in(width, height);
rgb8_image_t rgb_img_out(width, height);
rgb8_image_t rgb_img_expected(width, height);

void no_edges()
{
    //all pixels same, no edge detected, thus nothing to sharpen.
    fill_pixels(view(gray_img_in), gray8_pixel_t(50));
    fill_pixels(view(gray_img_expected), gray8_pixel_t(50));

    sharpen(const_view(gray_img_in), view(gray_img_out), 2, 1, 0.5);

    BOOST_TEST(equal_pixels(const_view(gray_img_out), const_view(gray_img_expected)));
}

void no_edges_rgb()
{
    //all pixels same, no edge detected, thus nothing to sharpen.
    fill_pixels(view(rgb_img_in), rgb8_pixel_t(50, 50, 50));
    fill_pixels(view(rgb_img_expected), rgb8_pixel_t(50, 50, 50));

    sharpen(const_view(rgb_img_in), view(rgb_img_out), 1, 3, 0.1);

    BOOST_TEST(equal_pixels(const_view(rgb_img_out), const_view(rgb_img_expected)));
}

void amount_zero()
{
    // image remains unchanged if specified ammount is zero.
    uint8_t in[] =
    {
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53
    };

    uint8_t out[] =
    {
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53,
        12, 32, 224, 53, 255, 55, 32, 224, 53
    };
     
    gray8c_view_t in_view = interleaved_view(9, 9, reinterpret_cast<const gray8_pixel_t*>(in), 9);
    
    gray8c_view_t exp_view = interleaved_view(9, 9, reinterpret_cast<const gray8_pixel_t*>(out), 9);

    sharpen(in_view, view(gray_img_out), 1, 0);
    BOOST_TEST(equal_pixels(const_view(gray_img_out), exp_view));
}

void spike_produces_halo()
{
    // Sharpening a single bright spike on a flat background must boost the
    // spike itself and push its immediate neighbours *below* the background
    // level (the classic unsharp-mask overshoot/undershoot halo). Unlike the
    // tests above, this exercises an input where the correct output is
    // actually different from the input.
    int const w = 15;
    int const h = 3;

    gray8_image_t in(w, h);
    gray8_image_t out(w, h);
    fill_pixels(view(in), gray8_pixel_t(50));
    for (int y = 0; y < h; ++y)
        view(in)(w / 2, y) = gray8_pixel_t(200);

    sharpen(const_view(in), view(out), 1.0, 1.0, 0.0);

    uint8_t expected[] =
    {
        50, 50, 50, 50, 49, 42, 14, 255, 14, 42, 49, 50, 50, 50, 50,
        50, 50, 50, 50, 49, 42, 14, 255, 14, 42, 49, 50, 50, 50, 50,
        50, 50, 50, 50, 49, 42, 14, 255, 14, 42, 49, 50, 50, 50, 50
    };
    gray8c_view_t exp_view = interleaved_view(w, h, reinterpret_cast<const gray8_pixel_t*>(expected), w);

    BOOST_TEST(equal_pixels(const_view(out), exp_view));
}

void threshold_suppresses_weak_edge()
{
    // Two edges of different strength: a weak one (50 -> 70) and a strong
    // one (50 -> 250). With threshold = 0.5, only the strong edge (whose
    // contrast exceeds half of the image's maximum contrast) should be
    // sharpened; the weak edge must come out byte-for-byte identical to the
    // input.
    int const w = 40;
    int const h = 3;

    gray8_image_t in(w, h);
    gray8_image_t out(w, h);
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            uint8_t v;
            if (x < w / 2)
                v = (x < w / 4) ? 50 : 70;
            else
                v = (x < 3 * w / 4) ? 50 : 250;
            view(in)(x, y) = gray8_pixel_t(v);
        }
    }

    sharpen(const_view(in), view(out), 1.0, 1.0, 0.5);

    uint8_t expected[] =
    {
        50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,
        50, 50, 50, 50, 50, 50, 50, 50, 50,  0, 255, 250, 250, 250, 250, 250, 250, 250, 250, 250,
        50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,
        50, 50, 50, 50, 50, 50, 50, 50, 50,  0, 255, 250, 250, 250, 250, 250, 250, 250, 250, 250,
        50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,
        50, 50, 50, 50, 50, 50, 50, 50, 50,  0, 255, 250, 250, 250, 250, 250, 250, 250, 250, 250
    };
    gray8c_view_t exp_view = interleaved_view(w, h, reinterpret_cast<const gray8_pixel_t*>(expected), w);

    // The weak edge (columns [0, w/2)) must be completely unaffected.
    BOOST_TEST(equal_pixels(
        subimage_view(const_view(out), 0, 0, w / 2, h),
        subimage_view(const_view(in), 0, 0, w / 2, h)));

    BOOST_TEST(equal_pixels(const_view(out), exp_view));
}

int main()
{
    no_edges();
    no_edges_rgb();
    amount_zero();
    spike_produces_halo();
    threshold_suppresses_weak_edge();
    return ::boost::report_errors();
}
