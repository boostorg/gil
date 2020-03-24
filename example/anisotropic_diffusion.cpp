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

enum class diffusion_option {
    privelege_high_contrast,
    privelege_wider_area
};

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

void compute_nabla(gil::gray8_view_t view, std::vector<gil::gray32f_view_t> nabla) {
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

template <typename Image, typename T>
auto convolve(const Image& input, gil::detail::kernel_2d<T> kernel) -> Image {
    Image result(input.dimensions());

    gil::detail::convolve_2d(gil::view(input), kernel, gil::view(result));

    return result;
}

void calculate_c(gil::gray32f_view_t input, double kappa, gil::gray32f_view_t output)
{
    // gil::for_each_pixel(input, [](gil::gray32f_pixel_t p))
    gil::transform_pixels(input, output, [kappa](gil::gray32f_pixel_t p)
    {
        float value = p.at(gray_channel{}) / kappa;
        float result = std::exp(-value * value);
        // float result = 1.0 / (1.0 + value * value);
        // if (result < 0.5) {
        //     printf("%f\n", value);
        // }
        return result;
    });
}

template <typename ImageView>
void print_images(std::initializer_list<ImageView> views, const std::string& prefix = "./nabla")
{
    unsigned int counter = 0;
    for (const auto& view: views)
    {
        gil::write_view(prefix + std::to_string(counter) + ".png", gil::color_converted_view<gil::gray8_pixel_t>(view), gil::png_tag{});
        ++counter;
    }
}

void anisotropic_diffusion(gil::gray8c_view_t input, unsigned int num_iter, double delta_t, double kappa, gil::gray8_view_t output)
{
    gil::copy_and_convert_pixels(input, output);
    double dx = 1.0;
    double dy = 1.0;
    double dd = std::sqrt(2);

    for (unsigned int i = 0; i < num_iter; ++i) {

        gil::gray32f_image_t nabla_north(input.dimensions());
        gil::gray32f_image_t nabla_south(input.dimensions());
        gil::gray32f_image_t nabla_west(input.dimensions());
        gil::gray32f_image_t nabla_east(input.dimensions());

        gil::gray32f_image_t nabla_north_east(input.dimensions());
        gil::gray32f_image_t nabla_south_east(input.dimensions());
        gil::gray32f_image_t nabla_south_west(input.dimensions());
        gil::gray32f_image_t nabla_north_west(input.dimensions());

        gil::gray32f_image_t c_north(input.dimensions());
        gil::gray32f_image_t c_south(input.dimensions());
        gil::gray32f_image_t c_west(input.dimensions());
        gil::gray32f_image_t c_east(input.dimensions());

        gil::gray32f_image_t c_north_east(input.dimensions());
        gil::gray32f_image_t c_south_east(input.dimensions());
        gil::gray32f_image_t c_south_west(input.dimensions());
        gil::gray32f_image_t c_north_west(input.dimensions());

        compute_nabla(output, {
            gil::view(nabla_north),
            gil::view(nabla_south),
            gil::view(nabla_west),
            gil::view(nabla_east),

            gil::view(nabla_north_east),
            gil::view(nabla_south_east),
            gil::view(nabla_south_west),
            gil::view(nabla_north_west),
            });

        calculate_c(gil::view(nabla_north), kappa, gil::view(c_north));
        calculate_c(gil::view(nabla_south), kappa, gil::view(c_south));
        calculate_c(gil::view(nabla_west), kappa, gil::view(c_west));
        calculate_c(gil::view(nabla_east), kappa, gil::view(c_east));

        calculate_c(gil::view(nabla_north_east), kappa, gil::view(c_north_east));
        calculate_c(gil::view(nabla_south_east), kappa, gil::view(c_south_east));
        calculate_c(gil::view(nabla_south_west), kappa, gil::view(c_south_west));
        calculate_c(gil::view(nabla_north_west), kappa, gil::view(c_north_west));

        print_images({
            gil::view(nabla_north),
            gil::view(nabla_south),
            gil::view(nabla_west),
            gil::view(nabla_east),

            gil::view(nabla_north_east),
            gil::view(nabla_south_east),
            gil::view(nabla_south_west),
            gil::view(nabla_north_west),
            });

        print_images({
            gil::view(c_north),
            gil::view(c_south),
            gil::view(c_west),
            gil::view(c_east),

            gil::view(c_north_east),
            gil::view(c_south_east),
            gil::view(c_south_west),
            gil::view(c_north_west),
            }, "diffusivity");

        float half = float(1.0f / 2);
        for (std::ptrdiff_t y = 0; y < output.height(); ++y)
        {
            for (std::ptrdiff_t x = 0; x < output.width(); ++ x)
            {
                float delta = delta_t * (
                    gil::view(c_north)(x, y).at(gray_channel{}) * gil::view(nabla_north)(x, y).at(gray_channel{}) + gil::view(c_south)(x, y).at(gray_channel{}) * gil::view(nabla_south)(x, y).at(gray_channel{})
                    + gil::view(c_west)(x, y).at(gray_channel{}) * gil::view(nabla_west)(x, y).at(gray_channel{}) + gil::view(c_east)(x, y).at(gray_channel{}) * gil::view(nabla_east)(x, y).at(gray_channel{})
                    + half * gil::view(c_north_east)(x, y).at(gray_channel{}) * gil::view(nabla_north_east)(x, y).at(gray_channel{}) + half * gil::view(c_south_east)(x, y).at(gray_channel{}) * gil::view(nabla_south_east)(x, y).at(gray_channel{})
                    + half * gil::view(c_south_west)(x, y).at(gray_channel{}) * gil::view(nabla_south_west)(x, y).at(gray_channel{}) + half * gil::view(c_north_west)(x, y).at(gray_channel{}) * gil::view(nabla_north_west)(x, y).at(gray_channel{})
                );
                // std::cout << delta << '\n';
                output(x, y) = output(x, y).at(gray_channel{}) + delta;
            }
        }
    }
}

// void anisotropic_diffusion(gil::gray8_view_t input, unsigned int num_iter, double delta_t, double kappa, diffusion_option option, gil::gray8_view_t output)
// {
//     gil::copy_pixels(input, output);
//     double dx = 1.0;
//     double dy = 1.0;
//     double dd = std::sqrt(2);

//     float h_n[3][3] = {
//         {0, 1, 0},
//         {0, -1, 0},
//         {0, 0, 0}
//     };
//     gil::detail::kernel_2d<float> h_nk(h_n, 1, 1, 9);
//     float h_s[3][3] = {
//         {0, 0, 0},
//         {0, -1, 0},
//         {0, 1, 0}
//     };
//     gil::detail::kernel_2d<float> h_sk(h_s, 1, 1, 9);
//     float h_e[3][3] = {
//         {0, 0, 0},
//         {0, -1, 1},
//         {0, 0, 0}
//     };
//     gil::detail::kernel_2d<float> h_ek(h_e, 1, 1, 9);
//     float h_w[3][3] = {
//         {0, 0, 0},
//         {1, -1, 0},
//         {0, 0, 0}
//     };
//     gil::detail::kernel_2d<float> h_wk(h_w, 1, 1, 9);
//     float h_ne[3][3] = {
//         {0, 0, 1},
//         {0, -1, 0},
//         {0, 0, 0}
//     };
//     gil::detail::kernel_2d<float> h_nek(h_ne, 1, 1, 9);
//     float h_se[3][3] = {
//         {0, 0, 0},
//         {0, -1, 0},
//         {1, 0, 0}
//     };
//     gil::detail::kernel_2d<float> h_sek(h_se, 1, 1, 9);
//     float h_sw[3][3] = {
//         {0, 0, 0},
//         {0, -1, 0},
//         {1, 0, 1}
//     };
//     gil::detail::kernel_2d<float> h_swk(h_sw, 1, 1, 9);
//     float h_nw[3][3] = {
//         {1, 0, 0},
//         {0, -1, 0},
//         {0, 0, 0}
//     };
//     gil::detail::kernel_2d<float> h_nwk(h_nw, 1, 1, 9);

//     for (unsigned int i = 1; i < num_iter; ++i) {

//         gil::gray32f_image_t nabla_ni(input.dimensions());
//         gil::detail::convolve_2d(output, h_nk, gil::view(nabla_ni));
//         diffuse(gil::view(nabla_ni), kappa, gil::view(nabla_ni));

//         gil::gray32f_image_t nabla_si(input.dimensions());
//         gil::detail::convolve_2d(output, h_sk, gil::view(nabla_si));
//         diffuse(gil::view(nabla_si), kappa, gil::view(nabla_si));

//         gil::gray32f_image_t nabla_ei(input.dimensions());
//         gil::detail::convolve_2d(output, h_ek, gil::view(nabla_ei));
//         diffuse(gil::view(nabla_ei), kappa, gil::view(nabla_ei));

//         gil::gray32f_image_t nabla_wi(input.dimensions());
//         gil::detail::convolve_2d(output, h_wk, gil::view(nabla_wi));
//         diffuse(gil::view(nabla_wi), kappa, gil::view(nabla_wi));

//         gil::gray32f_image_t nabla_nei(input.dimensions());
//         gil::detail::convolve_2d(output, h_nek, gil::view(nabla_nei));
//         diffuse(gil::view(nabla_nei), kappa, gil::view(nabla_nei));

//         gil::gray32f_image_t nabla_sei(input.dimensions());
//         gil::detail::convolve_2d(output, h_sek, gil::view(nabla_sei));
//         diffuse(gil::view(nabla_sei), kappa, gil::view(nabla_sei));

//         gil::gray32f_image_t nabla_swi(input.dimensions());
//         gil::detail::convolve_2d(output, h_swk, gil::view(nabla_swi));
//         diffuse(gil::view(nabla_swi), kappa, gil::view(nabla_swi));

//         gil::gray32f_image_t nabla_nwi(input.dimensions());
//         gil::detail::convolve_2d(output, h_nwk, gil::view(nabla_nwi));
//         diffuse(gil::view(nabla_nwi), kappa, gil::view(nabla_nwi));

//         for (std::ptrdiff_t y = 0; y < input.height(); ++y)
//         {
//             for (std::ptrdiff_t x = 0; x < input.width(); ++x)
//             {
//                 // output(x, y) = output(x, y).at(gray_channel{}) + delta_t * (
//                 //     gil::view(nabla_ei)(x, y) *
//                 // );
//             }
//         }
//     }
// }

int main()
{
    gil::rgb8_image_t input;
    gil::read_image("./input.png", input, gil::png_tag{});
    auto input_view = gil::view(input);

    gil::gray8_image_t gray(input.dimensions());
    auto gray_view = gil::view(gray);

    gil::copy_and_convert_pixels(input_view, gray_view);

    gil::gray8_image_t output(gray.dimensions());
    auto output_view = gil::view(output);

    anisotropic_diffusion(gray_view, 10, 1 / 7., 30, output_view);
    // anisotropic_diffusion(output_view, 10, 1 / 7, 30, output_view);
    // anisotropic_diffusion(output_view, 10, 1 / 7, 30, output_view);
    // anisotropic_diffusion(output_view, 10, 1 / 7, 30, output_view);
    // anisotropic_diffusion(output_view, 10, 1 / 7, 30, output_view);
    // gil::copy_pixels(gray_view, output_view);

    gil::write_view("./output.png", gil::color_converted_view<gil::gray8_pixel_t>(output_view), gil::png_tag{});
    gil::write_view("./gray.png", gil::color_converted_view<gil::gray8_pixel_t>(gray_view), gil::png_tag{});

}
