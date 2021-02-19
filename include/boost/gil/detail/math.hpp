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

#include <array>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <vector>

namespace boost { namespace gil { namespace detail {

enum class kernel_type
{
    sobeldx,
    sobeldy,
};

static constexpr double pi = 3.14159265358979323846;

static constexpr std::array<float, 9> dx_sobel = {{-1, 0, 1, -2, 0, 2, -1, 0, 1}};
static constexpr std::array<float, 25> dx_sobel2 = {{
    -1,-2,0,2,1,-4,-8,0,8,4,-6,-12,0,12,6,-4,-8,0,8,4,-1,-2,0,2,1
}}; 
static constexpr std::array<float, 9> dx_scharr = {{-1, 0, 1, -1, 0, 1, -1, 0, 1}};
static constexpr std::array<float, 9> dy_sobel = {{1, 2, 1, 0, 0, 0, -1, -2, -1}};
static constexpr std::array<float, 25> dy_sobel2 = {{
    -1,-4,-6,-4,-1,-2,-8,-12,-8,-2,0,0,0,0,0,2,8,12,8,2,1,4,6,4,1
}};
static constexpr std::array<float, 9> dy_scharr = {{1, 1, 1, 0, 0, 0, -1, -1, -1}};
static std::vector<std::vector<float>> smoothing_kernel {{1,2,1},{2,4,2},{1,2,1}};

template <typename T, typename Allocator>
inline detail::kernel_2d<T, Allocator> get_identity_kernel()
{
    detail::kernel_2d<T, Allocator> kernel(1, 0, 0);
    kernel[0] = 1;
    return kernel;
}

// Please refer https://stackoverflow.com/a/10032882/14958679 for getting an overview of the
// concept applied for obtaining higher order sobel kernels.

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
std::vector<std::vector<float>> kernel_convolve_impl(T1 kernel1, T2 kernel2)
{
    size_t convolved_kernel_size = kernel1.size() + kernel2.size() - 1;
    std::vector<std::vector<float>> convolved_kernel(convolved_kernel_size,
                                                         std::vector<float>(convolved_kernel_size)),
                                    dummy_kernel(convolved_kernel_size,
                                                         std::vector<float>(convolved_kernel_size));
    
    // 'dummy_kernel' will be made by padding 'kernel1' with appropriate no. of rows and columns 
    // containing zeros to match the size specified by 'convolved_kernel_size'.
    // 'convolved kernel' will store the result obtained after applying convolution between 
    // 'dummy_kernel' and 'kernel2'.
    // 'padding_reference' will be used to apply appropriate padding with zeros around 'kernel1'
    // to create 'dummy_kernel'.
    
    std::ptrdiff_t padding_reference = (kernel2.size() - 1) / 2;
    for(std::ptrdiff_t row = padding_reference;row < dummy_kernel.size() - padding_reference; ++row)
        for(std::ptrdiff_t col = padding_reference;col < dummy_kernel.size() - padding_reference;
                                                                                            ++col)
        {
            dummy_kernel[row][col] = kernel1[row - padding_reference][col - padding_reference];
        }

    std::ptrdiff_t flip_ker_row, flip_ker_col, row_boundary, col_boundary;
    float aux_total;
    for(std::ptrdiff_t dummy_row = 0;dummy_row < convolved_kernel_size; ++dummy_row)
    {
        for(std::ptrdiff_t dummy_col = 0;dummy_col < convolved_kernel_size; ++dummy_col)
        {
            aux_total = 0.0f;
            for(std::ptrdiff_t ker2_row = 0;ker2_row < kernel2.size(); ++ker2_row)
            {
                flip_ker_row = kernel2.size() - 1 - ker2_row;
                for(std::ptrdiff_t ker2_col = 0;ker2_col < kernel2.size(); ++ker2_col)
                {
                    flip_ker_col = kernel2.size() - 1 - ker2_col;
                    row_boundary = dummy_row + kernel2.size()/2 - flip_ker_row;
                    col_boundary = dummy_col + kernel2.size()/2 - flip_ker_col;
                    
                    // ignore input samples which are out of bound
                    if (row_boundary >= 0 && row_boundary < convolved_kernel_size &&
                        col_boundary >= 0 && col_boundary < convolved_kernel_size)
                    {
                        aux_total +=
                            kernel2[flip_ker_row][flip_ker_col] *
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
void kernel_fill(std::vector<std::vector<float>>& kernel, kernel_type type)                                                                                  
{
    if(type == kernel_type::sobeldx)
    {
        for(std::ptrdiff_t row = 0;row < 5; ++row)
            for(std::ptrdiff_t col = 0;col < 5; ++col)
                kernel[row][col] = dx_sobel2[5 * row + col];
    }
    else if(type == kernel_type::sobeldy)
    {
        for(std::ptrdiff_t row =0;row < 5; ++row)
            for(std::ptrdiff_t col = 0;col < 5; ++col)
                kernel[row][col] = dy_sobel2[5 * row + col];
    }
}

/// \brief Passes parameters to 'kernel_convolve_impl()' repeatedly until kernel vector of desired
/// order is obtained.
/// \param order - Indicates order of derivative whose kernel vector is to be returned.
/// \param type - Indicates the type of kernel vector which is to be returned.
std::vector<float> kernel_convolve(unsigned int order, kernel_type type)
{
    std::vector<float> convolved_kernel_flatten;
    std::vector<std::vector<float>> convolved_kernel(5, std::vector<float>(5));
    
    kernel_fill(convolved_kernel, type);
    
    for(unsigned int i = 0;i < order - 2; ++i)
        convolved_kernel = kernel_convolve_impl(convolved_kernel, smoothing_kernel);

    for(std::ptrdiff_t row = 0;row < convolved_kernel.size(); ++row)
        for(std::ptrdiff_t col = 0;col < convolved_kernel.size(); ++col)
            convolved_kernel_flatten.push_back(convolved_kernel[row][col]);
            
    return convolved_kernel_flatten;
}
/// @}
}}} // namespace boost::gil::detail

#endif
