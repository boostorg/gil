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
/// \brief Performs diffusion according to Perona-Malik equation
///
/// WARNING: Output channel type must be floating point,
/// otherwise there will be loss in accuracy which most
/// probably will lead to incorrect results (input will be unchanged).
/// Anisotropic diffusion is a smoothing algorithm that respects
/// edge boundaries and can work as an edge detector if suitable
/// iteration count is set and grayscale image view is used
/// as an input
template <typename InputView, typename OutputView>
void anisotropic_diffusion(const InputView& input, const OutputView& output, unsigned int num_iter,
                           typename channel_type<OutputView>::type delta_t,
                           typename channel_type<OutputView>::type kappa)
{
    using input_pixel_type = typename InputView::value_type;
    using pixel_type = typename OutputView::value_type;
    using computation_image = image<pixel_type>;
    using channel_type = typename channel_type<pixel_type>::type;
    const auto zero_pixel = []() {
        pixel_type pixel;
        for (std::size_t i = 0; i < num_channels<pixel_type>{}; ++i)
        {
            pixel[i] = 0;
        }

        return pixel;
    }();
    const auto width = input.width();
    const auto height = input.height();
    const point_t dims(width, height);
    computation_image result_image(width + 2, height + 2, zero_pixel);
    auto result = view(result_image);
    computation_image scratch_result_image(width + 2, height + 2, zero_pixel);
    auto scratch_result = view(scratch_result_image);
    transform_pixels(input, subimage_view(result, 1, 1, width, height), [](const input_pixel_type& pixel) {
        pixel_type converted;
        for (std::size_t i = 0; i < num_channels<pixel_type>{}; ++i)
        {
            converted[i] = pixel[i];
        }
        return converted;
    });

    const auto minus = [](const pixel_type& lhs, const pixel_type& rhs) {
        pixel_type result_pixel;
        static_transform(lhs, rhs, result_pixel, std::minus<channel_type>{});
        return result_pixel;
    };
    const auto multiply = [](const pixel_type& lhs, const pixel_type& rhs) {
        pixel_type result_pixel;
        static_transform(lhs, rhs, result_pixel, std::multiplies<channel_type>{});
        return result_pixel;
    };
    const auto plus_delta_t = [delta_t](const pixel_type& lhs, const pixel_type& rhs) {
        pixel_type result_pixel;
        static_transform(lhs, rhs, result_pixel, [delta_t](channel_type ilhs, channel_type irhs) {
            return (ilhs + irhs) * delta_t;
        });
        return result_pixel;
    };

    const auto g = [kappa](const pixel_type& input_pixel) {
        pixel_type result_pixel(input_pixel);
        static_transform(input_pixel, result_pixel, [kappa](channel_type value) {
            value /= kappa;
            return std::exp(-value * value);
        });

        return result_pixel;
    };

    for (unsigned int iteration = 0; iteration < num_iter; ++iteration)
    {
        for (std::ptrdiff_t relative_y = 0; relative_y < height; ++relative_y)
        {
            for (std::ptrdiff_t relative_x = 0; relative_x < width; ++relative_x)
            {
                const auto x = relative_x + 1;
                const auto y = relative_y + 1;
                const auto current = result(x, y);
                std::array<pixel_type, 4> nabla{
                    minus(result(x, y - 1), current), // north
                    minus(result(x, y + 1), current), // south
                    minus(result(x - 1, y), current), // west
                    minus(result(x + 1, y), current)  // east
                };
                std::array<pixel_type, 4> diffusivity{g(nabla[0]), g(nabla[1]), g(nabla[2]),
                                                      g(nabla[3])};
                std::array<pixel_type, 4> product;
                std::transform(nabla.begin(), nabla.end(), diffusivity.begin(), product.begin(),
                               multiply);
                auto sum =
                    std::accumulate(product.begin(), product.end(), zero_pixel, plus_delta_t);
                static_transform(result(x, y), sum, scratch_result(x, y), std::plus<channel_type>{});
            }
        }
        std::swap(result, scratch_result);
    }

    copy_pixels(subimage_view(result, 1, 1, width, height), output);
}

}} // namespace boost::gil
