#include <boost/core/lightweight_test.hpp>

#include <boost/gil/extension/blaze/blaze.hpp>

namespace gil = boost::gil;

void vector1d_zero_to_gray8()
{
    blaze::StaticVector<std::uint8_t, 1> v({0});
    auto p = gil::vector_to_pixel<gil::gray8_pixel_t>(v);

    BOOST_TEST_EQ(p[0], 0);
}

void vector1d_to_gray8()
{
    blaze::StaticVector<std::uint8_t, 1> v({23});
    auto p = gil::vector_to_pixel<gil::gray8_pixel_t>(v);

    BOOST_TEST_EQ(p[0], 23);
}

void vector3d_zero_to_rgb8()
{
    blaze::StaticVector<std::uint8_t, 3> v({0, 0, 0});
    auto p = gil::vector_to_pixel<gil::gray8_pixel_t>(v);

    BOOST_TEST_EQ(p[0], 0);
    BOOST_TEST_EQ(p[1], 0);
    BOOST_TEST_EQ(p[2], 0);
}

void vector3d_uniform_to_rgb8()
{
    std::uint8_t value = 23;
    blaze::StaticVector<std::uint8_t, 3> v({value, value, value});

    auto p = gil::vector_to_pixel<gil::rgb8_pixel_t>(v);

    BOOST_TEST_EQ(p[0], value);
    BOOST_TEST_EQ(p[1], value);
    BOOST_TEST_EQ(p[2], value);
}

void vector3d_distinct_to_rgb8()
{
    std::uint8_t value0 = 23;
    std::uint8_t value1 = 11;
    std::uint8_t value2 = 1;
    blaze::StaticVector<std::uint8_t, 3> v({value0, value1, value2});
    auto p = gil::vector_to_pixel<gil::rgb8_pixel_t>(v);

    BOOST_TEST_EQ(p[0], value0);
    BOOST_TEST_EQ(p[1], value1);
    BOOST_TEST_EQ(p[2], value2);
}

int main()
{
    vector1d_zero_to_gray8();
    vector1d_to_gray8();
    vector3d_zero_to_rgb8();
    vector3d_uniform_to_rgb8();
    vector3d_distinct_to_rgb8();

    return boost::report_errors();
}
