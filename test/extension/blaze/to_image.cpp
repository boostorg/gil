#include <boost/core/lightweight_test.hpp>

#include <boost/gil/algorithm.hpp>
#include <boost/gil/extension/blaze/blaze.hpp>

namespace gil = boost::gil;

void matrix_to_gray8_uniform()
{
    gil::gray8_image_t expected(16, 16, gil::gray8_pixel_t(13));
    blaze::DynamicMatrix<blaze::StaticVector<std::uint8_t, 1>> input(16, 16, {13});
    auto result = gil::to_image<gil::gray8_image_t>(input);

    BOOST_TEST(gil::equal_pixels(gil::view(result), gil::view(expected)));
}

void matrix_to_gray8_distinct()
{
    blaze::DynamicMatrix<blaze::StaticVector<std::uint8_t, 1>> input(16, 16, {13});
    gil::gray8_image_t expected(16, 16, gil::gray8_pixel_t(13));
    auto view = gil::view(expected);
    input(0, 0)[0] = 0;
    input(1, 0)[0] = 1; // rows and cols are different for GIL vs Blaze

    auto result = gil::to_image<gil::gray8_image_t>(input);
    view(0, 0)[0] = 0;
    view(0, 1)[0] = 1;

    BOOST_TEST(gil::equal_pixels(gil::view(result), gil::view(expected)));
}

void matrix_to_rgb8_uniform()
{
    gil::rgb8_pixel_t default_pixel(1, 2, 3);
    gil::rgb8_image_t expected(16, 16, default_pixel);

    blaze::StaticVector<std::uint8_t, 3> default_vector({1, 2, 3});
    blaze::DynamicMatrix<blaze::StaticVector<std::uint8_t, 3>> input(16, 16, default_vector);

    auto result = gil::to_image<gil::rgb8_image_t>(input);
    BOOST_TEST(gil::equal_pixels(gil::view(result), gil::view(expected)));
}

void matrix_to_rgb8_distinct()
{
    gil::rgb8_pixel_t default_pixel(1, 2, 3);
    gil::rgb8_image_t expected(16, 16, default_pixel);
    auto view = gil::view(expected);
    view(0, 0) = gil::rgb8_pixel_t(10, 20, 30);
    view(1, 0) = gil::rgb8_pixel_t(50, 50, 50);

    blaze::StaticVector<std::uint8_t, 3> default_vector({1, 2, 3});
    blaze::DynamicMatrix<blaze::StaticVector<std::uint8_t, 3>> input(16, 16, default_vector);
    input(0, 0) = {10, 20, 30};
    input(0, 1) = {50, 50, 50};

    auto result = gil::to_image<gil::rgb8_image_t>(input);
    BOOST_TEST(gil::equal_pixels(gil::view(result), gil::view(expected)));
}

int main()
{
    matrix_to_gray8_uniform();
    matrix_to_gray8_distinct();
    matrix_to_rgb8_uniform();
    matrix_to_rgb8_distinct();

    return boost::report_errors();
}
