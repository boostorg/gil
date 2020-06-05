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

namespace boost{ namespace gil{
namespace detail {
enum class direction: std::size_t
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

// nabla is directed difference in pixels,
// nabla north for example represents intensity
// change in the north direction (up), and nabla south
// represents intensity change in south direction (down).
template <typename InputView, typename OutputView>
void compute_nabla(InputView view, std::vector<OutputView> const& nabla)
{
    constexpr std::ptrdiff_t input_num_channels = num_channels<InputView>{};
    static_assert(num_channels<OutputView>{} == input_num_channels,
        "input and output views must have the same amount of channels");
    for (std::ptrdiff_t y = 1; y < view.height() - 1; ++y)
    {
        for (std::ptrdiff_t x = 1; x < view.width() - 1; ++x)
        {
            for (std::ptrdiff_t channel_index = 0;
                channel_index < input_num_channels;
                ++channel_index)
            {
                nabla[(std::size_t)direction::north](x, y)[channel_index] =
                    view(x, y - 1)[channel_index] - view(x, y)[channel_index];
                nabla[(std::size_t)direction::south](x, y)[channel_index] =
                    view(x, y + 1)[channel_index] - view(x, y)[channel_index];
                nabla[(std::size_t)direction::west](x, y)[channel_index] =
                    view(x - 1, y)[channel_index] - view(x, y)[channel_index];
                nabla[(std::size_t)direction::east](x, y)[channel_index] =
                    view(x + 1, y)[channel_index] - view(x, y)[channel_index];

                nabla[(std::size_t)direction::north_east](x, y)[channel_index] =
                    view(x + 1, y - 1)[channel_index] - view(x, y)[channel_index];
                nabla[(std::size_t)direction::south_east](x, y)[channel_index] =
                    view(x + 1, y + 1)[channel_index] - view(x, y)[channel_index];
                nabla[(std::size_t)direction::south_west](x, y)[channel_index] =
                    view(x - 1, y + 1)[channel_index] - view(x, y)[channel_index];
                nabla[(std::size_t)direction::north_west](x, y)[channel_index] =
                    view(x - 1, y - 1)[channel_index] - view(x, y)[channel_index];
            }
        }
    }
}

template <typename View>
void calculate_diffusvity(std::vector<View> nablas,
    double kappa,
    std::vector<View> const& diffusivities)
{
    using pixel_type = typename View::value_type;
    using channel_type = typename channel_type<View>::type;
    for (std::size_t i = 0; i < nablas.size(); ++i) {
        gil::transform_pixels(nablas[i], diffusivities[i], [kappa](pixel_type p)
        {
            auto op = [kappa](channel_type value)
            {
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
} // namespace boost::gil::detail

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
void anisotropic_diffusion(InputView input,
    unsigned int num_iter,
    double delta_t,
    double kappa,
    OutputView output)
{
    gil::copy_pixels(input, output);
    using element_type = typename OutputView::value_type;
    using computation_image_type = image<element_type>;
    using channel_type = typename channel_type<OutputView>::type;

    for (unsigned int i = 0; i < num_iter; ++i)
    {
        std::vector<computation_image_type> nabla_images(8,
            computation_image_type(input.dimensions()));
        std::vector<typename computation_image_type::view_t> nabla;
        std::transform(nabla_images.begin(), nabla_images.end(),
            std::back_inserter(nabla), [](computation_image_type& img)
            {
                return gil::view(img);
            });

        std::vector<computation_image_type> diffusivity_images(8,
            computation_image_type(input.dimensions()));
        std::vector<typename computation_image_type::view_t> diffusivity;
        std::transform(diffusivity_images.begin(), diffusivity_images.end(),
            std::back_inserter(diffusivity), [](computation_image_type& img)
            {
                return gil::view(img);
            });

        detail::compute_nabla(output, nabla);
        detail::calculate_diffusvity(nabla, kappa, diffusivity);

        channel_type half = channel_type(1.0f / 2);
        constexpr std::ptrdiff_t channel_count = num_channels<OutputView>{};
        for (std::ptrdiff_t y = 0; y < output.height(); ++y)
        {
            for (std::ptrdiff_t x = 0; x < output.width(); ++ x)
            {
                for (std::ptrdiff_t channel_index = 0;
                    channel_index < channel_count;
                    ++channel_index)
                {
                    using detail::direction;
                    channel_type delta = static_cast<channel_type>(delta_t * (
                        diffusivity[(std::size_t)direction::north](x, y)[channel_index]
                            * nabla[(std::size_t)direction::north](x, y)[channel_index]
                        + diffusivity[(std::size_t)direction::south](x, y)[channel_index]
                            * nabla[(std::size_t)direction::south](x, y)[channel_index]
                        + diffusivity[(std::size_t)direction::west](x, y)[channel_index]
                            * nabla[(std::size_t)direction::west](x, y)[channel_index]
                        + diffusivity[(std::size_t)direction::east](x, y)[channel_index]
                            * nabla[(std::size_t)direction::east](x, y)[channel_index]
                        + half
                            * diffusivity[(std::size_t)direction::north_east](x, y)[channel_index]
                            * nabla[(std::size_t)direction::north_east](x, y)[channel_index]
                        + half
                            * diffusivity[(std::size_t)direction::south_east](x, y)[channel_index]
                            * nabla[(std::size_t)direction::south_east](x, y)[channel_index]
                        + half
                            * diffusivity[(std::size_t)direction::south_west](x, y)[channel_index]
                            * nabla[(std::size_t)direction::south_west](x, y)[channel_index]
                        + half
                            * diffusivity[(std::size_t)direction::north_west](x, y)[channel_index]
                            * nabla[(std::size_t)direction::north_west](x, y)[channel_index]
                    ));
                    output(x, y)[channel_index] = output(x, y)[channel_index] + delta;
                }
            }
        }
    }
}

}} // namespace boost::gil
