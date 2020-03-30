//
// Copyright 2019-2020 Mateusz Loskot <mateusz at loskot dot net>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include <boost/gil.hpp>

#include <boost/core/lightweight_test.hpp>

#include "test_fixture.hpp"
#include "test_utility_output_stream.hpp"
#include "core/pixel/test_fixture.hpp"

namespace gil = boost::gil;
namespace fixture = boost::gil::test::fixture;

struct test_constructor_with_dimensions_pixel
{
    template <typename Image>
    void operator()(Image const &)
    {
        using image_t = Image;
        gil::point_t const dimensions{256, 128};
        using pixel_t = typename image_t::view_t::value_type;
        pixel_t const rnd_pixel = fixture::pixel_generator<pixel_t>::random();
        image_t image(dimensions, rnd_pixel);
        BOOST_TEST_EQ(image.width(), dimensions.x);
        BOOST_TEST_EQ(image.height(), dimensions.y);

        for (pixel_t const &p : gil::view(image))
            BOOST_TEST_EQ(p, rnd_pixel);
    }
    static void run()
    {
        boost::mp11::mp_for_each<fixture::image_types>(test_constructor_with_dimensions_pixel{});
    }
};

struct test_move_constructor
{
    template <typename Image>
    void operator()(Image const&)
    {
        using image_t = Image;
        gil::point_t const dimensions{256, 128};
        {
            image_t image(fixture::create_image<image_t>(dimensions.x, dimensions.y, 0));

            image_t image2(std::move(image));
            BOOST_TEST_EQ(image2.dimensions(), dimensions);
            BOOST_TEST_EQ(image.dimensions(), gil::point_t{});
        }
    }
    static void run()
    {
        boost::mp11::mp_for_each<fixture::image_types>(test_move_constructor{});
    }
};

struct test_move_assignement
{
    template <typename Image>
    void operator()(Image const&)
    {
        using image_t = Image;
        gil::point_t const dimensions{256, 128};
        {
            image_t image = fixture::create_image<image_t>(dimensions.x, dimensions.y, 0);
            image_t image2 = fixture::create_image<image_t>(dimensions.x * 2, dimensions.y * 2, 1);
            image2 = std::move(image);
            BOOST_TEST_EQ(image2.dimensions(), dimensions);
            BOOST_TEST_EQ(image.dimensions(), gil::point_t{});
        }
    }
    static void run()
    {
        boost::mp11::mp_for_each<fixture::image_types>(test_move_assignement{});
    }
};

int main()
{
    test_constructor_with_dimensions_pixel::run();

    test_move_constructor::run();
    test_move_assignement::run();

    return ::boost::report_errors();
}
