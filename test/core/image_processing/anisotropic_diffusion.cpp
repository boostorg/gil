//
// Copyright 2020 Olzhas Zhumabek <anonymous.from.applecity@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/gil/color_base_algorithm.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_processing/diffusion.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/gil/typedefs.hpp>

#include <boost/core/lightweight_test.hpp>

#include <limits>
#include <random>

namespace gil = boost::gil;

template <typename PixelType, typename DiffusionFunction>
void diffusion_function_check(DiffusionFunction diffusion)
{
    using limits = std::numeric_limits<gil::uint8_t>;
    using channel_type = typename gil::channel_type<PixelType>::type;
    for (channel_type value = limits::min(); value <= limits::max(); ++value)
    {
        PixelType pixel;
        gil::static_fill(pixel, value);
        auto diffusivity_value = diffusion(pixel);
        gil::static_for_each(diffusivity_value,
                             [](channel_type value) { BOOST_TEST(0 <= value && value <= 1.0); });
    }
}

void brightness_function_test()
{
    std::vector<gil::rgb32f_pixel_t> rgb_pixels{
        gil::rgb32f_pixel_t(0, 11, 14),     gil::rgb32f_pixel_t(2, 117, 200),
        gil::rgb32f_pixel_t(223, 2, 180),   gil::rgb32f_pixel_t(250, 254, 100),
        gil::rgb32f_pixel_t(255, 255, 255),
    };
    for (const auto& pixel : rgb_pixels)
    {
        boost::gil::laplace_function::stencil_type<gil::rgb32f_pixel_t> stencil;
        std::fill(stencil.begin(), stencil.end(), pixel);
        auto brightness_stencil = boost::gil::brightness_function::identity{}(stencil);
        for (const auto& brightness_pixel : brightness_stencil)
        {
            BOOST_TEST(pixel == brightness_pixel);
        }
    }
}

template <typename ImageType, typename OutputImageType>
void heat_conservation_test(std::uint32_t seed)
{
    std::mt19937 twister(seed);
    std::uniform_int_distribution<gil::uint16_t> dist(0, std::numeric_limits<gil::uint8_t>::max());

    ImageType image(32, 32);
    auto view = gil::view(image);
    constexpr std::ptrdiff_t num_channels = gil::num_channels<decltype(view)>::value;
    double before_diffusion[num_channels] = {0};
    for (auto& pixel : view)
    {
        for (std::size_t channel_index = 0; channel_index < num_channels; ++channel_index)
        {
            pixel[channel_index] = static_cast<gil::uint8_t>(dist(twister));
            before_diffusion[channel_index] += pixel[channel_index];
        }
    }

    OutputImageType output(32, 32);
    auto output_view = gil::view(output);
    gil::default_anisotropic_diffusion(view, output_view, 10, 5);
    double after_diffusion[num_channels] = {0};
    for (const auto& pixel : output_view)
    {
        for (std::size_t channel_index = 0; channel_index < num_channels; ++channel_index)
        {
            after_diffusion[channel_index] += pixel[channel_index];
        }
    }

    for (std::ptrdiff_t channel_index = 0; channel_index < num_channels; ++channel_index)
    {
        const auto percentage =
            before_diffusion[channel_index] != 0
                ? std::abs(after_diffusion[channel_index] - before_diffusion[channel_index]) /
                      after_diffusion[channel_index] * 100.0
                : std::abs(after_diffusion[channel_index] - before_diffusion[channel_index]) /
                      after_diffusion[channel_index] * 100.0;
#ifdef BOOST_GIL_TEST_DEBUG
        std::cout << percentage << ' ';
#endif
        BOOST_TEST(percentage < 1);
    }
#ifdef BOOST_GIL_TEST_DEBUG
    std::cout << '\n';
#endif
}

int main()
{
    for (std::uint32_t seed = 0; seed < 1000; ++seed)
    {
#ifdef BOOST_GIL_TEST_DEBUG
        std::cout << "seed: " << seed << '\n';
        std::cout << "conservation of heat test:\n"
                  << "gray8 test:\n";
#endif
        heat_conservation_test<gil::gray8_image_t, gil::gray32f_image_t>(seed);
#ifdef BOOST_GIL_TEST_DEBUG
        std::cout << "rgb8 test:\n";
#endif
        heat_conservation_test<gil::rgb8_image_t, gil::rgb32f_image_t>(seed);
    }

    for (double kappa = 5; kappa <= 70; ++kappa)
    {
        diffusion_function_check<gil::rgb32f_pixel_t>(
            gil::diffusion::perona_malik_diffusivity{kappa});
        diffusion_function_check<gil::rgb32f_pixel_t>(gil::diffusion::gaussian_diffusivity{kappa});
        diffusion_function_check<gil::rgb32f_pixel_t>(
            gil::diffusion::wide_regions_diffusivity{kappa});
        diffusion_function_check<gil::rgb32f_pixel_t>(
            gil::diffusion::more_wide_regions_diffusivity{kappa});

        diffusion_function_check<gil::gray32f_pixel_t>(
            gil::diffusion::perona_malik_diffusivity{kappa});
        diffusion_function_check<gil::gray32f_pixel_t>(gil::diffusion::gaussian_diffusivity{kappa});
        diffusion_function_check<gil::gray32f_pixel_t>(
            gil::diffusion::wide_regions_diffusivity{kappa});
        diffusion_function_check<gil::gray32f_pixel_t>(
            gil::diffusion::more_wide_regions_diffusivity{kappa});
    }

    brightness_function_test();

    return boost::report_errors();
}
