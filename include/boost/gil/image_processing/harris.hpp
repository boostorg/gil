#ifndef BOOST_GIL_IMAGE_PROCESSING_HARRIS_HPP
#define BOOST_GIL_IMAGE_PROCESSING_HARRIS_HPP

#include <boost/gil/image_view.hpp>
#include <boost/gil/typedefs.hpp>

namespace boost { namespace gil {
/// \defgroup CornerDetectionAlgorithms
/// \brief Algorithms that are used to find corners in an image
///
/// These algorithms are used to find spots from which
/// sliding the window will produce large intensity change


/// \brief function to record Harris responses
/// \ingroup CornerDetectionAlgorithms
///
/// This algorithm computes Harris responses
/// for structure tensor represented by m11, m12_21, m22 views.
/// Note that m12_21 represents both entries (1, 2) and (2, 1).
/// Window length represents size of a window which is slided around
/// to compute sum of corresponding entries. k is a discrimination
/// constant against edges (usually in range 0.04 to 0.06).
/// harris_response is an out parameter that will contain the Harris responses.
void compute_harris_responses(
    boost::gil::gray32f_view_t m11,
    boost::gil::gray32f_view_t m12_21,
    boost::gil::gray32f_view_t m22,
    boost::gil::gray32f_view_t weights,
    float k,
    boost::gil::gray32f_view_t harris_response)
{
    if (m11.dimensions() != m12_21.dimensions() || m12_21.dimensions() != m22.dimensions()) {
        throw std::invalid_argument("m prefixed arguments must represent"
            " tensor from the same image");
    }

    auto const window_length = weights.dimensions().x;
    auto const width = m11.width();
    auto const height = m11.height();
    auto const half_length = window_length / 2;

    for (auto y = half_length; y < height - half_length; ++y)
    {
        for (auto x = half_length; x < width - half_length; ++x)
        {
            float ddxx = 0;
            float dxdy = 0;
            float ddyy = 0;
            for (gil::gray32f_view_t::coord_t y_kernel = 0;
                y_kernel < window_length;
                ++y_kernel) {
                for (gil::gray32f_view_t::coord_t x_kernel = 0;
                    x_kernel < window_length;
                    ++x_kernel) {
                    ddxx += m11(x + x_kernel - half_length, y + y_kernel - half_length)
                        .at(std::integral_constant<int, 0>{})
                        * weights(x_kernel, y_kernel).at(std::integral_constant<int, 0>{});
                    dxdy += m12_21(x + x_kernel - half_length, y + y_kernel - half_length)
                        .at(std::integral_constant<int, 0>{})
                        * weights(x_kernel, y_kernel).at(std::integral_constant<int, 0>{});
                    ddyy += m22(x + x_kernel - half_length, y + y_kernel - half_length)
                        .at(std::integral_constant<int, 0>{})
                        * weights(x_kernel, y_kernel).at(std::integral_constant<int, 0>{});
                }
            }
            auto det = (ddxx * ddyy) - dxdy * dxdy;
            auto trace = ddxx + ddyy;
            auto harris_value = det - k * trace * trace;
            harris_response(x, y).at(std::integral_constant<int, 0>{}) = harris_value;
        }
    }
}

}} //namespace boost::gil
#endif
