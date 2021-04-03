//
// Copyright 2021 Harsit Pant <harshitpant83@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_processing/distance_transform.hpp>
#include <boost/gil/extension/io/png.hpp>

#include <iostream>

namespace gil = boost::gil;

/// calculates distance transform based on euclidean distance measurement formulae.

int main(int argc, char* argv[])
{
    if (argc != 6)
    {
        std::cerr << "usage: " << argv[0] << " <input.png> <output.png>"
                                             " <distance_from> <distance_type> <mask_size>";
        return -1;
    }

    std::string dist_from = argv[3];
    std::string dist_type = argv[4];
    std::string mask_size = argv[5];

    gil::gray8_image_t input;
    gil::read_image(argv[1], input, gil::png_tag{});
    gil::gray8_image_t output(input.dimensions());

    if (dist_from == "on_pixels" && dist_type == "euclidean_approximation" && mask_size == "three")
    {
        distance_transform(view(input), view(output), gil::distance_from::on_pixels,
            gil::distance_type::euclidean_approximation, gil::mask_size::three);
        gil::write_view(argv[2], view(output), gil::png_tag{});
    }

    else if (dist_from == "off_pixels" && dist_type == "euclidean_approximation" && mask_size == "three")
    {
        distance_transform(view(input), view(output), gil::distance_from::off_pixels,
            gil::distance_type::euclidean_approximation, gil::mask_size::three);
        gil::write_view(argv[2], view(output), gil::png_tag{});
    }

    else if (dist_from == "on_pixels" && dist_type == "euclidean_approximation" && mask_size == "five")
    {
        distance_transform(view(input), view(output), gil::distance_from::on_pixels,
            gil::distance_type::euclidean_approximation, gil::mask_size::five);
        gil::write_view(argv[2], view(output), gil::png_tag{});
    }

    else if (dist_from == "off_pixels" && dist_type == "euclidean_approximation" && mask_size == "five")
    {
        distance_transform(view(input), view(output), gil::distance_from::off_pixels,
            gil::distance_type::euclidean_approximation, gil::mask_size::five);
        gil::write_view(argv[2], view(output), gil::png_tag{});
    }

    else if (dist_from == "on_pixels" && dist_type == "precise_euclidean" && mask_size == "not_applicable")
    {
        distance_transform(view(input), view(output), gil::distance_from::on_pixels,
            gil::distance_type::precise_euclidean, gil::mask_size::not_applicable);
        gil::write_view(argv[2], view(output), gil::png_tag{});
    }

    else if (dist_from == "off_pixels" && dist_type == "precise_euclidean" && mask_size == "not_applicable")
    {
        distance_transform(view(input), view(output), gil::distance_from::off_pixels,
            gil::distance_type::precise_euclidean, gil::mask_size::not_applicable);
        gil::write_view(argv[2], view(output), gil::png_tag{});
    }
    else
    {
        std::cerr << "Incorrect options used.";
    }
}
