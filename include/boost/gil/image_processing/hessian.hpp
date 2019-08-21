#ifndef BOOST_GIL_IMAGE_PROCESSING_HESSIAN_HPP
#define BOOST_GIL_IMAGE_PROCESSING_HESSIAN_HPP

#include <boost/gil/image_view.hpp>
#include <boost/gil/typedefs.hpp>
#include <stdexcept>

namespace boost { namespace gil {

/// \brief Computes Hessian response
///
/// Computes Hessian response based on computed entries of Hessian matrix, e.g. second order
/// derivates in x and y, and derivatives in both x, y.
/// d stands for derivative, and x or y stand for derivative direction. For example,
/// ddxx means taking two derivatives (gradients) in horizontal direction.
/// Weights change perception of surroinding pixels.
/// Additional filtering is strongly advised.
template <typename GradientView, typename Weights, typename OutputView>
inline void compute_hessian_responses(
    GradientView ddxx,
    GradientView dxdy,
    GradientView ddyy,
    Weights weights,
    OutputView dst)
{
    if (ddxx.dimensions() != ddyy.dimensions()
        || ddyy.dimensions() != dxdy.dimensions()
        || dxdy.dimensions() != dst.dimensions()
        || weights.width() != weights.height()
        || weights.width() % 2 != 1)
    {
        throw std::invalid_argument("dimensions of views are not the same"
            " or weights don't have equal width and height"
            " or weights' dimensions are not odd");
    }
    // Use pixel type of output, as values will be written to output
    using pixel_t = typename std::remove_reference<decltype(std::declval<OutputView>()(0, 0))>::type;

    using channel_t = typename std::remove_reference
        <
            decltype(std::declval<pixel_t>().at(std::integral_constant<int, 0>{}))
        >::type;

    auto half_size = weights.width() / 2;
    for (auto y = half_size; y < dst.height() - half_size; ++y)
    {
        for (auto x = half_size; x < dst.width() - half_size; ++x)
        {
            auto ddxx_i = channel_t();
            auto ddyy_i = channel_t();
            auto dxdy_i = channel_t();
            constexpr std::integral_constant<int, 0> chosen_channel{};
            for (typename OutputView::coord_t w_y = 0; w_y < weights.height(); ++w_y)
            {
                for (typename OutputView::coord_t w_x = 0; w_x < weights.width(); ++w_x)
                {
                    ddxx_i += ddxx(x + w_x - half_size, y + w_y - half_size)
                        .at(std::integral_constant<int, 0>{})
                         * weights(w_x, w_y).at(std::integral_constant<int, 0>{});
                    ddyy_i += ddyy(x + w_x - half_size, y + w_y - half_size)
                        .at(std::integral_constant<int, 0>{})
                         * weights(w_x, w_y).at(std::integral_constant<int, 0>{});
                    dxdy_i += dxdy(x + w_x - half_size, y + w_y - half_size)
                        .at(std::integral_constant<int, 0>{})
                         * weights(w_x, w_y).at(std::integral_constant<int, 0>{});
                }
            }
            auto determinant = ddxx_i * ddyy_i - dxdy_i * dxdy_i;
            dst(x, y).at(std::integral_constant<int, 0>{}) = determinant;
        }
    }
}

}} // namespace boost::gil

#endif
