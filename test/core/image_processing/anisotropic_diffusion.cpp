//
// Copyright 2020 Olzhas Zhumabek <anonymous.from.applecity@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/gil/image.hpp>
#include <boost/gil/image_processing/diffusion.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/gil/typedefs.hpp>

#include <boost/core/lightweight_test.hpp>

#include <limits>
#include <random>

namespace gil = boost::gil;

template <typename ImageType>
void heat_conservation_test(std::uint32_t seed)
{
    std::mt19937 twister(seed);
    std::uniform_int_distribution<gil::uint8_t> dist;

    ImageType image(32, 32);
    auto view = gil::view(image);
    constexpr std::ptrdiff_t num_channels = gil::num_channels<decltype(view)>::value;
    double before_diffusion[num_channels] = {0};
    for (auto &pixel : view)
    {
        for (std::ptrdiff_t channel_index = 0; channel_index < num_channels; ++channel_index)
        {
            pixel[channel_index] = dist(twister);
            before_diffusion[channel_index] += pixel[channel_index];
        }
    }

    gil::gray32f_image_t output(32, 32);
    auto output_view = gil::view(output);
    gil::anisotropic_diffusion(view, 10, 1 / 7.0, 30, output_view);
    double after_diffusion[num_channels] = {0};
    for (const auto &pixel : output_view)
    {
        for (std::ptrdiff_t channel_index = 0; channel_index < num_channels; ++channel_index)
        {
            after_diffusion[channel_index] += pixel[channel_index];
        }
    }

    for (std::ptrdiff_t channel_index = 0; channel_index < num_channels; ++channel_index)
    {
        const auto percentage =
            std::abs(after_diffusion[channel_index] - before_diffusion[channel_index]) /
            after_diffusion[channel_index] * 100.0;
        std::cout << percentage << ' ';
        BOOST_TEST(percentage < 10.0);
    }
    std::cout << '\n';
}

template <typename ImageType>
void convergence_to_mean_test()
{
}

int main()
{
    for (std::uint32_t seed = 0; seed < 1000; ++seed)
    {
        std::cout << "seed: " << seed << '\n';
        std::cout << "gray8 test:\n";
        heat_conservation_test<gil::gray8_image_t>(seed);
        std::cout << "rgb8 test:\n";
        heat_conservation_test<gil::rgb8_image_t>(seed);
    }

    return boost::report_errors();
}
