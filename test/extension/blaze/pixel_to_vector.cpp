#include <boost/core/lightweight_test.hpp>

#include <boost/gil/extension/blaze/blaze.hpp>

namespace gil = boost::gil;

void gray8_zero_to_vector()
{
    gil::gray8_pixel_t p(0);
    auto v = gil::pixel_to_vector(p);
    BOOST_TEST_EQ(v[0], 0);
}

void gray8_to_vector()
{
    gil::gray8_pixel_t p(23);
    auto v = gil::pixel_to_vector(p);
    BOOST_TEST_EQ(v[0], 23);
}

void rgb8_zero_to_vector()
{
    gil::rgb8_pixel_t p(0, 0, 0);
    auto vector = gil::pixel_to_vector(p);
    BOOST_TEST_EQ(vector[0], 0);
    BOOST_TEST_EQ(vector[1], 0);
    BOOST_TEST_EQ(vector[2], 0);
}

void rgb8_uniform_to_vector()
{
    auto value = std::uint8_t(23);
    gil::rgb8_pixel_t p(value, value, value);
    auto vector = gil::pixel_to_vector(p);
    BOOST_TEST_EQ(vector[0], value);
    BOOST_TEST_EQ(vector[1], value);
    BOOST_TEST_EQ(vector[2], value);
}

void rgb8_distinct_to_vector()
{
    auto value0 = std::uint8_t(23);
    auto value1 = std::uint8_t(11);
    auto value2 = std::uint8_t(1);
    gil::rgb8_pixel_t p(value0, value1, value2);
    auto vector = gil::pixel_to_vector(p);
    BOOST_TEST_EQ(vector[0], value0);
    BOOST_TEST_EQ(vector[1], value1);
    BOOST_TEST_EQ(vector[2], value2);
}

int main()
{
    gray8_zero_to_vector();
    gray8_to_vector();
    rgb8_zero_to_vector();
    rgb8_uniform_to_vector();
    rgb8_distinct_to_vector();

    return boost::report_errors();
}
