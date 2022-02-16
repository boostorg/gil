//
// Copyright 2019 Olzhas Zhumabek <anonymous.from.applecity@gmail.com>
// Copyright 2021 Prathamesh Tagore <prathameshtagore@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_GIL_IMAGE_PROCESSING_DETAIL_MATH_HPP
#define BOOST_GIL_IMAGE_PROCESSING_DETAIL_MATH_HPP

#include <boost/gil/extension/numeric/kernel.hpp>
#include <array>
#include <cmath>
#include <vector>

namespace boost { namespace gil { namespace detail {

enum class kernel_type
{
    sobel_dx,
    sobel_dy,
};

static constexpr double pi = 3.14159265358979323846;

static constexpr std::array<float, 9> dx_sobel = {{-1, 0, 1, -2, 0, 2, -1, 0, 1}};
static constexpr std::array<float, 25> dx_sobel2 = {{
    -1,-2,0,2,1,-4,-8,0,8,4,-6,-12,0,12,6,-4,-8,0,8,4,-1,-2,0,2,1
}}; 
// In variable name "dx_sobel2", "2" indicates that the order of Sobel derivative in x-direction 
// is 2.
static constexpr std::array<float, 9> dx_scharr = {{-1, 0, 1, -1, 0, 1, -1, 0, 1}};
static constexpr std::array<float, 9> dy_sobel = {{1, 2, 1, 0, 0, 0, -1, -2, -1}};
static constexpr std::array<float, 25> dy_sobel2 = {{
    -1,-4,-6,-4,-1,-2,-8,-12,-8,-2,0,0,0,0,0,2,8,12,8,2,1,4,6,4,1
}};
// In variable name "dy_sobel2", "2" indicates that the order of Sobel derivative in y-direction 
// is 2.
static constexpr std::array<float, 9> dy_scharr = {{1, 1, 1, 0, 0, 0, -1, -1, -1}};
static const std::vector<std::vector<float>> smoothing_kernel {{1,2,1},{2,4,2},{1,2,1}};

template <typename T, typename Allocator>
inline detail::kernel_2d<T, Allocator> get_identity_kernel()
{
    detail::kernel_2d<T, Allocator> kernel(1, 0, 0);
    kernel[0] = 1;
    return kernel;
}

// Please refer https://stackoverflow.com/a/10032882/14958679 for getting an overview of the
// concept applied for obtaining higher order Sobel kernels.

/// \defgroup KernelGeneration
/// \brief Contains documentation for functions used in kernel generation.
///
/// Separate functions are used for generating only those kernels whose dimensions are greater than
/// 5x5. Smaller kernels are fed directly to the algorithm.
///

/// \addtogroup KernelGeneration
/// @{

/// \brief Produces higher order kernel vector by performing discrete convolution between lower
/// order kernel vector and a smoothing kernel vector(kernel used for suppressing noise).
/// \param kernel1 - First argument for kernel vector convolution.
/// \param kernel2 - Second argument for kernel vector convolution.
/// \tparam T1 - Type of first argument for kernel vector convolution.
/// \tparam T2 - Type of second argument for kernel vector convolution.
template<typename T1, typename T2>
inline auto kernel_convolve_impl(T1 kernel1, T2 kernel2) -> std::vector<std::vector<float>>
{
    std::ptrdiff_t convolved_kernel_size = kernel1.size() + kernel2.size() - 1;
    std::vector<std::vector<float>> convolved_kernel(convolved_kernel_size,
        std::vector<float>(convolved_kernel_size));
    std::vector<std::vector<float>> dummy_kernel(convolved_kernel_size,
        std::vector<float>(convolved_kernel_size));
    
    // 'dummy_kernel' will be made by padding 'kernel1' with appropriate no. of rows and columns 
    // containing zeros to match the size specified by 'convolved_kernel_size'.

    // 'convolved kernel' will store the result obtained after applying convolution between 
    // 'dummy_kernel' and 'kernel2'.
    
    // 'padding_origin' will be used for determining indices of blocks from where padding begins 
    // inside 'dummy_kernel'. It will be used for applying appropriate padding with zeros around 
    // 'kernel1' to create 'dummy_kernel'.
    
    std::ptrdiff_t padding_origin = (kernel2.size() - 1) / 2;
    for (std::ptrdiff_t row = padding_origin;
        row < static_cast<std::ptrdiff_t>(dummy_kernel.size()) - padding_origin; ++row)
    {
        for (std::ptrdiff_t col = padding_origin;
            col < static_cast<std::ptrdiff_t>(dummy_kernel.size()) - padding_origin; ++col)
        {
            dummy_kernel[row][col] = kernel1[row - padding_origin][col - padding_origin];
        }
    }

    std::ptrdiff_t flip_kernel_row, flip_kernel_col, row_boundary, col_boundary;
    float aux_total = 0.0f;
    for (std::ptrdiff_t dummy_row = 0; dummy_row < convolved_kernel_size; ++dummy_row)
    {
        for (std::ptrdiff_t dummy_col = 0; dummy_col < convolved_kernel_size; ++dummy_col)
        {
            aux_total = 0.0f;
            for (std::ptrdiff_t kernel2_row = 0;
                kernel2_row < static_cast<std::ptrdiff_t>(kernel2.size()); ++kernel2_row)
            {
                flip_kernel_row = kernel2.size() - 1 - kernel2_row;
                for (std::ptrdiff_t kernel2_col = 0;
                    kernel2_col < static_cast<std::ptrdiff_t>(kernel2.size()); ++kernel2_col)
                {
                    flip_kernel_col = kernel2.size() - 1 - kernel2_col;
                    row_boundary = dummy_row + kernel2.size()/2 - flip_kernel_row;
                    col_boundary = dummy_col + kernel2.size()/2 - flip_kernel_col;
                    
                    // ignore input samples which are out of bound
                    if (row_boundary >= 0 && row_boundary < convolved_kernel_size &&
                        col_boundary >= 0 && col_boundary < convolved_kernel_size)
                    {
                        aux_total +=
                            kernel2[flip_kernel_row][flip_kernel_col] *
                            dummy_kernel[row_boundary][col_boundary];
                    }
                }
            }
            convolved_kernel[dummy_row][dummy_col] = aux_total;
        }
    }
    return convolved_kernel;
}

/// \brief Fills kernel vector given as argument with a second order kernel in horizontal or
/// vertical direction. The type of the kernel which is to be used for filling will be indicated 
/// by the variable 'type'.
/// \param kernel - Kernel vector which will be filled.
/// \param type - Indicates the type of second order derivative kernel which is to be filled inside
/// first argument.
inline void kernel_vector_fill(std::vector<std::vector<float>>& kernel, kernel_type type)
{
    if (type == kernel_type::sobel_dx)
    {
        for (std::ptrdiff_t row = 0; row < 5; ++row)
        {
            for (std::ptrdiff_t col = 0; col < 5; ++col)
            {
                kernel[row][col] = dx_sobel2[5 * row + col];
            }
        }
    }
    else if (type == kernel_type::sobel_dy)
    {
        for (std::ptrdiff_t row =0; row < 5; ++row)
        {
            for (std::ptrdiff_t col = 0; col < 5; ++col)
            {
                kernel[row][col] = dy_sobel2[5 * row + col];
            }
        }
    }
}

/// \brief Passes parameters to 'kernel_convolve_impl()' repeatedly until kernel vector of desired
/// order is obtained.
/// \param order - Indicates order of derivative whose kernel vector is to be returned.
/// \param type - Indicates the type of kernel vector which is to be returned.
inline auto kernel_convolve(unsigned int order, kernel_type type) -> std::vector<float>
{
    std::vector<float> convolved_kernel_flatten;
    std::vector<std::vector<float>> convolved_kernel(5, std::vector<float>(5));
    
    kernel_vector_fill(convolved_kernel, type);

    std::vector<std::vector<float>> smoothing_dummy = smoothing_kernel;

    // Variable 'smooth_repetition' will store the number of times we need to convolve 
    // 'smoothing_dummy' with itself. This number when used as a power of 2 in its exponentiation,
    // will result in a number which is the largest power of 2 smaller than 'order - 2'.
    double const smooth_repetition = static_cast<unsigned int>(std::log2(order - 2));

    for (unsigned int i = 0; i < smooth_repetition; ++i)
    {
        smoothing_dummy = kernel_convolve_impl(smoothing_dummy, smoothing_dummy);
    }

    convolved_kernel = kernel_convolve_impl(convolved_kernel, smoothing_dummy);

    // Variable 'order_decrease' will store the amount of decrease in order obtained due to the above 
    // optimization. It stores the largest power of 2 smaller than 'order - 2'.
    double const order_decrease = std::pow(2, smooth_repetition);
    for (unsigned int i = 0; i < order - 2 - order_decrease; ++i)
    {
        convolved_kernel = kernel_convolve_impl(convolved_kernel, smoothing_kernel);
    }

    for (std::ptrdiff_t row = 0; row < static_cast<std::ptrdiff_t>(convolved_kernel.size()); ++row)
    {
        for (std::ptrdiff_t col = 0; col < static_cast<std::ptrdiff_t>(convolved_kernel.size());
            ++col)
        {
            convolved_kernel_flatten.push_back(convolved_kernel[row][col]);
        }
    }

    return convolved_kernel_flatten;
}
/// @}
}}} // namespace boost::gil::detail
 
#endif
