#include <boost/gil/algorithm.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/io/read_image.hpp>
#include <boost/gil/io/write_view.hpp>
#include <boost/gil/extension/io/png/tags.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/io/typedefs.hpp>

#include <cmath>
#include <type_traits>
#include <iostream>
#include <cstdio>

namespace gil = boost::gil;

using gray_channel = std::integral_constant<int, 0>;

namespace boost{namespace gil{
using gray64f_view_t = gil::image_view<gil::memory_based_2d_locator<gil::memory_based_step_iterator<gil::gray64f_pixel_t*>>>;
}}

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

void compute_nabla(gil::gray64f_view_t view, const std::vector<gil::gray32f_view_t>& nabla) {
    for (std::ptrdiff_t y = 1; y < view.height() - 1; ++y)
    {
        for (std::ptrdiff_t x = 1; x < view.width() - 1; ++x)
        {
            nabla[north](x, y) = view(x, y - 1).at(gray_channel{}) - view(x, y).at(gray_channel{});
            nabla[south](x, y) = view(x, y + 1).at(gray_channel{}) - view(x, y).at(gray_channel{});
            nabla[west](x, y) = view(x - 1, y).at(gray_channel{}) - view(x, y).at(gray_channel{});
            nabla[east](x, y) = view(x + 1, y).at(gray_channel{}) - view(x, y).at(gray_channel{});

            nabla[north_east](x, y) = view(x + 1, y - 1).at(gray_channel{}) - view(x, y).at(gray_channel{});
            nabla[south_east](x, y) = view(x + 1, y + 1).at(gray_channel{}) - view(x, y).at(gray_channel{});
            nabla[south_west](x, y) = view(x - 1, y + 1).at(gray_channel{}) - view(x, y).at(gray_channel{});
            nabla[north_west](x, y) = view(x - 1, y - 1).at(gray_channel{}) - view(x, y).at(gray_channel{});
        }
    }
}

void calculate_diffusvity(std::vector<gil::gray32f_view_t> nablas, double kappa, const std::vector<gil::gray32f_view_t> diffusivities)
{
    BOOST_ASSERT(nablas.size() == diffusivities.size());
    for (std::size_t i = 0; i < nablas.size(); ++i) {
        gil::transform_pixels(nablas[i], diffusivities[i], [kappa](gil::gray32f_pixel_t p)
        {
            float value = p.at(gray_channel{}) / kappa;
            float result = std::exp(-value * value);
            return result;
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

void anisotropic_diffusion(gil::gray64f_view_t input, unsigned int num_iter, double delta_t, double kappa, gil::gray64f_view_t output)
{
    gil::copy_pixels(input, output);

    for (unsigned int i = 0; i < num_iter; ++i) {
        std::vector<gil::gray32f_image_t> nabla_images(8, gil::gray32f_image_t(input.dimensions()));
        std::vector<gil::gray32f_view_t> nabla;
        std::transform(nabla_images.begin(), nabla_images.end(),
                       std::back_inserter(nabla), [](gil::gray32f_image_t& i)
                        {
                            return gil::view(i);
                        });

        std::vector<gil::gray32f_image_t> diffusivity_images(8, gil::gray32f_image_t(input.dimensions()));
        std::vector<gil::gray32f_view_t> diffusivity;
        std::transform(diffusivity_images.begin(), diffusivity_images.end(),
                       std::back_inserter(diffusivity), [](gil::gray32f_image_t& i)
                        {
                            return gil::view(i);
                        });

        compute_nabla(output, nabla);
        calculate_diffusvity(nabla, kappa, diffusivity);

        print_images(nabla);
        print_images(diffusivity, "diffusivity");

        float half = float(1.0f / 2);
        for (std::ptrdiff_t y = 0; y < output.height(); ++y)
        {
            for (std::ptrdiff_t x = 0; x < output.width(); ++ x)
            {
                float delta = delta_t * (
                    diffusivity[north](x, y).at(gray_channel{}) * nabla[north](x, y).at(gray_channel{}) + diffusivity[south](x, y).at(gray_channel{}) * nabla[south](x, y).at(gray_channel{})
                    + diffusivity[west](x, y).at(gray_channel{}) * nabla[west](x, y).at(gray_channel{}) + diffusivity[east](x, y).at(gray_channel{}) * nabla[east](x, y).at(gray_channel{})
                    + half * diffusivity[north_east](x, y).at(gray_channel{}) * nabla[north_east](x, y).at(gray_channel{}) + half * diffusivity[south_east](x, y).at(gray_channel{}) * nabla[south_east](x, y).at(gray_channel{})
                    + half * diffusivity[south_west](x, y).at(gray_channel{}) * nabla[south_west](x, y).at(gray_channel{}) + half * diffusivity[north_west](x, y).at(gray_channel{}) * nabla[north_west](x, y).at(gray_channel{})
                );
                output(x, y) = output(x, y).at(gray_channel{}) + delta;
            }
        }
    }
}

int main()
{
    gil::rgb8_image_t input;
    gil::read_image("./input.png", input, gil::png_tag{});
    auto input_view = gil::view(input);

    gil::gray64f_image_t gray(input.dimensions());
    auto gray_view = gil::view(gray);

    gil::transform_pixels(input_view, gray_view,
    [](const gil::rgb8_pixel_t& p)
    {
        return 0.3 * p.at(std::integral_constant<int, 0>{})
               + 0.59 * p.at(std::integral_constant<int, 1>{})
               + 0.11 * p.at(std::integral_constant<int, 2>{});
    });
    gil::gray64f_image_t output(gray.dimensions());
    auto output_view = gil::view(output);

    anisotropic_diffusion(gray_view, 10, 1 / 7., 30, output_view);

    gil::gray8_image_t true_output(output.dimensions());
    gil::transform_pixels(output_view, gil::view(true_output), [](gil::gray64f_pixel_t p)
    {
        return p.at(gray_channel{});
    });

    gil::write_view("./output.png", gil::view(true_output), gil::png_tag{});
    gil::write_view("./gray.png", gil::color_converted_view<gil::gray8_pixel_t>(gray_view), gil::png_tag{});

}
