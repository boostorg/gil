//
// Copyright 2019 Olzhas Zhumabek <anonymous.from.applecity@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_GIL_IMAGE_PROCESSING_NUMERIC_HPP
#define BOOST_GIL_IMAGE_PROCESSING_NUMERIC_HPP

#include <boost/gil/detail/math.hpp>
#include <cmath>
#include <boost/gil/image_view.hpp>
#include <boost/gil/typedefs.hpp>

namespace boost { namespace gil {

/// \defgroup ImageProcessingMath
/// \brief Math operations for IP algorithms
///
/// This is mostly handful of mathemtical operations that are required by other
/// image processing algorithms
///
/// \brief Normalized cardinal sine
/// \ingroup ImageProcessingMath
///
/// normalized_sinc(x) = sin(pi * x) / (pi * x)
///
inline double normalized_sinc(double x)
{
    return std::sin(x * boost::gil::pi) / (x * boost::gil::pi);
}

/// \brief Lanczos response at point x
/// \ingroup ImageProcessingMath
///
/// Lanczos response is defined as:
/// x == 0: 1
/// -a < x && x < a: 0
/// otherwise: normalized_sinc(x) / normalized_sinc(x / a)
inline double lanczos(double x, std::ptrdiff_t a)
{
    // means == but <= avoids compiler warning
    if (0 <= x && x <= 0)
        return 1;

    if (-a < x && x < a)
        return normalized_sinc(x) / normalized_sinc(x / static_cast<double>(a));

    return 0;
}

<<<<<<< HEAD
inline void compute_tensor_entries(
    boost::gil::gray16_view_t dx,
    boost::gil::gray16_view_t dy,
    boost::gil::gray32f_view_t m11,
    boost::gil::gray32f_view_t m12_21,
    boost::gil::gray32f_view_t m22)
{
    for (std::ptrdiff_t y = 0; y < dx.height(); ++y) {
        for (std::ptrdiff_t x = 0; x < dx.width(); ++x) {
            auto dx_value = dx(x, y);
            auto dy_value = dy(x, y);
            m11(x, y) = dx_value * dx_value;
            m12_21(x, y) = dx_value * dy_value;
            m22(x, y) = dy_value * dy_value;
        }
    }
}

template <typename GradientView, typename OutputView>
inline void compute_hessian_entries(
    GradientView dx,
    GradientView dy,
    OutputView ddxx,
    OutputView dxdy,
    OutputView ddyy)
{
    using x_coord_t = typename OutputView::x_coord_t;
    using y_coord_t = typename OutputView::y_coord_t;
    using pixel_t = typename std::remove_reference<decltype(ddxx(0, 0))>::type;
    using channel_t = typename std::remove_reference<
                        decltype(
                            std::declval<pixel_t>().at(
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
    for (y_coord_t y = 1; y < ddxx.height() - 1; ++y)
    {
        for (x_coord_t x = 1; x < ddxx.width() - 1; ++x)
        {
            pixel_t ddxx_i;
            boost::gil::static_transform(ddxx_i, ddxx_i,
                [](channel_t) { return static_cast<channel_t>(0); });
            pixel_t dxdy_i;
            boost::gil::static_transform(dxdy_i, dxdy_i,
                [](channel_t) { return static_cast<channel_t>(0); });
            pixel_t ddyy_i;
            boost::gil::static_transform(ddyy_i, ddyy_i,
                [](channel_t) { return static_cast<channel_t>(0); });
            for (y_coord_t y_filter = 0; y_filter < 2; ++y_filter)
            {
                for (x_coord_t x_filter = 0; x_filter < 2; ++x_filter)
                {
                    auto adjusted_y = y + y_filter - 1;
                    auto adjusted_x = x + x_filter - 1;
                    ddxx_i.at(std::integral_constant<int, 0>{}) +=
                        dx(adjusted_x, adjusted_y).at(chosen_channel)
                        * x_kernel[y_filter][x_filter];
                    dxdy_i.at(std::integral_constant<int, 0>{}) +=
                        dx(adjusted_x, adjusted_y).at(chosen_channel)
                        * y_kernel[y_filter][x_filter];
                    ddyy_i.at(std::integral_constant<int, 0>{}) +=
                        dy(adjusted_x, adjusted_y).at(chosen_channel)
                        * y_kernel[y_filter][x_filter];
                }
            }
            ddxx(x, y) = ddxx_i;
            dxdy(x, y) = dxdy_i;
            ddyy(x, y) = ddyy_i;
        }
    }
}

/// \brief Generate mean kernel
/// \ingroup ImageProcessingMath
///
/// Fills supplied view with normalized mean
/// in which all entries will be equal to
/// \code 1 / (dst.size()) \endcode
inline void generate_normalized_mean(boost::gil::gray32f_view_t dst)
{
    if (dst.width() != dst.height() || dst.width() % 2 != 1)
        throw std::invalid_argument("kernel dimensions should be odd and equal");
    const float entry = 1.0f / static_cast<float>(dst.size());

    for (auto& pixel: dst) {
        pixel.at(std::integral_constant<int, 0>{}) = entry;
    }
}

/// \brief Generate kernel with all 1s
/// \ingroup ImageProcessingMath
///
/// Fills supplied view with 1s (ones)
inline void generate_unnormalized_mean(boost::gil::gray32f_view_t dst)
{
    if (dst.width() != dst.height() || dst.width() % 2 != 1)
        throw std::invalid_argument("kernel dimensions should be odd and equal");

    for (auto& pixel: dst) {
        pixel.at(std::integral_constant<int, 0>{}) = 1.0f;
    }
}

/// \brief Generate Gaussian kernel
/// \ingroup ImageProcessingMath
///
/// Fills supplied view with values taken from Gaussian distribution. See
/// https://en.wikipedia.org/wiki/Gaussian_blur
inline void generate_gaussian_kernel(boost::gil::gray32f_view_t dst, double sigma)
{
    if (dst.width() != dst.height() || dst.width() % 2 != 1)
        throw std::invalid_argument("kernel dimensions should be odd and equal");

    const double denominator = 2 * boost::gil::pi * sigma * sigma;
    const auto middle = boost::gil::point_t(dst.width() / 2, dst.height() / 2);
    for (boost::gil::gray32f_view_t::coord_t y = 0; y < dst.height(); ++y)
    {
        for (boost::gil::gray32f_view_t::coord_t x = 0; x < dst.width(); ++x)
        {
            const auto delta_x = std::abs(middle.x - x);
            const auto delta_y = std::abs(middle.y - y);
            const double power = (delta_x * delta_x +  delta_y * delta_y) / (2 * sigma * sigma);
            const double nominator = std::exp(-power);
            const float value = nominator / denominator;
            dst(x, y).at(std::integral_constant<int, 0>{}) = value;
        }
    }
}

}} // namespace boost::gil

#endif
