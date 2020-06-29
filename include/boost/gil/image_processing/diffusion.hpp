//
// Copyright 2020 Olzhas Zhumabek <anonymous.from.applecity@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/gil/algorithm.hpp>
#include <boost/gil/color_base_algorithm.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/gil/point.hpp>
#include <boost/gil/typedefs.hpp>
#include <functional>
#include <numeric>
#include <vector>

namespace boost { namespace gil {

template <typename PixelType>
struct diffusion_strategy
{
    using channel_type = typename channel_type<PixelType>::type;
    channel_type kappa;
    channel_type delta_t;
    template <typename View>
    PixelType operator()(View padded_view, point_t origin)
    {
        // seems like lambda capture doesn't work for member variables in C++11
        const auto local_delta_t = delta_t;
        const auto local_kappa = kappa;

        const auto minus = [](const PixelType& lhs, const PixelType& rhs) {
            PixelType result_pixel;
            static_transform(lhs, rhs, result_pixel, std::minus<channel_type>{});

            return result_pixel;
        };
        const auto multiply = [](const PixelType& lhs, const PixelType& rhs) {
            PixelType result_pixel;
            static_transform(lhs, rhs, result_pixel, std::multiplies<channel_type>{});
            return result_pixel;
        };
        const auto plus = [](const PixelType& lhs, const PixelType& rhs) {
            PixelType result_pixel;
            static_transform(lhs, rhs, result_pixel,
                             [](channel_type ilhs, channel_type irhs) { return (ilhs + irhs); });
            return result_pixel;
        };

        const auto g = [local_kappa](const PixelType& input_pixel) {
            PixelType result_pixel(input_pixel);
            channel_type sum = 0;
            static_for_each(result_pixel,
                            [&sum](channel_type channel_value) { sum += std::abs(channel_value); });
            channel_type sigma = local_kappa * num_channels<PixelType>{} * channel_type(255);
            channel_type exponent = (sum * sum) / (sigma * sigma);
            static_fill(result_pixel, std::exp(exponent));
            // static_transform(scratch_pixel, result_pixel, [local_kappa](channel_type value) {
            //     value /= local_kappa * num_channels<PixelType>{} * channel_type(255);
            //     return std::exp(-value * value);
            // });

            return result_pixel;
        };
        const auto x = origin.x;
        const auto y = origin.y;
        const auto current = padded_view(x, y);
        constexpr std::size_t point_count = 8;
        std::array<PixelType, point_count> nabla{
            minus(padded_view(x, y - 1), current),     // north
            minus(padded_view(x, y + 1), current),     // south
            minus(padded_view(x - 1, y), current),     // west
            minus(padded_view(x + 1, y), current),     // east
            minus(padded_view(x - 1, y - 1), current), // north west
            minus(padded_view(x + 1, y - 1), current), // north east
            minus(padded_view(x - 1, y + 1), current), // south west
            minus(padded_view(x + 1, y + 1), current)  // south east
        };
        std::array<PixelType, point_count> diffusivity;
        std::transform(nabla.begin(), nabla.end(), diffusivity.begin(), g);
        std::array<PixelType, point_count> product;
        std::transform(nabla.begin(), nabla.end(), diffusivity.begin(), product.begin(), multiply);

        // for (std::size_t i = 4; i < point_count; ++i)
        // {
        //     static_transform(product[i], product[i], [](channel_type value) { return value / 2;
        //     });
        // }
        const auto zero_pixel = []() {
            PixelType pixel;
            static_fill(pixel, channel_type(0));

            return pixel;
        }();
        auto sum = std::accumulate(product.begin(), product.end(), zero_pixel, plus);
        static_transform(sum, sum,
                         [local_delta_t](channel_type value) { return value * local_delta_t; });
        PixelType result_pixel;
        static_transform(padded_view(x, y), sum, result_pixel, std::plus<channel_type>{});

        return result_pixel;
    }
};

/// \brief Performs diffusion according to Perona-Malik equation
///
/// WARNING: Output channel type must be floating point,
/// otherwise there will be loss in accuracy which most
/// probably will lead to incorrect results (input will be unchanged).
/// Anisotropic diffusion is a smoothing algorithm that respects
/// edge boundaries and can work as an edge detector if suitable
/// iteration count is set and grayscale image view is used
/// as an input
template <typename InputView, typename OutputView,
          typename ComputationStrategy = diffusion_strategy<typename OutputView::value_type>>
void anisotropic_diffusion(const InputView& input, const OutputView& output, unsigned int num_iter,
                           ComputationStrategy strategy)
{
    using input_pixel_type = typename InputView::value_type;
    using pixel_type = typename OutputView::value_type;
    using computation_image = image<pixel_type>;
    const auto width = input.width();
    const auto height = input.height();
    const point_t dims(width, height);
    const auto zero_pixel = []() {
        pixel_type pixel;
        for (std::size_t i = 0; i < num_channels<pixel_type>{}; ++i)
        {
            pixel[i] = 0;
        }

        return pixel;
    }();
    computation_image result_image(width + 2, height + 2, zero_pixel);
    auto result = view(result_image);
    computation_image scratch_result_image(width + 2, height + 2, zero_pixel);
    auto scratch_result = view(scratch_result_image);
    transform_pixels(input, subimage_view(result, 1, 1, width, height),
                     [](const input_pixel_type& pixel) {
                         pixel_type converted;
                         for (std::size_t i = 0; i < num_channels<pixel_type>{}; ++i)
                         {
                             converted[i] = pixel[i];
                         }
                         return converted;
                     });

    for (unsigned int iteration = 0; iteration < num_iter; ++iteration)
    {
        for (std::ptrdiff_t relative_y = 0; relative_y < height; ++relative_y)
        {
            for (std::ptrdiff_t relative_x = 0; relative_x < width; ++relative_x)
            {
                auto x = relative_x + 1;
                auto y = relative_y + 1;
                scratch_result(x, y) = strategy(result, {x, y});
            }
        }
        using std::swap;
        swap(result, scratch_result);
    }

    copy_pixels(subimage_view(result, 1, 1, width, height), output);
}

}} // namespace boost::gil
