#include <boost/core/lightweight_test.hpp>
#include <boost/core/lightweight_test_trait.hpp>

#include <boost/gil/extension/blaze/blaze.hpp>

namespace gil = boost::gil;

template <typename PixelType, typename ImageType>
void pixel3_uint8_zeroes_test()
{
    PixelType zero_pixel = PixelType(0, 0, 0);
    ImageType image(16, 16, zero_pixel);
    auto view = gil::view(image);

    auto matrix_view = gil::as_matrix_channeled(view);
    BOOST_TEST(blaze::isZero(matrix_view));
}

template <typename PixelType, typename ImageType>
void pixel3_uint8_different_values_test()
{
    PixelType zero_pixel = PixelType(0, 0, 0);
    ImageType image(16, 16, zero_pixel);
    auto view = gil::view(image);

    auto matrix_view = gil::as_matrix_channeled(view);
    std::uint8_t value(255);
    for (std::size_t i = 0; i < matrix_view.rows(); ++i) {
        for (std::size_t j = 0; j < matrix_view.columns(); ++j) {
            matrix_view(i, j) = {
                static_cast<unsigned char>(i), static_cast<unsigned char>(j), value};
        }
    }

    for (std::ptrdiff_t i = 0; i < matrix_view.rows(); ++i) {
        for (std::ptrdiff_t j = 0; j < matrix_view.columns(); ++j) {
            PixelType expected_pixel(
                static_cast<unsigned char>(i), static_cast<unsigned char>(j), value);
            BOOST_TEST(view(j, i) == expected_pixel);
        }
    }
}

template <typename PixelType, typename ImageType>
void pixel4_uint8_zeroes_test()
{
    PixelType zero_pixel = PixelType(0, 0, 0, 0);
    ImageType image(16, 16, zero_pixel);
    auto view = gil::view(image);

    auto matrix_view = gil::as_matrix_channeled(view);
    BOOST_TEST(blaze::isZero(matrix_view));
}

template <typename PixelType, typename ImageType>
void pixel4_uint8_different_values_test()
{
    PixelType zero_pixel = PixelType(0, 0, 0, 0);
    ImageType image(16, 16, zero_pixel);
    auto view = gil::view(image);

    auto matrix_view = gil::as_matrix_channeled(view);
    std::uint8_t value(255);
    for (std::size_t i = 0; i < matrix_view.rows(); ++i) {
        for (std::size_t j = 0; j < matrix_view.columns(); ++j) {
            matrix_view(i, j) = {
                static_cast<unsigned char>(i), static_cast<unsigned char>(j), value, 0};
        }
    }

    for (std::ptrdiff_t i = 0; i < matrix_view.rows(); ++i) {
        for (std::ptrdiff_t j = 0; j < matrix_view.columns(); ++j) {
            PixelType expected_pixel(
                static_cast<unsigned char>(i), static_cast<unsigned char>(j), value, 0);
            BOOST_TEST(view(j, i) == expected_pixel);
        }
    }
}

template <typename PixelType, typename ImageType>
void pixel4_float32_zeroes_test()
{
    PixelType zero_pixel = PixelType(0, 0, 0, 0);
    ImageType image(16, 16, zero_pixel);
    auto view = gil::view(image);

    auto matrix_view = gil::as_matrix_channeled(view);
    BOOST_TEST(blaze::isZero(matrix_view));
}

template <typename PixelType, typename ImageType>
void pixel4_float32_different_values_test()
{
    PixelType zero_pixel = PixelType(0, 0, 0, 0);
    ImageType image(16, 16, zero_pixel);
    auto view = gil::view(image);

    auto matrix_view = gil::as_matrix_channeled(view);
    float value = 0.125;
    for (std::size_t i = 0; i < matrix_view.rows(); ++i) {
        for (std::size_t j = 0; j < matrix_view.columns(); ++j) {
            matrix_view(i, j) = {static_cast<float>(i), static_cast<float>(j), value, 0.0f};
        }
    }

    for (std::ptrdiff_t i = 0; i < matrix_view.rows(); ++i) {
        for (std::ptrdiff_t j = 0; j < matrix_view.columns(); ++j) {
            PixelType expected_pixel(static_cast<float>(i), static_cast<float>(j), value, 0.0f);
            BOOST_TEST(view(j, i) == expected_pixel);
        }
    }
}

template <typename PixelType, typename ImageType>
void pixel3_float32_zeroes_test()
{
    PixelType zero_pixel = PixelType(0, 0, 0);
    ImageType image(16, 16, zero_pixel);
    auto view = gil::view(image);

    auto matrix_view = gil::as_matrix_channeled(view);
    BOOST_TEST(blaze::isZero(matrix_view));
}

template <typename PixelType, typename ImageType>
void pixel3_float32_different_values_test()
{
    PixelType zero_pixel = PixelType(0, 0, 0);
    ImageType image(16, 16, zero_pixel);
    auto view = gil::view(image);

    auto matrix_view = gil::as_matrix_channeled(view);
    float value = 0.125;
    for (std::size_t i = 0; i < matrix_view.rows(); ++i) {
        for (std::size_t j = 0; j < matrix_view.columns(); ++j) {
            matrix_view(i, j) = {static_cast<float>(i), static_cast<float>(j), value};
        }
    }

    for (std::ptrdiff_t i = 0; i < matrix_view.rows(); ++i) {
        for (std::ptrdiff_t j = 0; j < matrix_view.columns(); ++j) {
            PixelType expected_pixel(static_cast<float>(i), static_cast<float>(j), value);
            BOOST_TEST(view(j, i) == expected_pixel);
        }
    }
}

void check_rgba8_as_matrix_channeled()
{
    using pixel_type = gil::rgba8_pixel_t;
    using image_type = gil::rgba8_image_t;
    pixel4_uint8_zeroes_test<pixel_type, image_type>();
    pixel4_uint8_different_values_test<pixel_type, image_type>();
}

void check_rgb8_as_matrix_channeled()
{
    using pixel_type = gil::rgb8_pixel_t;
    using image_type = gil::rgb8_image_t;
    pixel3_uint8_zeroes_test<pixel_type, image_type>();
    pixel3_uint8_different_values_test<pixel_type, image_type>();
}

void check_bgr8_as_matrix_channeled()
{
    using pixel_type = gil::bgr8_pixel_t;
    using image_type = gil::bgr8_image_t;
    pixel3_uint8_zeroes_test<pixel_type, image_type>();
    pixel3_uint8_different_values_test<pixel_type, image_type>();
}

void check_rgb32f_as_matrix_channeled()
{
    using pixel_type = gil::rgb32f_pixel_t;
    using image_type = gil::rgb32f_image_t;
    pixel3_float32_zeroes_test<pixel_type, image_type>();
    pixel3_float32_different_values_test<pixel_type, image_type>();
}

void check_rgba32f_as_matrix_channeled()
{
    using pixel_type = gil::rgba32f_pixel_t;
    using image_type = gil::rgba32f_image_t;
    pixel4_float32_zeroes_test<pixel_type, image_type>();
    pixel4_float32_different_values_test<pixel_type, image_type>();
}

void check_cmyk8_as_matrix_channeled()
{
    using pixel_type = gil::cmyk8_pixel_t;
    using image_type = gil::cmyk8_image_t;
    pixel4_uint8_zeroes_test<pixel_type, image_type>();
    pixel4_uint8_different_values_test<pixel_type, image_type>();
}

void check_cmyk32f_as_matrix_channeled()
{
    using pixel_type = gil::cmyk32f_pixel_t;
    using image_type = gil::cmyk32f_image_t;
    pixel4_float32_zeroes_test<pixel_type, image_type>();
    pixel4_float32_different_values_test<pixel_type, image_type>();
}

int main()
{
    check_rgba8_as_matrix_channeled();
    check_rgb8_as_matrix_channeled();   
    check_bgr8_as_matrix_channeled();
    check_rgb32f_as_matrix_channeled();
    check_rgba32f_as_matrix_channeled();
    check_cmyk8_as_matrix_channeled();
    check_cmyk32f_as_matrix_channeled();

    return boost::report_errors();
}