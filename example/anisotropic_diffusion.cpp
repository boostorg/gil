#include <boost/gil/algorithm.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/io/read_image.hpp>
#include <boost/gil/io/write_view.hpp>
#include <boost/gil/extension/io/png/tags.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/io/typedefs.hpp>
#include <boost/gil/pixel.hpp>

#include <cmath>
#include <string>
#include <vector>
#include <type_traits>

namespace gil = boost::gil;

using gray_channel = std::integral_constant<int, 0>;

namespace boost{namespace gil{
using gray64f_view_t = gil::image_view<gil::memory_based_2d_locator<gil::memory_based_step_iterator<gil::gray64f_pixel_t*>>>;
}}
namespace boost{ namespace gil{
enum direction {
    north = 0,
    south = 1,
    west = 2,
    east = 3,
    north_east = 4,
    south_east = 5,
    south_west = 6,
    north_west = 7
};

template <typename OutputView, typename InputView>
void compute_nabla(InputView view, const std::vector<OutputView>& nabla) {
    constexpr auto input_num_channels = num_channels<InputView>{};
    static_assert(num_channels<OutputView>{} == input_num_channels);
    for (std::ptrdiff_t y = 1; y < view.height() - 1; ++y)
    {
        for (std::ptrdiff_t x = 1; x < view.width() - 1; ++x)
        {
            for (std::ptrdiff_t channel_index = 0; channel_index < input_num_channels; ++channel_index)
            {
                nabla[north](x, y) = view(x, y - 1)[channel_index] - view(x, y)[channel_index];
                nabla[south](x, y) = view(x, y + 1)[channel_index] - view(x, y)[channel_index];
                nabla[west](x, y) = view(x - 1, y)[channel_index] - view(x, y)[channel_index];
                nabla[east](x, y) = view(x + 1, y)[channel_index] - view(x, y)[channel_index];

                nabla[north_east](x, y) = view(x + 1, y - 1)[channel_index] - view(x, y)[channel_index];
                nabla[south_east](x, y) = view(x + 1, y + 1)[channel_index] - view(x, y)[channel_index];
                nabla[south_west](x, y) = view(x - 1, y + 1)[channel_index] - view(x, y)[channel_index];
                nabla[north_west](x, y) = view(x - 1, y - 1)[channel_index] - view(x, y)[channel_index];
            }
        }
    }
}

template <typename View>
void calculate_diffusvity(std::vector<View> nablas, double kappa, const std::vector<View> diffusivities)
{
    using pixel_type = typename View::value_type;
    using channel_type = typename channel_type<View>::type;
    BOOST_ASSERT(nablas.size() == diffusivities.size());
    for (std::size_t i = 0; i < nablas.size(); ++i) {
        gil::transform_pixels(nablas[i], diffusivities[i], [kappa](pixel_type p)
        {
            auto op = [kappa](channel_type value)
            {
                value /= kappa;
                float result = std::exp(-value * value);
                return result;               
            };
            pixel_type result_pixel;
            static_transform(p, result_pixel, op);
            return result_pixel;
        });
    }
}

template <typename ImageView>
void print_images(const std::vector<ImageView>& views, const std::string& prefix = "./nabla")
{
    unsigned int counter = 0;
    for (const auto& view: views)
    {
        gil::write_view(prefix + std::to_string(counter) + ".png", gil::color_converted_view<gil::gray8_pixel_t>(view), gil::png_tag{});
        ++counter;
    }
}

template <typename InputView, typename OutputView>
void anisotropic_diffusion(InputView input, unsigned int num_iter, double delta_t, double kappa, OutputView output)
{
    gil::copy_pixels(input, output);
    using element_type = typename OutputView::value_type;
    using computation_image_type = image<element_type>;

    for (unsigned int i = 0; i < num_iter; ++i) {
        std::vector<computation_image_type> nabla_images(8, computation_image_type(input.dimensions()));
        std::vector<typename computation_image_type::view_t> nabla;
        std::transform(nabla_images.begin(), nabla_images.end(),
                       std::back_inserter(nabla), [](computation_image_type& img)
                        {
                            return gil::view(img);
                        });

        std::vector<computation_image_type> diffusivity_images(8, computation_image_type(input.dimensions()));
        std::vector<typename computation_image_type::view_t> diffusivity;
        std::transform(diffusivity_images.begin(), diffusivity_images.end(),
                       std::back_inserter(diffusivity), [](computation_image_type& img)
                        {
                            return gil::view(img);
                        });

        compute_nabla(output, nabla);
        calculate_diffusvity(nabla, kappa, diffusivity);

        // print_images(nabla);
        // print_images(diffusivity, "diffusivity");

        float half = float(1.0f / 2);
        constexpr auto channel_count = num_channels<OutputView>{};
        for (std::ptrdiff_t y = 0; y < output.height(); ++y)
        {
            for (std::ptrdiff_t x = 0; x < output.width(); ++ x)
            {
                for (std::ptrdiff_t channel_index = 0; channel_index < channel_count; ++channel_index) {
                    float delta = delta_t * (
                        diffusivity[north](x, y)[channel_index] * nabla[north](x, y)[channel_index] + diffusivity[south](x, y)[channel_index] * nabla[south](x, y)[channel_index]
                        + diffusivity[west](x, y)[channel_index] * nabla[west](x, y)[channel_index] + diffusivity[east](x, y)[channel_index] * nabla[east](x, y)[channel_index]
                        + half * diffusivity[north_east](x, y)[channel_index] * nabla[north_east](x, y)[channel_index] + half * diffusivity[south_east](x, y)[channel_index] * nabla[south_east](x, y)[channel_index]
                        + half * diffusivity[south_west](x, y)[channel_index] * nabla[south_west](x, y)[channel_index] + half * diffusivity[north_west](x, y)[channel_index] * nabla[north_west](x, y)[channel_index]
                    );
                    output(x, y)[channel_index] = output(x, y)[channel_index] + delta;
                }
            }
        }
    }
}
}}

#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 5)
    {
        std::cerr << "usage: " << argv[0] << " <gray input.png> <output.png> <positive iteration count> <positive kappa~30>\n";
        return -1;
    }
    std::string input_path = argv[1];
    std::string output_path = argv[2];

    unsigned int iteration_count = static_cast<unsigned int>(std::stoul(argv[3]));
    unsigned int kappa = static_cast<unsigned int>(std::stoul(argv[4]));

    gil::gray8_image_t input;
    gil::read_image(input_path, input, gil::png_tag{});
    auto input_view = gil::view(input);

    gil::gray64f_image_t gray(input.dimensions());
    auto gray_view = gil::view(gray);

    gil::transform_pixels(input_view, gray_view,
    [](const gil::gray8_pixel_t& p)
    {
        return p.at(gray_channel{});
    });
    gil::gray64f_image_t output(gray.dimensions());
    auto output_view = gil::view(output);

    anisotropic_diffusion(gray_view, iteration_count, 1 / 7., kappa, output_view);

    gil::gray8_image_t true_output(output.dimensions());
    gil::transform_pixels(output_view, gil::view(true_output), [](gil::gray64f_pixel_t p)
    {
        return p.at(gray_channel{});
    });

    gil::write_view(output_path, gil::view(true_output), gil::png_tag{});
}