//
// Created by shino on 13.08.21.
//
#include <boost/gil.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/extension/numeric/matrix.hpp>
#include <boost/gil/extension/numeric/arithmetics.hpp>

namespace gil = boost::gil;

#include <iostream>

int main() {
    gil::gray8_image_t input_image;

    gil::read_image("input.png", input_image, gil::png_tag{});
    auto input = gil::view(input_image);


    gil::matrix_storage matrix(input.dimensions());
    auto view = gil::view(matrix);
    gil::value_cast(input, view);

    auto dx_kernel = gil::generate_dx_sobel();
    auto dy_kernel = gil::generate_dy_sobel();

    gil::matrix_storage dx_matrix(input.dimensions());
    gil::matrix_storage dy_matrix(input.dimensions());
    auto dx = gil::view(dx_matrix);
    auto dy = gil::view(dy_matrix);

    gil::convolve_2d<gil::elem_t>(view, dx_kernel, dx, gil::boundary_option::extend_constant);
    gil::convolve_2d<gil::elem_t>(view, dy_kernel, dy, gil::boundary_option::extend_constant);

    gil::gray8_image_t output_image(input.dimensions());
    auto output = gil::view(output_image);
    auto max_derivative_value = 4.0 * 255.0;
    gil::abs_remap_cast(dx, output, 0.0, max_derivative_value);
    gil::write_view("dx_output.png", output, gil::png_tag{});
    gil::abs_remap_cast(dy, output, 0.0, max_derivative_value);
    gil::write_view("dy_output.png", output, gil::png_tag{});

    gil::matrix_storage gradient_storage(input.dimensions());
    gil::gradient(dx, dy, gil::view(gradient_storage));
    gil::remap_cast(gil::view(gradient_storage), output, 0.0, max_derivative_value * std::sqrt(2.0));
    gil::write_view("gradient.png", output, gil::png_tag{});
}
