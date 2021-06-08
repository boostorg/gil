#include <boost/core/lightweight_test.hpp>
#include <boost/core/lightweight_test_trait.hpp>

#include <boost/gil/extension/blaze/blaze.hpp>

namespace gil = boost::gil;

gil::gray8_pixel_t zero_gray8_pixel(0);

void as_matrix_typecheck_for_gray8_image()
{
    gil::gray8_image_t image(16, 16, zero_gray8_pixel);
    auto view = gil::view(image);

    auto result = gil::as_matrix(view);
    using expected_t = blaze::CustomMatrix<gil::uint8_t, blaze::unaligned, blaze::unpadded, blaze::rowMajor>;
    BOOST_TEST_TRAIT_TRUE((std::is_same<decltype(result), expected_t>));
}

void as_matrix_gray8_image_zero_value_check()
{
    gil::gray8_image_t image(16, 16, zero_gray8_pixel);
    auto view = gil::view(image);

    auto result = gil::as_matrix(view);
    BOOST_TEST(blaze::isZero(result));    
}

void as_matrix_gray8_image_modify_matrix()
{
    gil::gray8_image_t image(16, 16, zero_gray8_pixel);
    auto view = gil::view(image);

    auto result = gil::as_matrix(view);
    gil::gray8_image_t image2(16, 16, zero_gray8_pixel);
    auto expected_before_modify = gil::view(image2);
    BOOST_TEST(gil::equal_pixels(view, expected_before_modify));

    gil::uint8_t value = 23;
    result(1, 0) = value; // do not forget Blaze's different indexing
    BOOST_TEST(view(0, 1)[0] == value);

    std::uint8_t value2 = 40;
    result(3, 2) = value2;
    BOOST_TEST(view(2, 3)[0] == value2);
}

void as_matrix_gray8_image_modify_image()
{
    gil::gray8_image_t image(16, 16, zero_gray8_pixel);
    auto view = gil::view(image);

    auto result = gil::as_matrix(view);
    auto expected_before_modify = blaze::zero<gil::uint8_t>(16, 16);
    BOOST_TEST(result == expected_before_modify);
    gil::uint8_t value = 23;
    view(1, 0)[0] = value;
    BOOST_TEST(result(0, 1) == value);

    std::uint8_t value2 = 40;
    view(3, 2)[0] = value2;
    BOOST_TEST(result(2, 3) == value2);
}

void as_matrix_typecheck_for_gray16_image()
{
    gil::gray16_image_t image(16, 16);
    auto view = gil::view(image);

    auto result = gil::as_matrix(view);
    using expected_t = blaze::CustomMatrix<gil::uint16_t, blaze::unaligned, blaze::unpadded, blaze::rowMajor>;
    BOOST_TEST_TRAIT_TRUE((std::is_same<decltype(result), expected_t>));    
}

gil::gray16_pixel_t zero_gray16_pixel(0);

void as_matrix_gray16_image_zero_value_check()
{
    gil::gray16_image_t image(16, 16, zero_gray16_pixel);
    auto view = gil::view(image);

    auto result = gil::as_matrix(view);
    BOOST_TEST(blaze::isZero(result));    
}

void as_matrix_gray16_image_modify_matrix()
{
    gil::gray16_image_t image(16, 16, zero_gray16_pixel);
    auto view = gil::view(image);

    auto result = gil::as_matrix(view);
    gil::gray16_image_t image2(16, 16, zero_gray16_pixel);
    auto expected_before_modify = gil::view(image2);
    BOOST_TEST(gil::equal_pixels(view, expected_before_modify));

    gil::uint16_t value = 23;
    result(1, 0) = value; // do not forget Blaze's different indexing
    BOOST_TEST(view(0, 1)[0] == value);

    std::uint16_t value2 = 40;
    result(3, 2) = value2;
    BOOST_TEST(view(2, 3)[0] == value2);
}

void as_matrix_gray16_image_modify_image()
{
    gil::gray16_image_t image(16, 16, zero_gray16_pixel);
    auto view = gil::view(image);

    auto result = gil::as_matrix(view);
    auto expected_before_modify = blaze::zero<gil::uint16_t>(16, 16);
    BOOST_TEST(result == expected_before_modify);
    gil::uint16_t value = 23;
    view(1, 0)[0] = value;
    BOOST_TEST(result(0, 1) == value);

    std::uint16_t value2 = 40;
    view(3, 2)[0] = value2;
    BOOST_TEST(result(2, 3) == value2);
}

int main()
{
    as_matrix_typecheck_for_gray8_image();
    as_matrix_gray8_image_zero_value_check();
    as_matrix_gray8_image_modify_matrix();
    as_matrix_gray8_image_modify_image();
    as_matrix_typecheck_for_gray16_image();
    as_matrix_gray16_image_zero_value_check();
    as_matrix_gray16_image_modify_matrix();
    as_matrix_gray16_image_modify_image();

    return boost::report_errors();
}