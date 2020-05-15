#include <boost/core/lightweight_test.hpp>

#include <boost/gil/extension/blaze/blaze.hpp>

namespace gil = boost::gil;

void gray8_to_matrix_uniform()
{
    gil::gray8_image_t input(16, 16, gil::gray8_pixel_t(13));
    auto matrix = gil::to_matrix_channeled(gil::view(input));
    blaze::DynamicMatrix<blaze::StaticVector<std::uint8_t, 1>> expected(16, 16, {13});

    BOOST_TEST_EQ(matrix, expected);
}

void gray8_to_matrix_distinct()
{
    gil::gray8_image_t input(16, 16, gil::gray8_pixel_t(13));
    auto view = gil::view(input);
    view(0, 0)[0] = 0;
    view(1, 0)[0] = 1; // rows and cols are different for GIL vs Blaze

    auto matrix = gil::to_matrix_channeled(gil::view(input));
    blaze::DynamicMatrix<blaze::StaticVector<std::uint8_t, 1>> expected(16, 16, {13});
    expected(0, 0)[0] = 0;
    expected(0, 1)[0] = 1;

    BOOST_TEST_EQ(matrix, expected);
}

void rgb8_to_matrix_uniform()
{
    gil::rgb8_pixel_t default_pixel(1, 2, 3);
    gil::rgb8_image_t input(16, 16, default_pixel);

    blaze::StaticVector<std::uint8_t, 3> default_vector({1, 2, 3});
    blaze::DynamicMatrix<blaze::StaticVector<std::uint8_t, 3>> expected(16, 16, default_vector);

    auto result = gil::to_matrix_channeled(gil::view(input));
    BOOST_TEST_EQ(result, expected);
}

void rgb8_to_matrix_distinct()
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
    BOOST_TEST_EQ(result, expected);
}

int main()
{
    gray8_to_matrix_uniform();
    gray8_to_matrix_distinct();
    rgb8_to_matrix_uniform();
    rgb8_to_matrix_distinct();


    return boost::report_errors();
}
