#include <blaze/Blaze.h>

#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/pixel.hpp>

#include <cstddef>
#include <utility>

namespace boost { namespace gil {
/** \brief Used to bypass scoped_channel_type of GIL

    Some channel types in GIL, like `gil::float32_t` and `gil::float64_t` have a different type
    compared to it's underlying float type (because they constrain the range to be [0...1]),
    this struct will strip that and provide typedef equal to the underlying channel type.
*/
template <typename ChannelType>
struct true_channel_type {
    using type = ChannelType;
};

/// \cond specializations
template <>
struct true_channel_type<boost::gil::float32_t> {
    using type = float;
};

template <>
struct true_channel_type<boost::gil::float64_t> {
    using type = double;
};
/// \endcond

template <typename ChannelType>
using true_channel_type_t = typename true_channel_type<ChannelType>::type;

template <typename Pixel, bool IsRowVector = blaze::rowVector>
struct pixel_vector;

template <typename ChannelValue, typename Layout, bool IsRowVector>
struct pixel_vector<boost::gil::pixel<ChannelValue, Layout>, IsRowVector>
    : boost::gil::pixel<true_channel_type_t<ChannelValue>, Layout>,
      blaze::DenseVector<pixel_vector<boost::gil::pixel<ChannelValue, Layout>, IsRowVector>,
                         IsRowVector> {
    using parent_t = boost::gil::pixel<ChannelValue, Layout>;
    using boost::gil::pixel<ChannelValue, Layout>::pixel;

    using This = pixel_vector<boost::gil::pixel<ChannelValue, Layout>, IsRowVector>;
    using Basetype = blaze::DenseVector<This, IsRowVector>;
    using ResultType = This;

    using TransposeType = pixel_vector<boost::gil::pixel<ChannelValue, Layout>, !IsRowVector>;

    static constexpr bool simdEnabled = false;

    using ElementType = ChannelValue;
    using TagType = blaze::Group0;
    using ReturnType = const ChannelValue&;
    using CompositeType = const This&;

    using Reference = ChannelValue&;
    using ConstReference = const ChannelValue&;
    using Pointer = ChannelValue*;
    using ConstPointer = const ChannelValue*;

    // TODO: rule of 5
    template <typename OtherPixel, bool AnotherIsRowVector>
    pixel_vector(pixel_vector<OtherPixel, AnotherIsRowVector> other) : parent_t(other)
    {
    }

    template <typename OtherPixel>
    pixel_vector& operator=(const pixel_vector<OtherPixel, IsRowVector> other)
    {
        parent_t::operator=(other);
        return *this;
    }

    template <typename VT>
    pixel_vector& operator=(const blaze::DenseVector<VT, IsRowVector>& v)
    {
        if ((~v).size() != size()) {
            throw std::invalid_argument(
                "incoming vector has incompatible size with this pixel vector");
        }

        for (std::size_t i = 0; i < size(); ++i) {
            (*this)[i] = (~v)[i];
        }

        return *this;
    }

  private:
    template <typename OtherPixel, typename Op>
    void perform_op(pixel_vector<OtherPixel, IsRowVector> p, Op op)
    {
        constexpr auto num_channels = boost::gil::num_channels<parent_t>{};
        static_assert(num_channels == boost::gil::num_channels<OtherPixel>{});
        auto& current = *this;
        for (std::ptrdiff_t i = 0; i < num_channels; ++i) {
            current[i] = op(current[i], p[i]);
        }
    }

  public:
    std::size_t size() const { return boost::gil::num_channels<parent_t>{}; }

    constexpr bool canAlias() const { return true; }

    template <typename Other>
    bool isAliased(const Other* alias) const noexcept
    {
        return static_cast<const void*>(this) == static_cast<const void*>(alias);
    }

    template <typename Other>
    bool canAlias(const Other* alias) const noexcept
    {
        return static_cast<const void*>(this) == static_cast<const void*>(alias);
    }

    // TODO: Add SFINAE for cases when parent_t::is_mutable is false
    template <typename OtherPixel>
    pixel_vector& operator+=(pixel_vector<OtherPixel, IsRowVector> other)
    {
        perform_op(other, std::plus<>{});
        return *this;
    }

    template <typename OtherPixel>
    pixel_vector& operator-=(pixel_vector<OtherPixel, IsRowVector> other)
    {
        perform_op(other, std::minus<>{});
        return *this;
    }

    template <typename OtherPixel>
    pixel_vector& operator*=(pixel_vector<OtherPixel, IsRowVector> other)
    {
        perform_op(other, std::multiplies<>{});
        return *this;
    }

    template <typename OtherPixel>
    pixel_vector& operator/=(pixel_vector<OtherPixel, IsRowVector> other)
    {
        perform_op(other, std::divides<>{});
        return *this;
    }
};

template <typename ChannelValue1, typename ChannelValue2, typename Layout, bool IsRowVector>
inline pixel_vector<boost::gil::pixel<std::common_type_t<ChannelValue1, ChannelValue2>, Layout>,
                    IsRowVector>
operator+(pixel_vector<boost::gil::pixel<ChannelValue1, Layout>, IsRowVector> lhs,
          pixel_vector<boost::gil::pixel<ChannelValue2, Layout>, IsRowVector> rhs)
{
    lhs += rhs;
    return lhs;
}

template <typename ChannelValue1, typename ChannelValue2, typename Layout, bool IsRowVector>
inline pixel_vector<boost::gil::pixel<std::common_type_t<ChannelValue1, ChannelValue2>, Layout>,
                    IsRowVector>
operator-(pixel_vector<boost::gil::pixel<ChannelValue1, Layout>, IsRowVector> lhs,
          pixel_vector<boost::gil::pixel<ChannelValue2, Layout>, IsRowVector> rhs)
{
    lhs -= rhs;
    return lhs;
}

template <typename ChannelValue1, typename ChannelValue2, typename Layout, bool IsRowVector>
inline pixel_vector<boost::gil::pixel<std::common_type_t<ChannelValue1, ChannelValue2>, Layout>,
                    IsRowVector>
operator*(pixel_vector<boost::gil::pixel<ChannelValue1, Layout>, IsRowVector> lhs,
          pixel_vector<boost::gil::pixel<ChannelValue2, Layout>, IsRowVector> rhs)
{
    lhs *= rhs;
    return lhs;
}

template <typename ChannelValue1, typename ChannelValue2, typename Layout, bool IsRowVector>
inline pixel_vector<boost::gil::pixel<std::common_type_t<ChannelValue1, ChannelValue2>, Layout>,
                    IsRowVector>
operator/(pixel_vector<boost::gil::pixel<ChannelValue1, Layout>, IsRowVector> lhs,
          pixel_vector<boost::gil::pixel<ChannelValue2, Layout>, IsRowVector> rhs)
{
    lhs /= rhs;
    return lhs;
}

template <typename ChannelValue1, typename ChannelValue2, typename Layout, bool IsRowVector>
inline bool operator==(pixel_vector<boost::gil::pixel<ChannelValue1, Layout>, IsRowVector> lhs,
          pixel_vector<boost::gil::pixel<ChannelValue2, Layout>, IsRowVector> rhs)
{
    for (std::size_t i = 0; i < lhs.size(); ++i)
    {
        if (lhs[i] != rhs[i])
        {
            return false;
        }
    }
    return true;
}

template <typename ChannelValue1, typename ChannelValue2, typename Layout, bool IsRowVector>
inline bool operator!=(pixel_vector<boost::gil::pixel<ChannelValue1, Layout>, IsRowVector> lhs,
          pixel_vector<boost::gil::pixel<ChannelValue2, Layout>, IsRowVector> rhs)
{
    return !(lhs == rhs);
}

template <typename Pixel, bool IsRowVector, typename Scalar>
inline bool operator==(const pixel_vector<Pixel, IsRowVector>& v, Scalar s)
{
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        if (v[i] != s)
        {
            return false;
        }
    }

    return true;
}

template <typename Pixel, bool IsRowVector, typename Scalar>
inline bool operator!=(const pixel_vector<Pixel, IsRowVector>& v, Scalar s)
{
    return !(v == s);
}

template <typename Pixel>
struct corresponding_vector_type {
    using type =
        blaze::StaticVector<true_channel_type_t<typename boost::gil::channel_type<Pixel>::type>,
                            boost::gil::num_channels<Pixel>{}, blaze::rowMajor, blaze::unaligned,
                            blaze::unpadded>;
};

template <typename Pixel>
struct is_layout_compatible {
    static constexpr bool value =
        sizeof(Pixel) == sizeof(typename corresponding_vector_type<Pixel>::type);
};

template <typename Pixel, bool IsRowVector = blaze::rowVector>
struct layout_compatible_vector {
    using type = std::conditional_t<is_layout_compatible<Pixel>::value,
                                    typename corresponding_vector_type<Pixel>::type,
                                    pixel_vector<Pixel, IsRowVector>>;
};

/** \brief Extract `channel`th value from each pixel in `view` and writes into `result`

   The function can be used with multi channel views, just the `channel` argument might need to be
   adjusted to select appropriate channel. Do note that it will strip wrapper type around floating
   point types like gil::float32_t, etc.

    \arg channel The channel to extract from pixels, defaults to 0
    \tparam View The source view type to convert into matrix
    \arg view The source view to convert into matrix
    \tparam MT The concrete type of matrix
    \arg result The out argument to write result into
    \tparam SO Storage order flag
*/
template <typename View, typename MT, bool SO>
void to_matrix(View view, blaze::DenseMatrix<MT, SO>& result, std::size_t channel = 0)
{
    constexpr auto num_channels = boost::gil::num_channels<View>{};
    if (channel >= num_channels) {
        throw std::invalid_argument("channel index exceeds available channels in the view");
    }
    (~result) = blaze::generate(
        view.height(), view.width(), [&view, channel](std::size_t i, std::size_t j) {
            using element_type = blaze::UnderlyingElement_t<MT>;
            return static_cast<element_type>(view(j, i)[channel]);
        });
}

/** \brief Converts an image view into `DynamicMatrix<ChannelType>`, where each entry corresponds to
   selected channel value of the pixel entry

   The function can be used with multi channel views, just the `channel` argument might need to be
   adjusted to select appropriate channel. Do note that it will strip wrapper type around floating
   point types like gil::float32_t, etc.

    \arg channel The channel to extract from pixels, defaults to 0
    \tparam View The source view type to convert into matrix
    \arg view The source view to convert into matrix
*/
template <typename View>
auto to_matrix(View view, std::size_t channel = 0)
{
    constexpr auto num_channels = boost::gil::num_channels<View>{};
    if (channel >= num_channels) {
        throw std::invalid_argument("channel index exceeds available channels in the view");
    }
    using channel_type = typename boost::gil::channel_type<View>::type;
    blaze::DynamicMatrix<true_channel_type_t<channel_type>> result(view.height(), view.width());
    to_matrix(view, result, channel);
    return result;
}

namespace detail
{
template <typename PixelType, std::size_t... indices>
auto pixel_to_vector_impl(const PixelType& pixel, std::integer_sequence<std::size_t, indices...>)
{
    using channel_t = typename boost::gil::channel_type<PixelType>::type;
    return blaze::StaticVector<true_channel_type_t<channel_t>, sizeof...(indices)>{
        pixel[indices]...};
}
} // namespace detail

/** \brief Converts a pixel into `StaticVector`

    Useful when working with multi-channel images

    \tparam PixelType The source pixel type
    \arg pixel The source pixel to convert into `StaticVector`

    \return a `StaticVector` where elements correspond to channel values, in the same order
*/
template <typename PixelType>
auto pixel_to_vector(const PixelType& pixel)
{
    return detail::pixel_to_vector_impl(
        pixel, std::make_index_sequence<boost::gil::num_channels<PixelType>{}>{});
}

/** \brief Convert possibly multi-channel view into `DynamicMatrix` of `StaticVector`s

    Works as if applying `pixel_to_vector` on each pixel to generate corresponding matrix entry

    \tparam View The source view type to convert into `DynamicMatrix`
    \arg view The source view

    \return a `DynamicMatrix<StaticVector<ChannelType, num_channel>>`, where each element
   corresponds to pixel in the view with `pixel_to_vector` applied.
*/
template <typename View>
auto to_matrix_channeled(View view)
{
    return blaze::evaluate(
        blaze::generate(view.height(), view.width(), [&view](std::size_t i, std::size_t j) {
            return pixel_to_vector(view(j, i));
        }));
}

/** \brief constructs `blaze::CustomMatrix` out of `image_view`

    Creates a matrix which is semantically like pointer, e.g. changes in the matrix
    will be reflected inside the view, and vice versa. Do note that assigning to another
    `blaze::CustomMatrix` will behave like shallow copy, thus it is important to perform
    deep copy using e.g. `blaze::DynamicMatrix` to have independent copy.

    The input view must have single channel pixels like `boost::gil::gray8_view_t`,
    for multi-channel matrices please first check layout compatibility of the pixel type
    and corresponding `blaze::StaticVector<ChannelType, num_channels>`, then use
    `as_matrix_channeled`.

    \tparam IsAligned A flag that indicates if the source view is aligned
    \tparam IsPAdded A flag that indicates if the source view is padded
    \tparam StorageOrder rowMajor or columnMajor
    \tparam SingleChannelView The type of the view to get matrix view from, must have single channel
        pixels
    \arg source The source image view to get matrix view from

    \return A `CustomMatrix<ChannelType>` with all the alignment, padding and storage order flags
*/
template <blaze::AlignmentFlag IsAligned = blaze::unaligned,
          blaze::PaddingFlag IsPadded = blaze::unpadded, bool StorageOrder = blaze::rowMajor,
          typename SingleChannelView>
auto as_matrix(SingleChannelView source)
{
    using channel_t = typename boost::gil::channel_type<SingleChannelView>::type;
    return blaze::CustomMatrix<channel_t, IsAligned, IsPadded, StorageOrder>(
        reinterpret_cast<true_channel_type_t<channel_t>*>(&source(0, 0)),
        source.height(),
        source.width());
}

/** \brief constructs `blaze::CustomMatrix` out of `image_view` whose elements are
    `StaticVector<ChannelType, num_channels>`

    Please note that there are layout incompatibilities between Blaze's `StaticVector` and
    GIL's pixel types. GIL's pixel types are padded to 4 byte boundary if the size is less than
    8 bytes, while Blaze pads to 16. There is a `static_assert` that prevents obviously wrong
    use cases, but it is advised to first check if the pixel type and resulting `StaticVector`
    are compatible, then using the function. For already tested types, please run tests for
    this function (tagged with the function name).

    \tparam IsPixelAligned Flag that indicates if individual pixels inside view are aligned.

    It seems like Blaze automatically pads if the flag is set, so do not set if padding will
    be off too.

    \tparam IsPixelPadded Flag that indicates if individual pixels inside view are padded.

    This flag is probably the source of problems. Be careful when setting it and always
    check if resulting `StaticVector` and pixel type are compatible

    \tparam IsAligned Flag that indicates if image is aligned in memory
    \tparam IsPadded Flag that indicates if individual rows are padded
    \tparam StorageOrder either rowMajor or columnMajor
    \tparam ImageView The type of image view to get channeled matrix view from
    \arg source The image view to get channeled matrix view from

    \return A `CustomMatrix<StaticVector<ChannelType, num_channel>>` with all the alignment, padding
   and storage order flags
*/
template <blaze::AlignmentFlag IsPixelAligned = blaze::unaligned,
          blaze::PaddingFlag IsPixelPadded = blaze::unpadded,
          blaze::AlignmentFlag IsAligned = blaze::unaligned,
          blaze::PaddingFlag IsPadded = blaze::unpadded, bool StorageOrder = blaze::rowMajor,
          typename ImageView>
auto as_matrix_channeled(ImageView source)
{
    using pixel_t = typename ImageView::value_type;
    using channel_t = typename boost::gil::channel_type<ImageView>::type;
    constexpr auto num_channels = boost::gil::num_channels<ImageView>{};
    // using element_type = blaze::StaticVector<true_channel_type_t<channel_t>,
    //                                          num_channels,
    //                                          blaze::rowMajor,
    //                                          IsPixelAligned,
    //                                          IsPixelPadded>;
    using element_type = typename layout_compatible_vector<pixel_t>::type;
    static_assert(sizeof(pixel_t) == sizeof(element_type),
                  "The function is made to believe that pixel and corresponding vector types are"
                  "layout compatible, but they are not");
    return blaze::CustomMatrix<element_type, IsAligned, IsPadded, StorageOrder>(
        reinterpret_cast<element_type*>(&source(0, 0)), source.height(), source.width());
}

/** \brief Converts vector into pixel

    Useful when working with multi-channel images.

    \tparam PixelType The pixel type to convert to
    \tparam VT The concrete vector type
    \arg vector The source vector to convert into `PixelType`
    \tparam TransposeFlag Transpose flag for the vector

    \return A pixel where each channel corresponds to entry in the vector, in the same order
*/
template <typename PixelType, typename VT, bool TransposeFlag>
auto vector_to_pixel(const blaze::DenseVector<VT, TransposeFlag>& vector)
{
    auto num_channels = boost::gil::num_channels<PixelType>{};
    PixelType pixel{};
    for (std::size_t i = 0; i < num_channels; ++i) {
        pixel[i] = (~vector)[i];
    }

    return pixel;
}

namespace detail
{
template <typename ImageView, typename PixelType = typename ImageView::value_type, typename MT>
void from_matrix(std::true_type /*is_vector*/, ImageView view,
                 const blaze::DenseMatrix<MT, blaze::rowMajor>& data)
{
    for (std::ptrdiff_t i = 0; i < view.height(); ++i) {
        for (std::ptrdiff_t j = 0; j < view.width(); ++j) {
            view(j, i) = vector_to_pixel<PixelType>((~data)(i, j));
        }
    }
}

template <typename ImageView, typename PixelType = typename ImageView::value_type, typename MT>
void from_matrix(std::false_type /*is not vector*/, ImageView view,
                 const blaze::DenseMatrix<MT, blaze::rowMajor>& data)
{
    for (std::ptrdiff_t i = 0; i < view.height(); ++i) {
        for (std::ptrdiff_t j = 0; j < view.width(); ++j) {
            view(j, i)[0] = (~data)(i, j);
        }
    }
}
} // namespace detail

/** \brief Converts a matrix into image with specified type

    This function automatically detects if the matrix is made of
    vectors or scalars, and does appropriate conversion from elements
    to pixel values and returns the resulting image

    \tparam ImageType The image type to convert into
    \tparam PixelType The pixel type that resulting image will consist of
    \tparam MT The concrete matrix type
    \tparam SO Either rowMajor or columnMajor
    \arg data The data to convert into image
*/
template <typename ImageType, typename PixelType = typename ImageType::value_type, typename MT,
          bool SO>
ImageType from_matrix(const blaze::DenseMatrix<MT, SO>& data)
{
    ImageType result((~data).columns(), (~data).rows());
    auto view = boost::gil::view(result);
    detail::from_matrix<decltype(view), PixelType>(
        blaze::IsDenseVector<blaze::UnderlyingElement_t<MT>>{}, view, data);
    return result;
}

/** \brief Converts the input matrix into image and writes into provided view

    This function automatically detects if the matrix is made of
    vectors or scalars, and does appropriate conversion from elements
    to pixel values and writes into passed view

    \tparam ImageView the type of ImageView to write image into
    \tparam PixelType the type of pixels that the resulting image is made of
    \tparam MT The concrete type of matrix
    \tparam SO Either rowMajor or columnMajor
*/
template <typename ImageView, typename PixelType = typename ImageView::value_type, typename MT,
          bool SO>
void from_matrix(const blaze::DenseMatrix<MT, SO>& data, ImageView view)
{
    detail::from_matrix<decltype(view), PixelType>(
        blaze::IsDenseVector<blaze::UnderlyingElement_t<MT>>{}, view, data);
}

}}


namespace blaze
{
template <typename Pixel, bool IsRowVector>
struct UnderlyingElement<boost::gil::pixel_vector<Pixel, IsRowVector>> {
    using Type = typename boost::gil::channel_type<Pixel>::type;
};

template <typename Pixel, bool IsRowVector>
struct IsDenseVector<boost::gil::pixel_vector<Pixel, IsRowVector>> : std::true_type {
};
} // namespace blaze