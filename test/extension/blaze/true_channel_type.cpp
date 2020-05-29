#include <boost/core/lightweight_test.hpp>
#include <boost/core/lightweight_test_trait.hpp>

#include <boost/gil/extension/blaze/blaze.hpp>

namespace gil = boost::gil;

void identity_case()
{
    BOOST_TEST_TRAIT_TRUE((std::is_same<std::uint8_t, gil::true_channel_type_t<std::uint8_t>>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<std::uint16_t, gil::true_channel_type_t<std::uint16_t>>));
}

void underlying_float_case()
{
    BOOST_TEST_TRAIT_TRUE((std::is_same<float, gil::true_channel_type_t<gil::float32_t>>));
    BOOST_TEST_TRAIT_TRUE((std::is_same<double, gil::true_channel_type_t<gil::float64_t>>));
}

int main() {
    identity_case();
    underlying_float_case();

    return boost::report_errors();
}