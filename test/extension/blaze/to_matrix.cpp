#include <boost/core/lightweight_test.hpp>

#include <boost/gil/algorithm.hpp>
#include <boost/gil/extension/blaze/blaze.hpp>

namespace gil = boost::gil;

template <typename ImageType>
void test_to_matrix_type()
{
    using pixel_type = typename ImageType::value_type;
    using channel_type = typename gil::channel_type<pixel_type>::type;
    constexpr auto num_channels = gil::num_channels<ImageType>{};

    pixel_type zero_pixel{};
    for (std::size_t i = 0; i < num_channels; ++i) {
        zero_pixel[i] = 0;
    }

    ImageType zero_image(16, 16, zero_pixel);
    auto view = gil::view(zero_image);

    for (std::size_t i = 0; i < num_channels; ++i) {
        auto result = gil::to_matrix(view, i);
        BOOST_TEST(blaze::isZero(result));
    }

    gil::transform_pixels(view, view, [num_channels](auto) {
        pixel_type pixel;
        for (std::size_t i = 0; i < num_channels; ++i) {
            pixel[i] = i;
            if constexpr (std::is_same_v<channel_type, gil::float32_t>) {
                pixel[i] /= num_channels;
            }
        }
        return pixel;
    });

    for (std::size_t channel_index; channel_index < num_channels; ++channel_index) {
        auto result = gil::to_matrix(view, channel_index);
        for (std::size_t i = 0; i < view.height(); ++i) {
            for (std::size_t j = 0; j < view.width(); ++j) {
                BOOST_TEST(result(j, i) == view(i, j)[channel_index]);
            }
        }
    }

    BOOST_TEST_THROWS(gil::to_matrix(view, num_channels), std::invalid_argument);
    BOOST_TEST_THROWS(gil::to_matrix(view, num_channels + 1), std::invalid_argument);
}

template <typename ImageType,
          typename OutputType = typename gil::channel_type<typename ImageType::value_type>::type>
void test_to_matrix_out()
{
    using pixel_type = typename ImageType::value_type;
    using channel_type = typename gil::channel_type<pixel_type>::type;
    constexpr auto num_channels = gil::num_channels<ImageType>{};

    pixel_type zero_pixel{};
    for (std::size_t i = 0; i < num_channels; ++i) {
        zero_pixel[i] = 0;
    }

    ImageType zero_image(16, 16, zero_pixel);
    auto view = gil::view(zero_image);

    for (std::size_t i = 0; i < num_channels; ++i) {
        blaze::DynamicMatrix<OutputType> result(view.height(), view.width());
        gil::to_matrix(view, result, i);
        BOOST_TEST(blaze::isZero(result));
    }

    gil::transform_pixels(view, view, [num_channels](auto) {
        pixel_type pixel;
        for (std::size_t i = 0; i < num_channels; ++i) {
            pixel[i] = i;
            if constexpr (std::is_same_v<channel_type, gil::float32_t>) {
                pixel[i] /= num_channels;
            }
        }
        return pixel;
    });

    for (std::size_t channel_index; channel_index < num_channels; ++channel_index) {
        blaze::DynamicMatrix<OutputType> result(view.height(), view.width());
        gil::to_matrix(view, result, channel_index);
        for (std::size_t i = 0; i < view.height(); ++i) {
            for (std::size_t j = 0; j < view.width(); ++j) {
                BOOST_TEST(result(j, i) == view(i, j)[channel_index]);
            }
        }
    }

    blaze::DynamicMatrix<OutputType> result(view.height(), view.width());
    BOOST_TEST_THROWS(gil::to_matrix(view, result, num_channels), std::invalid_argument);
    BOOST_TEST_THROWS(gil::to_matrix(view, result, num_channels + 1), std::invalid_argument);
}

void gray8_to_matrix()
{
    using image_type = gil::gray8_image_t;
    test_to_matrix_type<image_type>();
    test_to_matrix_out<image_type>();
}

void rgb8_to_matrix()
{
    using image_type = gil::rgb8_image_t;
    test_to_matrix_type<image_type>();
    test_to_matrix_out<image_type>();
}

void rgb32f_to_matrix()
{
    using image_type = gil::rgb32f_image_t;
    test_to_matrix_type<image_type>();
    test_to_matrix_out<image_type, float>();
}

void rgb16_to_matrix()
{
    using image_type = gil::rgb16_image_t;
    test_to_matrix_type<image_type>();
    test_to_matrix_out<image_type>();
}

void rgb8s_to_matrix()
{
    using image_type = gil::rgb8s_image_t;
    test_to_matrix_type<image_type>();
    test_to_matrix_out<image_type>();
}

int main()
{
    gray8_to_matrix();
    rgb8_to_matrix();
    rgb32f_to_matrix();
    rgb16_to_matrix();
    rgb8s_to_matrix();

    return boost::report_errors();
}