#include <boost/gil/image_processing/numeric.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/core/lightweight_test.hpp>

namespace gil = boost::gil;

bool are_equal(gil::gray32f_view_t expected, gil::gray32f_view_t actual)
{
    if (expected.dimensions() != actual.dimensions())
        return false;

    for (long int y = 0; y < expected.height(); ++y)
    {
        for (long int x = 0; x < expected.width(); ++x)
        {
            if (expected(x, y) != actual(x, y))
            {
                return false;
            }
        }
    }

    return true;
}

void test_normalized_mean_generation()
{
    gil::gray32f_image_t kernel_image(gil::point_t(5, 5));
    auto view = gil::view(kernel_image);
    gil::generate_normalized_mean(view);
    bool is_correct = true;
    boost::gil::for_each_pixel(view, [&is_correct](gil::gray32f_pixel_t& pixel) {
        const auto chosen_channel = std::integral_constant<int, 0>{};
        const auto expected_value = static_cast<float>(1 / 25.f);
        if (pixel.at(chosen_channel) != expected_value)
            is_correct = false;
    });
    BOOST_TEST(is_correct);
}

void test_normalized_mean_throw()
{
    gil::gray32f_image_t kernel_image(gil::point_t(5, 6));
    auto view = gil::view(kernel_image);
    bool have_thrown = false;
    try
    {
        gil::generate_normalized_mean(view);
    } catch(std::invalid_argument& e)
    {
        have_thrown = true;
    }
    BOOST_TEST(have_thrown);
}

void test_unnormalized_mean_generation()
{
    gil::gray32f_image_t kernel_image(gil::point_t(5, 5));
    auto view = gil::view(kernel_image);
    gil::generate_unnormalized_mean(view);
    bool is_correct = true;
    boost::gil::for_each_pixel(view, [&is_correct](gil::gray32f_pixel_t& pixel)
    {
        const auto chosen_channel = std::integral_constant<int, 0>{};
        const auto expected_value = 1.f;
        if (pixel.at(chosen_channel) != expected_value)
            is_correct = false;
    });
    BOOST_TEST(is_correct);
}

void test_unnormalized_mean_throw() {
    gil::gray32f_image_t kernel_image(gil::point_t(5, 6));
    auto view = gil::view(kernel_image);
    bool have_thrown = false;
    try
    {
        gil::generate_unnormalized_mean(view);
    } catch(std::invalid_argument& e)
    {
        have_thrown = true;
    }
    BOOST_TEST(have_thrown);
}

#include <iostream>

void test_gaussian_kernel_generation()
{
    gil::gray32f_image_t kernel_image(gil::point_t(7, 7));
    auto view = gil::view(kernel_image);
    gil::generate_gaussian_kernel(view, 0.84089642);
    bool is_correct = true;
    const float expected_values[][7] = {
        0.00000067,	0.00002292,	0.00019117,	0.00038771,	0.00019117,	0.00002292,	0.00000067,
        0.00002292,	0.00078633,	0.00655965,	0.01330373,	0.00655965,	0.00078633,	0.00002292,
        0.00019117,	0.00655965,	0.05472157,	0.11098164,	0.05472157,	0.00655965,	0.00019117,
        0.00038771,	0.01330373,	0.11098164,	0.22508352,	0.11098164,	0.01330373,	0.00038771,
        0.00019117,	0.00655965,	0.05472157,	0.11098164,	0.05472157,	0.00655965,	0.00019117,
        0.00002292,	0.00078633,	0.00655965,	0.01330373,	0.00655965,	0.00078633,	0.00002292,
        0.00000067,	0.00002292,	0.00019117,	0.00038771,	0.00019117,	0.00002292,	0.00000067
    };

    const auto chosen_channel = std::integral_constant<int, 0>{};
    for (gil::gray32f_view_t::coord_t y = 0; y < view.height(); ++y)
        for (gil::gray32f_view_t::coord_t x = 0; x < view.height(); ++x) {
            std::cout << x << ' ' << y << ' ' << view(x, y).at(chosen_channel) << '\n';
            if (view(x, y).at(chosen_channel) != expected_values[y][x])
                is_correct = false;
        }



    BOOST_TEST(is_correct);
}

void test_gaussian_kernel_throw()
{
    gil::gray32f_image_t kernel_image(gil::point_t(5, 6));
    auto view = gil::view(kernel_image);
    bool have_thrown = false;
    try
    {
        gil::generate_gaussian_kernel(view, 0.5);
    } catch(std::invalid_argument& e)
    {
        have_thrown = true;
    }
    BOOST_TEST(have_thrown);
}

int main()
{
    test_normalized_mean_generation();
    test_normalized_mean_throw();
    test_unnormalized_mean_generation();
    test_unnormalized_mean_throw();
    test_gaussian_kernel_generation();
    test_gaussian_kernel_throw();
    return boost::report_errors();
}
