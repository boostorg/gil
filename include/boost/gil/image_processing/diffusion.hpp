//
// Copyright 2020 Olzhas Zhumabek <anonymous.from.applecity@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/gil/algorithm.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/gil/typedefs.hpp>

#include <vector>

namespace boost { namespace gil {
namespace detail {
namespace direction {
enum
{
    north = 0,
    south = 1,
    west = 2,
    east = 3,
    north_east = 4,
    south_east = 5,
    south_west = 6,
    north_west = 7
};
}

// nabla is directed difference in pixels,
// nabla north for example represents intensity
// change in the north direction (up), and nabla south
// represents intensity change in south direction (down).
template <typename InputView, typename OutputView>
void compute_nabla(InputView view, std::vector<OutputView> const &nabla)
{
    constexpr std::ptrdiff_t input_num_channels = num_channels<InputView>{};
    using pixel_type = typename OutputView::value_type;
    static_assert(num_channels<OutputView>{} == input_num_channels,
                  "input and output views must have the same amount of channels");
    const auto width = view.width();
    const auto height = view.height();
    using namespace direction;
    const auto zero_pixel = []() {
        pixel_type pixel;
        for (std::ptrdiff_t i = 0; i < input_num_channels; ++i)
        {
            pixel[i] = 0;
        }

        return pixel;
    }();
    auto minus = [](pixel_type lhs, pixel_type rhs) {
        for (std::ptrdiff_t i = 0; i < input_num_channels; ++i)
        {
            lhs[i] -= rhs[i];
        }

        return lhs;
    };
    // NW corner
    {
        const std::ptrdiff_t x = 0;
        const std::ptrdiff_t y = 0;
        const auto current = view(x, y);
        nabla[north](x, y) = zero_pixel;
        nabla[south](x, y) = minus(view(x, y + 1), current);
        nabla[west](x, y) = zero_pixel;
        nabla[east](x, y) = minus(view(x + 1, y), current);
        nabla[north_east](x, y) = zero_pixel;
        nabla[south_east](x, y) = minus(view(x + 1, y + 1), current);
        nabla[south_west](x, y) = zero_pixel;
        nabla[north_west](x, y) = zero_pixel;
    }

    // NE corner
    {
        const std::ptrdiff_t x = width - 1;
        const std::ptrdiff_t y = 0;
        const auto current = view(x, y);
        nabla[north](x, y) = zero_pixel;
        nabla[south](x, y) = minus(view(x, y + 1), current);
        nabla[west](x, y) = minus(view(x - 1, y), current);
        nabla[east](x, y) = zero_pixel;
        nabla[north_east](x, y) = zero_pixel;
        nabla[south_east](x, y) = zero_pixel;
        nabla[south_west](x, y) = minus(view(x - 1, y + 1), current);
        nabla[north_west](x, y) = zero_pixel;
    }

    // SW corner
    {
        const std::ptrdiff_t x = 0;
        const std::ptrdiff_t y = height - 1;
        const auto current = view(x, y);
        nabla[north](x, y) = minus(view(x, y - 1), current);
        nabla[south](x, y) = zero_pixel;
        nabla[west](x, y) = zero_pixel;
        nabla[east](x, y) = minus(view(x + 1, y), current);
        nabla[north_east](x, y) = minus(view(x + 1, y - 1), current);
        nabla[south_east](x, y) = zero_pixel;
        nabla[south_west](x, y) = zero_pixel;
        nabla[north_west](x, y) = zero_pixel;
    }

    // SE corner
    {
        const std::ptrdiff_t x = width - 1;
        const std::ptrdiff_t y = height - 1;
        const auto current = view(x, y);
        nabla[north](x, y) = minus(view(x, y - 1), current);
        nabla[south](x, y) = zero_pixel;
        nabla[west](x, y) = minus(view(x - 1, y), current);
        nabla[east](x, y) = zero_pixel;
        nabla[north_east](x, y) = zero_pixel;
        nabla[south_east](x, y) = zero_pixel;
        nabla[south_west](x, y) = zero_pixel;
        nabla[north_west](x, y) = minus(view(x - 1, y - 1), current);
    }

    // first and last rows
    {
        const std::ptrdiff_t y1 = 0;
        const std::ptrdiff_t y2 = height - 1;
        for (std::ptrdiff_t x = 1; x < width - 1; ++x)
        {
            const auto current1 = view(x, y1);
            const auto current2 = view(x, y2);

            nabla[north](x, y1) = zero_pixel;
            nabla[north](x, y2) = minus(view(x, y2 - 1), current2);
            nabla[south](x, y1) = minus(view(x, y1 + 1), current1);
            nabla[south](x, y2) = zero_pixel;
            nabla[west](x, y1) = minus(view(x - 1, y1), current1);
            nabla[west](x, y2) = minus(view(x - 1, y2), current2);
            nabla[east](x, y1) = minus(view(x + 1, y1), current1);
            nabla[east](x, y2) = minus(view(x + 1, y2), current2);

            nabla[north_east](x, y1) = zero_pixel;
            nabla[north_east](x, y2) = minus(view(x + 1, y2 - 1), current2);
            nabla[south_east](x, y1) = minus(view(x + 1, y1 + 1), current1);
            nabla[south_east](x, y2) = zero_pixel;
            nabla[south_west](x, y1) = minus(view(x - 1, y1 + 1), current1);
            nabla[north_west](x, y1) = zero_pixel;
        }
    }

    // first and last columns
    {
        const std::ptrdiff_t x1 = 0;
        const std::ptrdiff_t x2 = width - 1;
        for (std::ptrdiff_t y = 1; y < height - 1; ++y)
        {
            const auto current1 = view(x1, y);
            const auto current2 = view(x2, y);
            nabla[north](x1, y) = minus(view(x1, y - 1), current1);
            nabla[north](x2, y) = minus(view(x2, y - 1), current2);
            nabla[south](x1, y) = minus(view(x1, y + 1), current1);
            nabla[south](x2, y) = minus(view(x2, y + 1), current2);
            nabla[west](x1, y) = zero_pixel;
            nabla[west](x2, y) = minus(view(x2 - 1, y), current2);
            nabla[east](x1, y) = minus(view(x1 + 1, y), current1);
            nabla[east](x2, y) = zero_pixel;

            nabla[north_east](x1, y) = minus(view(x1 + 1, y - 1), current1);
            nabla[north_west](x2, y) = zero_pixel;
            nabla[south_east](x1, y) = minus(view(x1 + 1, y + 1), current1);
            nabla[south_east](x2, y) = zero_pixel;
            nabla[south_west](x1, y) = zero_pixel;
            nabla[south_west](x2, y) = minus(view(x2 - 1, y + 1), current2);
            nabla[north_west](x1, y) = zero_pixel;
            nabla[north_west](x2, y) = minus(view(x2 - 1, y - 1), current2);
        }
    }

    for (std::ptrdiff_t y = 1; y < view.height() - 1; ++y)
    {
        for (std::ptrdiff_t x = 1; x < view.width() - 1; ++x)
        {
            for (std::ptrdiff_t channel_index = 0; channel_index < input_num_channels;
                 ++channel_index)
            {
                nabla[north](x, y)[channel_index] =
                    view(x, y - 1)[channel_index] - view(x, y)[channel_index];
                nabla[south](x, y)[channel_index] =
                    view(x, y + 1)[channel_index] - view(x, y)[channel_index];
                nabla[west](x, y)[channel_index] =
                    view(x - 1, y)[channel_index] - view(x, y)[channel_index];
                nabla[east](x, y)[channel_index] =
                    view(x + 1, y)[channel_index] - view(x, y)[channel_index];

                nabla[north_east](x, y)[channel_index] =
                    view(x + 1, y - 1)[channel_index] - view(x, y)[channel_index];
                nabla[south_east](x, y)[channel_index] =
                    view(x + 1, y + 1)[channel_index] - view(x, y)[channel_index];
                nabla[south_west](x, y)[channel_index] =
                    view(x - 1, y + 1)[channel_index] - view(x, y)[channel_index];
                nabla[north_west](x, y)[channel_index] =
                    view(x - 1, y - 1)[channel_index] - view(x, y)[channel_index];
            }
        }
    }
}

template <typename View>
void calculate_diffusvity(std::vector<View> nablas, double kappa,
                          std::vector<View> const &diffusivities)
{
    using pixel_type = typename View::value_type;
    using channel_type = typename channel_type<View>::type;
    for (std::size_t i = 0; i < nablas.size(); ++i)
    {
        transform_pixels(nablas[i], diffusivities[i], [kappa](pixel_type p) {
            auto op = [kappa](channel_type value) {
                value /= static_cast<channel_type>(kappa);
                auto result = std::exp(-value * value);
                return result;
            };
            pixel_type result_pixel;
            static_transform(p, result_pixel, op);
            return result_pixel;
        });
    }
}
} // namespace detail

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
void anisotropic_diffusion(InputView input, unsigned int num_iter, double delta_t, double kappa,
                           OutputView output)
{
    copy_pixels(input, output);
    using element_type = typename OutputView::value_type;
    using computation_image_type = image<element_type>;
    using channel_type = typename channel_type<OutputView>::type;

    for (unsigned int i = 0; i < num_iter; ++i)
    {
        std::vector<computation_image_type> nabla_images(
            8, computation_image_type(input.dimensions()));
        std::vector<typename computation_image_type::view_t> nabla;
        std::transform(nabla_images.begin(), nabla_images.end(), std::back_inserter(nabla),
                       [](computation_image_type &img) { return gil::view(img); });

        std::vector<computation_image_type> diffusivity_images(
            8, computation_image_type(input.dimensions()));
        std::vector<typename computation_image_type::view_t> diffusivity;
        std::transform(diffusivity_images.begin(), diffusivity_images.end(),
                       std::back_inserter(diffusivity),
                       [](computation_image_type &img) { return gil::view(img); });

        detail::compute_nabla(output, nabla);
        detail::calculate_diffusvity(nabla, kappa, diffusivity);

        channel_type half = channel_type(1.0f / 2);
        constexpr std::ptrdiff_t channel_count = num_channels<OutputView>{};
        for (std::ptrdiff_t y = 0; y < output.height(); ++y)
        {
            for (std::ptrdiff_t x = 0; x < output.width(); ++x)
            {
                for (std::ptrdiff_t channel_index = 0; channel_index < channel_count;
                     ++channel_index)
                {
                    using namespace detail::direction;
                    channel_type delta = static_cast<channel_type>(
                        delta_t *
                        (diffusivity[north](x, y)[channel_index] *
                             nabla[north](x, y)[channel_index] +
                         diffusivity[south](x, y)[channel_index] *
                             nabla[south](x, y)[channel_index] +
                         diffusivity[west](x, y)[channel_index] * nabla[west](x, y)[channel_index] +
                         diffusivity[east](x, y)[channel_index] * nabla[east](x, y)[channel_index] +
                         half * diffusivity[north_east](x, y)[channel_index] *
                             nabla[north_east](x, y)[channel_index] +
                         half * diffusivity[south_east](x, y)[channel_index] *
                             nabla[south_east](x, y)[channel_index] +
                         half * diffusivity[south_west](x, y)[channel_index] *
                             nabla[south_west](x, y)[channel_index] +
                         half * diffusivity[north_west](x, y)[channel_index] *
                             nabla[north_west](x, y)[channel_index]));
                    output(x, y)[channel_index] = output(x, y)[channel_index] + delta;
                }
            }
        }
    }
}

}} // namespace boost::gil
