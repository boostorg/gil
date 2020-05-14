#include <blaze/Blaze.h>

#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/pixel.hpp>

#include <cstddef>
#include <utility>

namespace boost { namespace gil {
namespace detail
{
template <typename PixelType, std::size_t... indices>
auto pixel_to_vector_impl(const PixelType& pixel, std::integer_sequence<std::size_t, indices...>)
{
    using ChannelType = typename boost::gil::channel_type<PixelType>::type;
    return blaze::StaticVector<ChannelType, sizeof...(indices)>{pixel[indices]...};
}
} // namespace detail

template <typename PixelType>
auto pixel_to_vector(const PixelType& pixel)
{
    return detail::pixel_to_vector_impl(
        pixel, std::make_index_sequence<boost::gil::num_channels<PixelType>{}>{});
}

template <typename View>
auto to_matrix_channeled(View view)
{
    return blaze::evaluate(
        blaze::generate(view.height(), view.width(), [&view](std::size_t i, std::size_t j)
        {
            return pixel_to_vector(view(j, i));
        }));
}

template <typename GrayView>
auto to_matrix(GrayView source)
{
    using value_type = std::remove_cv_t<std::remove_reference_t<decltype(
        std::declval<typename GrayView::value_type>().at(std::integral_constant<int, 0>{}))>>;
    return blaze::CustomMatrix<value_type, blaze::unaligned, blaze::unpadded>(
        reinterpret_cast<value_type*>(&source(0, 0)), source.height(), source.width());
}

inline boost::gil::gray8_image_t to_gray8_image(const blaze::DynamicMatrix<std::uint8_t>& source)
{
    boost::gil::gray8_image_t result(source.columns(), source.rows());
    auto matrix_view = to_matrix(boost::gil::view(result));
    matrix_view = source;
    return result;
}

template <typename PixelType, typename VT, bool TransposeFlag>
auto vector_to_pixel(const blaze::DenseVector<VT, TransposeFlag>& vector)
{
    auto num_channels = boost::gil::num_channels<PixelType>{};
    PixelType pixel{};
    for (std::size_t i = 0; i < num_channels; ++i)
    {
        pixel[i] = (~vector)[i];
    }

    return pixel;
}

template <typename ImageType, typename PixelType = typename ImageType::value_type, typename MT>
ImageType to_image(const blaze::DenseMatrix<MT, blaze::rowMajor>& data)
{
    ImageType image((~data).rows(), (~data).columns());
    auto view = boost::gil::view(image);

    for (std::ptrdiff_t i = 0; i < view.height(); ++i)
    {
        for (std::ptrdiff_t j = 0; j < view.width(); ++j)
        {
            view(j, i) = vector_to_pixel<PixelType>((~data)(i, j));
        }
    }

    return image;
}
}}
