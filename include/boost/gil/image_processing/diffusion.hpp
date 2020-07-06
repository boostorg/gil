//
// Copyright 2020 Olzhas Zhumabek <anonymous.from.applecity@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "boost/gil/detail/math.hpp"
#include <boost/gil/algorithm.hpp>
#include <boost/gil/color_base_algorithm.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/gil/point.hpp>
#include <boost/gil/typedefs.hpp>
#include <functional>
#include <numeric>
#include <vector>

namespace boost { namespace gil {
namespace diffusion {
struct perona_malik_diffusivity
{
    double kappa;
    template <typename Pixel>
    Pixel operator()(Pixel input)
    {
        using channel_type = typename channel_type<Pixel>::type;
        // C++11 doesn't seem to capture members
        auto local_kappa = kappa;
        static_transform(input, input, [local_kappa](channel_type value) {
            value /= local_kappa;
            return std::exp(-std::abs(value));
        });

        return input;
    }
};

struct gaussian_diffusivity
{
    double kappa;
    template <typename Pixel>
    Pixel operator()(Pixel input)
    {
        using channel_type = typename channel_type<Pixel>::type;
        // C++11 doesn't seem to capture members
        auto local_kappa = kappa;
        static_transform(input, input, [local_kappa](channel_type value) {
            value /= local_kappa;
            return std::exp(-value * value);
        });

        return input;
    }
};

struct wide_regions_diffusivity
{
    double kappa;
    template <typename Pixel>
    Pixel operator()(Pixel input)
    {
        using channel_type = typename channel_type<Pixel>::type;
        // C++11 doesn't seem to capture members
        auto local_kappa = kappa;
        static_transform(input, input, [local_kappa](channel_type value) {
            value /= local_kappa;
            return 1.0 / (1.0 + value * value);
        });

        return input;
    }
};

struct more_wide_regions_diffusivity
{
    double kappa;
    template <typename Pixel>
    Pixel operator()(Pixel input)
    {
        using channel_type = typename channel_type<Pixel>::type;
        // C++11 doesn't seem to capture members
        auto local_kappa = kappa;
        static_transform(input, input, [local_kappa](channel_type value) {
            value /= local_kappa;
            return 1.0 / std::sqrt((1.0 + value * value));
        });

        return input;
    }
};
} // namespace diffusion

namespace laplace_function {
template <typename PixelType>
using stencil_type = std::array<PixelType, 8>;

struct stencil_4points
{
    double delta_t = 0.25;

    template <typename SubImageView>
    stencil_type<typename SubImageView::value_type> compute_laplace(SubImageView view,
                                                                    point_t origin)
    {
        stencil_type<typename SubImageView::value_type> stencil;
        auto out = stencil.begin();
        auto current = view(origin);
        using channel_type = typename channel_type<typename SubImageView::value_type>::type;
        std::array<gil::point_t, 4> offsets{point_t{0, -1}, point_t{-1, 0}, point_t{+1, 0},
                                            point_t{0, 1}};
        for (auto offset : offsets)
        {
            ++out; // skip 45 degree values;
            static_transform(view(origin.x + offset.x, origin.y + offset.y), current, *out++,
                             std::minus<channel_type>{});
        }
    }

    template <typename Pixel>
    Pixel reduce(const stencil_type<Pixel>& stencil)
    {
        auto first = stencil.begin();
        auto last = stencil.end();
        using channel_type = typename channel_type<Pixel>::value_type;
        auto result = []() {
            Pixel zero_pixel;
            static_fill(zero_pixel, channel_type(0));
            return zero_pixel;
        }();

        for (; first != last; ++first)
        {
            static_transform(result, *first, result, std::plus<channel_type>{});
        }
        Pixel delta_t_pixel;
        static_fill(delta_t_pixel, delta_t);
        static_transform(result, delta_t_pixel, result, std::multiplies<channel_type>{});

        return result;
    }
};

struct stencil_8points_standard
{
    double delta_t = 0.125;

    template <typename SubImageView>
    stencil_type<typename SubImageView::value_type> compute_laplace(SubImageView view,
                                                                    point_t origin)
    {
        stencil_type<typename SubImageView::value_type> stencil;
        auto out = stencil.begin();
        auto current = view(origin);
        using channel_type = typename channel_type<typename SubImageView::value_type>::type;
        std::array<gil::point_t, 8> offsets{point_t{-1, -1}, point_t{0, -1}, point_t{+1, -1},
                                            point_t{-1, 0},  point_t{+1, 0}, point_t{-1, +1},
                                            point_t{0, +1},  point_t{+1, +1}};
        for (auto offset : offsets)
        {
            static_transform(view(origin.x + offset.x, origin.y + offset.y), current, *out++,
                             std::minus<channel_type>{});
        }

        return stencil;
    }

    template <typename Pixel>
    Pixel reduce(const stencil_type<Pixel>& stencil)
    {
        auto first = stencil.begin();
        auto last = stencil.end();
        using channel_type = typename channel_type<Pixel>::type;
        auto result = []() {
            Pixel zero_pixel;
            static_fill(zero_pixel, channel_type(0));
            return zero_pixel;
        }();
        auto half = std::next(first, 4);
        for (; first != half; ++first)
        {
            static_transform(result, *first, result, std::plus<channel_type>{});
        }

        for (first = half; first != last; ++first)
        {
            Pixel half_pixel;
            static_fill(half_pixel, channel_type(1 / 2.0));
            static_transform(*first, half_pixel, half_pixel, std::multiplies<channel_type>{});
            static_transform(result, half_pixel, result, std::plus<channel_type>{});
        }

        Pixel delta_t_pixel;
        static_fill(delta_t_pixel, delta_t);
        static_transform(result, delta_t_pixel, result, std::multiplies<channel_type>{});

        return result;
    }
};
} // namespace laplace_function

namespace brightness_function {
using laplace_function::stencil_type;
struct identity
{
    template <typename Pixel>
    stencil_type<Pixel> operator()(const stencil_type<Pixel>& stencil)
    {
        return stencil;
    }
};

// TODO: Figure out how to implement color gradient brightness, as it
// seems to need dx and dy using sobel or scharr kernels

// TODO: Implement luminance based brightness function

} // namespace brightness_function

enum class matlab_connectivity
{
    minimal,
    maximal
};

enum class matlab_conduction_method
{
    exponential,
    quadratic
};

template <typename InputView, typename OutputView>
void classic_anisotropic_diffusion(const InputView& input, const OutputView& output,
                                   unsigned int num_iter, double kappa)
{
    anisotropic_diffusion(input, output, num_iter, laplace_function::stencil_4points{},
                          brightness_function::identity{},
                          diffusion::perona_malik_diffusivity{kappa});
}

template <typename InputView, typename OutputView>
void matlab_anisotropic_diffusion(const InputView& input, const OutputView& output,
                                  unsigned int num_iter, double kappa,
                                  matlab_connectivity connectivity,
                                  matlab_conduction_method conduction_method)
{
    if (connectivity == matlab_connectivity::minimal)
    {
        if (conduction_method == matlab_conduction_method::exponential)
        {
            anisotropic_diffusion(input, output, num_iter, laplace_function::stencil_4points{},
                                  brightness_function::identity{},
                                  diffusion::gaussian_diffusivity{kappa});
        }
        else if (conduction_method == matlab_conduction_method::quadratic)
        {
            anisotropic_diffusion(input, output, num_iter, laplace_function::stencil_4points{},
                                  brightness_function::identity{},
                                  diffusion::gaussian_diffusivity{kappa});
        }
        else
        {
            throw std::logic_error("unhandled conduction method found");
        }
    }
    else if (connectivity == matlab_connectivity::maximal)
    {
        if (conduction_method == matlab_conduction_method::exponential)
        {
            anisotropic_diffusion(input, output, num_iter, laplace_function::stencil_4points{},
                                  brightness_function::identity{},
                                  diffusion::gaussian_diffusivity{kappa});
        }
        else if (conduction_method == matlab_conduction_method::quadratic)
        {
            anisotropic_diffusion(input, output, num_iter, laplace_function::stencil_4points{},
                                  brightness_function::identity{},
                                  diffusion::gaussian_diffusivity{kappa});
        }
        else
        {
            throw std::logic_error("unhandled conduction method found");
        }
    }
    else
    {
        throw std::logic_error("unhandled connectivity found");
    }
}

template <typename InputView, typename OutputView>
void default_anisotropic_diffusion(const InputView& input, const OutputView& output,
                                   unsigned int num_iter, double kappa)
{
    anisotropic_diffusion(input, output, num_iter, laplace_function::stencil_8points_standard{},
                          brightness_function::identity{}, diffusion::gaussian_diffusivity{kappa});
}

/// \brief Performs diffusion according to Perona-Malik equation
///
/// WARNING: Output channel type must be floating point,
/// otherwise there will be loss in accuracy which most
/// probably will lead to incorrect results (input will be unchanged).
/// Anisotropic diffusion is a smoothing algorithm that respects
/// edge boundaries and can work as an edge detector if suitable
/// iteration count is set and grayscale image view is used
/// as an input
template <typename InputView, typename OutputView,
          typename LaplaceStrategy = laplace_function::stencil_8points_standard,
          typename BrightnessFunction = brightness_function::identity,
          typename DiffusivityFunction = diffusion::gaussian_diffusivity>
void anisotropic_diffusion(const InputView& input, const OutputView& output, unsigned int num_iter,
                           LaplaceStrategy laplace, BrightnessFunction brightness,
                           DiffusivityFunction diffusivity)
{
    using input_pixel_type = typename InputView::value_type;
    using pixel_type = typename OutputView::value_type;
    using channel_type = typename channel_type<pixel_type>::type;
    using computation_image = image<pixel_type>;
    const auto width = input.width();
    const auto height = input.height();
    const point_t dims(width, height);
    const auto zero_pixel = []() {
        pixel_type pixel;
        static_fill(pixel, static_cast<channel_type>(0));

        return pixel;
    }();
    computation_image result_image(width + 2, height + 2, zero_pixel);
    auto result = view(result_image);
    computation_image scratch_result_image(width + 2, height + 2, zero_pixel);
    auto scratch_result = view(scratch_result_image);
    transform_pixels(input, subimage_view(result, 1, 1, width, height),
                     [](const input_pixel_type& pixel) {
                         pixel_type converted;
                         for (std::size_t i = 0; i < num_channels<pixel_type>{}; ++i)
                         {
                             converted[i] = pixel[i];
                         }
                         return converted;
                     });

    for (unsigned int iteration = 0; iteration < num_iter; ++iteration)
    {
        for (std::ptrdiff_t relative_y = 0; relative_y < height; ++relative_y)
        {
            for (std::ptrdiff_t relative_x = 0; relative_x < width; ++relative_x)
            {
                auto x = relative_x + 1;
                auto y = relative_y + 1;
                auto stencil = laplace.compute_laplace(result, point_t(x, y));
                auto brightness_stencil = brightness(stencil);
                laplace_function::stencil_type<pixel_type> diffusivity_stencil;
                std::transform(brightness_stencil.begin(), brightness_stencil.end(),
                               diffusivity_stencil.begin(), diffusivity);
                laplace_function::stencil_type<pixel_type> product_stencil;
                std::transform(stencil.begin(), stencil.end(), diffusivity_stencil.begin(),
                               product_stencil.begin(), [](pixel_type lhs, pixel_type rhs) {
                                   static_transform(lhs, rhs, lhs, std::multiplies<channel_type>{});
                                   return lhs;
                               });
                static_transform(result(x, y), laplace.reduce(product_stencil),
                                 scratch_result(x, y), std::plus<channel_type>{});
            }
        }
        using std::swap;
        swap(result, scratch_result);
    }

    copy_pixels(subimage_view(result, 1, 1, width, height), output);
}

}} // namespace boost::gil
