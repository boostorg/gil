#include <boost/core/lightweight_test.hpp>
#include <boost/core/lightweight_test_trait.hpp>

#include <boost/gil/algorithm.hpp>
#include <boost/gil/extension/blaze/blaze.hpp>

namespace gil = boost::gil;

void gray8_image_to_matrix_conversion_typecheck()
{
    gil::gray8_image_t input(16, 16, gil::gray8_pixel_t(13));
    auto matrix = gil::to_matrix_channeled(gil::view(input));
    BOOST_TEST_TRAIT_TRUE((std::is_same<blaze::StaticVector<std::uint8_t, 1>,
                                  blaze::UnderlyingElement_t<decltype(matrix)>>));
}

void gray8_image_to_matrix_values_check()
{
    gil::gray8_image_t input(16, 16, gil::gray8_pixel_t(13));
    auto matrix = gil::to_matrix_channeled(gil::view(input));
    blaze::DynamicMatrix<blaze::StaticVector<std::uint8_t, 1>> expected(16, 16, {13});

    BOOST_TEST(matrix == expected);
}

void gray8_image_with_differing_values_to_matrix_value_check()
{
    gil::gray8_image_t input(16, 16, gil::gray8_pixel_t(13));
    auto view = gil::view(input);
    view(0, 0)[0] = 0;
    view(1, 0)[0] = 1; // rows and cols are different for GIL vs Blaze

    auto matrix = gil::to_matrix_channeled(gil::view(input));
    blaze::DynamicMatrix<blaze::StaticVector<std::uint8_t, 1>> expected(16, 16, {13});
    expected(0, 0)[0] = 0;
    expected(0, 1)[0] = 1;

    BOOST_TEST(matrix == expected);
}

void rgb8_image_to_matrix_typecheck()
{
    gil::rgb8_pixel_t default_pixel(1, 2, 3);
    gil::rgb8_image_t input(16, 16, default_pixel);

    blaze::StaticVector<std::uint8_t, 3> default_vector({1, 2, 3});
    blaze::DynamicMatrix<blaze::StaticVector<std::uint8_t, 3>> expected(16, 16, default_vector);

    auto result = gil::to_matrix_channeled(gil::view(input));
    BOOST_TEST_TRAIT_TRUE((std::is_same<blaze::StaticVector<std::uint8_t, 3>,
                                  blaze::UnderlyingElement_t<decltype(result)>>));
}

void rgb8_image_to_matrix_value_check()
{
    gil::rgb8_pixel_t default_pixel(1, 2, 3);
    gil::rgb8_image_t input(16, 16, default_pixel);

    blaze::StaticVector<std::uint8_t, 3> default_vector({1, 2, 3});
    blaze::DynamicMatrix<blaze::StaticVector<std::uint8_t, 3>> expected(16, 16, default_vector);

    auto result = gil::to_matrix_channeled(gil::view(input));
    BOOST_TEST(result == expected);
}

void rgb8_image_with_differing_values_to_matrix_value_check()
{
    gil::rgb8_pixel_t default_pixel(1, 2, 3);
    gil::rgb8_image_t input(16, 16, default_pixel);
    auto view = gil::view(input);
    view(0, 0) = gil::rgb8_pixel_t(10, 20, 30);
    view(1, 0) = gil::rgb8_pixel_t(50, 50, 50);

    blaze::StaticVector<std::uint8_t, 3> default_vector({1, 2, 3});
    blaze::DynamicMatrix<blaze::StaticVector<std::uint8_t, 3>> expected(16, 16, default_vector);
    expected(0, 0) = {10, 20, 30};
    expected(0, 1) = {50, 50, 50};

    auto result = gil::to_matrix_channeled(gil::view(input));
    BOOST_TEST(result == expected);
}

int main()
{
    gray8_image_to_matrix_conversion_typecheck();
    gray8_image_to_matrix_values_check();
    gray8_image_with_differing_values_to_matrix_value_check();
    rgb8_image_to_matrix_typecheck();
    rgb8_image_to_matrix_value_check();
    rgb8_image_with_differing_values_to_matrix_value_check();
    return boost::report_errors();
}