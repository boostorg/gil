#include <boost/gil/algorithm.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/io/typedefs.hpp>
#include <boost/gil/image_processing/numeric.hpp>

#include <cmath>
#include <string>
#include <vector>
#include <type_traits>

namespace gil = boost::gil;

namespace boost{namespace gil{
using gray64f_view_t = gil::image_view<gil::memory_based_2d_locator<gil::memory_based_step_iterator<gil::gray64f_pixel_t*>>>;
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
        return p[0];
    });
    gil::gray64f_image_t output(gray.dimensions());
    auto output_view = gil::view(output);

    gil::anisotropic_diffusion(gray_view, iteration_count, 1 / 7., kappa, output_view);

    gil::gray8_image_t true_output(output.dimensions());
    gil::transform_pixels(output_view, gil::view(true_output), [](gil::gray64f_pixel_t p)
    {
        return static_cast<gil::uint8_t>(p[0]);
    });

    gil::write_view(output_path, gil::view(true_output), gil::png_tag{});
}