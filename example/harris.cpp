//
// Copyright 2019 Olzhas Zhumabek <anonymous.from.applecity@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/image_processing/numeric.hpp>
#include <boost/gil/image_processing/harris.hpp>
#include <vector>
#include <functional>
#include <set>
#include <iostream>
#include <fstream>

namespace gil = boost::gil;

// some images might produce artifacts
// when converted to grayscale,
// which was previously observed on
// canny edge detector for test input
// used for this example
gil::gray8_image_t to_grayscale(gil::rgb8_view_t original)
{
    gil::gray8_image_t output_image(original.dimensions());
    auto output = gil::view(output_image);
    constexpr double max_channel_intensity = (std::numeric_limits<std::uint8_t>::max)();
    for (long int y = 0; y < original.height(); ++y) {
        for (long int x = 0; x < original.width(); ++x) {
            // scale the values into range [0, 1] and calculate linear intensity
            double red_intensity = original(x, y).at(std::integral_constant<int, 0>{})
                / max_channel_intensity;
            double green_intensity = original(x, y).at(std::integral_constant<int, 1>{})
                / max_channel_intensity;
            double blue_intensity = original(x, y).at(std::integral_constant<int, 2>{})
                / max_channel_intensity;
            auto linear_luminosity = 0.2126 * red_intensity
                                    + 0.7152 * green_intensity
                                    + 0.0722 * blue_intensity;

            // perform gamma adjustment
            double gamma_compressed_luminosity = 0;
            if (linear_luminosity < 0.0031308) {
                gamma_compressed_luminosity = linear_luminosity * 12.92;
            } else {
                gamma_compressed_luminosity = 1.055 * std::pow(linear_luminosity, 1 / 2.4) - 0.055;
            }

            // since now it is scaled, descale it back
            output(x, y) = gamma_compressed_luminosity * max_channel_intensity;
        }
    }

    return output_image;
}

void apply_gaussian_blur(gil::gray8_view_t input_view, gil::gray8_view_t output_view)
{
    constexpr static auto filterHeight = 5ull;
    constexpr static auto filterWidth = 5ull;
    constexpr static double filter[filterHeight][filterWidth] =
    {
        2,  4,  6,  4,  2,
        4, 9, 12, 9,  4,
        5, 12, 15, 12,  5,
        4, 9, 12, 9,  4,
        2,  4,  5,  4,  2,
    };
    constexpr double factor = 1.0 / 159;
    constexpr double bias = 0.0;

    const auto height = input_view.height();
    const auto width = input_view.width();
    for (long x = 0; x < width; ++x) {
        for (long y = 0; y < height; ++y) {
            double intensity = 0.0;
            for (size_t filter_y = 0; filter_y < filterHeight; ++filter_y) {
                for (size_t filter_x = 0; filter_x < filterWidth; ++filter_x) {
                    int image_x = x - filterWidth / 2 + filter_x;
                    int image_y = y - filterHeight / 2 + filter_y;
                    if (image_x >= input_view.width() || image_x < 0
                        || image_y >= input_view.height() || image_y < 0) {
                        continue;
                    }
                    auto& pixel = input_view(image_x, image_y);
                    intensity += pixel.at(std::integral_constant<int, 0>{})
                        * filter[filter_y][filter_x];
                }
            }
            auto& pixel = output_view(gil::point_t(x, y));
            pixel = (std::min)((std::max)(int(factor * intensity + bias), 0), 255);
        }

    }
}

void calculate_gradients(gil::gray8_view_t input,
    gil::gray16_view_t x_gradient,
    gil::gray16_view_t y_gradient)
{
    using x_coord_t = gil::gray16_view_t::x_coord_t;
    using y_coord_t = gil::gray16_view_t::y_coord_t;
    using pixel_t = std::remove_reference<decltype(x_gradient(0, 0))>::type;
    using channel_t = typename std::remove_reference<
                        decltype(
                            std::declval<gil::gray16_pixel_t>().at(
                                std::integral_constant<int, 0>{}
                            )
                        )
                       >::type;

    constexpr double x_kernel[3][3] =
    {
        {1, 0, -1},
        {2, 0, -2},
        {1, 0, -1}
    };
    constexpr double y_kernel[3][3] =
    {
        {1, 2, 1},
        {0, 0, 0},
        {-1, -2, -1}
    };
    constexpr auto chosen_channel = std::integral_constant<int, 0>{};
    for (y_coord_t y = 1; y < input.height() - 1; ++y)
    {
        for (x_coord_t x = 1; x < input.width() - 1; ++x)
        {
            gil::gray16_pixel_t x_result;
            boost::gil::static_transform(x_result, x_result,
                [](channel_t) { return static_cast<channel_t>(0); });
            gil::gray16_pixel_t y_result;
            boost::gil::static_transform(y_result, y_result,
                [](channel_t) { return static_cast<channel_t>(0); });
            for (y_coord_t y_filter = 0; y_filter < 2; ++y_filter)
            {
                for (x_coord_t x_filter = 0; x_filter < 2; ++x_filter)
                {
                    auto adjusted_y = y + y_filter - 1;
                    auto adjusted_x = x + x_filter - 1;
                    x_result.at(std::integral_constant<int, 0>{}) +=
                        input(adjusted_x, adjusted_y).at(chosen_channel)
                        * x_kernel[y_filter][x_filter];
                    y_result.at(std::integral_constant<int, 0>{}) +=
                        input(adjusted_x, adjusted_y).at(chosen_channel)
                        * y_kernel[y_filter][x_filter];
                }
            }
            x_gradient(x, y) = static_cast<std::uint8_t>(x_result.at(chosen_channel));
            y_gradient(x, y) = static_cast<std::uint8_t>(y_result.at(chosen_channel));
        }
    }
}

std::vector<gil::point_t> suppress(
    gil::gray32f_view_t harris_response,
    double harris_response_threshold)
{
    std::vector<gil::point_t> corner_points;
    for (gil::gray32f_view_t::coord_t y = 1; y < harris_response.height() - 1; ++y)
    {
        for (gil::gray32f_view_t::coord_t x = 1; x < harris_response.width() - 1; ++x)
        {
            auto value = [](gil::gray32f_pixel_t pixel) {
                return pixel.at(std::integral_constant<int, 0>{});
            };
            double values[9] = {
                value(harris_response(x - 1, y - 1)),
                value(harris_response(x, y - 1)),
                value(harris_response(x + 1, y - 1)),
                value(harris_response(x - 1, y)),
                value(harris_response(x, y)),
                value(harris_response(x + 1, y)),
                value(harris_response(x - 1, y + 1)),
                value(harris_response(x, y + 1)),
                value(harris_response(x + 1, y + 1))
            };

            auto maxima = *std::max_element(
                values,
                values + 9,
                [](double lhs, double rhs)
                {
                    return lhs < rhs;
                }
            );

            if (maxima == value(harris_response(x, y))
                && std::count(values, values + 9, maxima) == 1
                && maxima >= harris_response_threshold)
            {
                corner_points.emplace_back(x, y);
            }
        }
    }

    return corner_points;
}

int main(int argc, char* argv[])
{
    if (argc != 6)
    {
        std::cout << "usage: " << argv[0] << " <input.png> <odd-window-size>"
            " <discrimination-constant> <harris-response-threshold> <output.png>\n";
        return -1;
    }

    long int window_size = std::stoi(argv[2]);
    double discrimnation_constant = std::stod(argv[3]);
    long harris_response_threshold = std::stol(argv[4]);

    gil::rgb8_image_t input_image;

    gil::read_image(argv[1], input_image, gil::png_tag{});

    auto input_view = gil::view(input_image);
    auto grayscaled = to_grayscale(input_view);
    gil::gray8_image_t smoothed_image(grayscaled.dimensions());
    auto smoothed = gil::view(smoothed_image);
    apply_gaussian_blur(gil::view(grayscaled), smoothed);
    gil::gray16_image_t x_gradient_image(grayscaled.dimensions());
    gil::gray16_image_t y_gradient_image(grayscaled.dimensions());

    auto x_gradient = gil::view(x_gradient_image);
    auto y_gradient = gil::view(y_gradient_image);
    calculate_gradients(smoothed, x_gradient, y_gradient);

    gil::gray32f_image_t m11(x_gradient.dimensions());
    gil::gray32f_image_t m12_21(x_gradient.dimensions());
    gil::gray32f_image_t m22(x_gradient.dimensions());
    gil::compute_tensor_entries(
        x_gradient,
        y_gradient,
        gil::view(m11),
        gil::view(m12_21),
        gil::view(m22)
    );

    gil::gray32f_image_t harris_response(x_gradient.dimensions());
    gil::gray32f_image_t gaussian_kernel(gil::point_t(5, 5));
    gil::generate_gaussian_kernel(gil::view(gaussian_kernel), 0.84089642);
    gil::compute_harris_responses(
        gil::view(m11),
        gil::view(m12_21),
        gil::view(m22),
        gil::view(gaussian_kernel),
        discrimnation_constant,
        gil::view(harris_response)
    );

    auto corner_points = suppress(gil::view(harris_response), harris_response_threshold);
    for (auto point: corner_points)
    {
        input_view(point) = gil::rgb8_pixel_t(0, 0, 0);
        input_view(point).at(std::integral_constant<int, 1>{}) = 255;
    }
    gil::write_view(argv[5], input_view, gil::png_tag{});
}
