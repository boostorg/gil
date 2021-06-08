#include <boost/core/lightweight_test.hpp>
#include <boost/core/lightweight_test_trait.hpp>

#include <boost/gil/algorithm.hpp>
#include <boost/gil/extension/blaze/blaze.hpp>
#include <type_traits>

namespace gil = boost::gil;

template <typename PixelType>
PixelType create_pixel(std::uint8_t value)
{
    PixelType pixel;
    auto num_channels = gil::num_channels<PixelType>::value;
    for (std::ptrdiff_t i = 0; i < num_channels; ++i) {
        pixel[i] = value;
    }

    return pixel;
}

template <typename PixelType>
PixelType create_zero_pixel()
{
    return create_pixel<PixelType>(0);
}

template <typename VectorType, typename ImageType,
          typename PixelType = typename ImageType::value_type>
void test_vector_matrix_type()
{
    blaze::DynamicMatrix<VectorType> input(16, 16, VectorType({0}));
    auto result = gil::from_matrix<ImageType>(input);
    using result_type = decltype(result);
    BOOST_TEST_TRAIT_TRUE(( std::is_same<result_type, ImageType> ));

    ImageType expected(16, 16, create_zero_pixel<PixelType>());

    BOOST_TEST(result == expected);

    std::uint8_t value = 23;
    constexpr auto num_channels = gil::num_channels<PixelType>{};
    for (std::ptrdiff_t i = 0; i < num_channels; ++i) {
        auto pixel = create_zero_pixel<PixelType>();
        pixel[i] = value;
        ImageType expected(16, 16, pixel);

        auto vector = VectorType{0};
        vector[i] = value;
        blaze::DynamicMatrix<VectorType> input(16, 16, vector);

        auto result = gil::from_matrix<ImageType, PixelType>(input);
        BOOST_TEST(result == expected);
    }

    // different scope
    {
        PixelType pixel;
        VectorType vector;
        auto num_channels = gil::num_channels<PixelType>::value;
        for (std::ptrdiff_t i = 0; i < num_channels; ++i) {
            pixel[i] = i;
            vector[static_cast<std::size_t>(i)] = i;
        }

        ImageType expected(16, 16, pixel);
        blaze::DynamicMatrix<VectorType> input(16, 16, vector);

        auto result = gil::from_matrix<ImageType, PixelType>(input);
        BOOST_TEST(result == expected);
    }
}

template <typename ScalarType, typename ImageType,
          typename PixelType = typename ImageType::value_type>
void test_scalar_matrix_type()
{
    blaze::DynamicMatrix<ScalarType> input(16, 16, 0);
    ImageType expected(16, 16, create_zero_pixel<PixelType>());

    auto result = gil::from_matrix<ImageType, PixelType>(input);
    using result_type = decltype(result);
    BOOST_TEST_TRAIT_TRUE((std::is_same<result_type, ImageType>));
    BOOST_TEST(result == expected);

    // same but non-zero value
    ScalarType value = 23;
    input = value;
    gil::fill_pixels(gil::view(expected), PixelType(value));
    auto uniform_test_result = gil::from_matrix<ImageType, PixelType>(input);
    BOOST_TEST(uniform_test_result == expected);

    auto view = gil::view(expected);

    input(0, 0) = 0;
    input(0, 10) = 0;
    view(0, 0)[0] = 0;
    view(10, 0)[0] = 0;

    auto nonuniform_test_result = gil::from_matrix<ImageType, PixelType>(input);
    BOOST_TEST(nonuniform_test_result == expected);
}

void matrix_to_rgb8_image()
{
    using image_type = gil::rgb8_image_t;
    using vector_type = blaze::StaticVector<gil::uint8_t, 3>;
    test_vector_matrix_type<vector_type, image_type>();
}

void matrix_to_rgb32f_image()
{
    using image_type = gil::rgb32f_image_t;
    using vector_type = blaze::StaticVector<float, 3>;
    test_vector_matrix_type<vector_type, image_type>();
}

void matrix_to_rgba32f_image()
{
    using image_type = gil::rgba32f_image_t;
    using vector_type = blaze::StaticVector<float, 4>;
    test_vector_matrix_type<vector_type, image_type>();
}

void matrix_to_rgba8_image()
{
    using image_type = gil::rgba8_image_t;
    using vector_type = blaze::StaticVector<gil::uint8_t, 4>;
    test_vector_matrix_type<vector_type, image_type>();
}

void matrix_to_gray8_image()
{
    using image_type = gil::gray8_image_t;
    using vector_type = blaze::StaticVector<gil::uint8_t, 1>;
    test_vector_matrix_type<vector_type, image_type>();
}

void matrix_to_gray16_image()
{
    using image_type = gil::gray8_image_t;
    using vector_type = blaze::StaticVector<gil::uint16_t, 1>;
    test_vector_matrix_type<vector_type, image_type>();
}

void matrix_to_gray32f_image()
{
    using image_type = gil::gray32f_image_t;
    using vector_type = blaze::StaticVector<float, 1>;
    test_vector_matrix_type<vector_type, image_type>();
}

void scalar_matrix_to_gray8_image()
{
    using image_type = gil::gray8_image_t;
    using scalar_type = std::uint8_t;
    test_scalar_matrix_type<scalar_type, image_type>();    
}

void scalar_matrix_to_gray16_image()
{
    using image_type = gil::gray16_image_t;
    using scalar_type = std::uint16_t;
    test_scalar_matrix_type<scalar_type, image_type>();    
}

void scalar_matrix_to_gray32f_image()
{
    using image_type = gil::gray32f_image_t;
    using scalar_type = float;
    test_scalar_matrix_type<scalar_type, image_type>();    
}

int main() {
    matrix_to_rgb8_image();
    matrix_to_rgb32f_image();
    matrix_to_rgba32f_image();
    matrix_to_rgba8_image();
    matrix_to_gray8_image();
    matrix_to_gray16_image();
    matrix_to_gray32f_image();
    scalar_matrix_to_gray8_image();
    scalar_matrix_to_gray16_image();
    scalar_matrix_to_gray32f_image();

    return boost::report_errors();
}