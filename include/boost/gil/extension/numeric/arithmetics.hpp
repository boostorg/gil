//
// Created by shino on 14.08.21.
//

#ifndef BOOST_GIL_ARITHMETICS_HPP
#define BOOST_GIL_ARITHMETICS_HPP

#include <boost/gil/extension/numeric/matrix.hpp>

#include <boost/gil/io/typedefs.hpp>

namespace boost { namespace gil {

template <typename GrayImageView>
void value_cast(const GrayImageView& view, const gil::matrix& dst) {
    if (view.dimensions() != dst.dimensions()) {
        throw std::invalid_argument("dimensions are not the same");
    }

    using pixel_type = typename GrayImageView::value_type;
    gil::transform_pixels(view, dst, [](const pixel_type& pixel) {
        return static_cast<float>(pixel[0]);
    });
}

template <typename SingleChannelPixel, typename InterimType = double>
    struct remap_converter {
    InterimType src_min;
    InterimType src_max;
    InterimType dst_min = std::numeric_limits<typename channel_type<SingleChannelPixel>::type>::min();
    InterimType dst_max = std::numeric_limits<typename channel_type<SingleChannelPixel>::type>::max();
    using destination_channel_t = typename gil::channel_type<SingleChannelPixel>::type;

    SingleChannelPixel operator()(gil::elemc_ref_t matrix_elem) {
        const auto dst_range_length = dst_max - dst_min;
        const auto src_range_length = src_max - src_min;
        return SingleChannelPixel(static_cast<destination_channel_t>(dst_min + (matrix_elem[0] - src_min)
        / src_range_length
        * dst_range_length));
    }
};

template <typename SingleChannelPixel, typename InterimType = double>
    struct abs_remap_converter: remap_converter<SingleChannelPixel, InterimType> {
        SingleChannelPixel operator()(gil::elem_t matrix_elem) {
            matrix_elem[0] = std::abs(matrix_elem[0]);
            return static_cast<remap_converter<SingleChannelPixel, InterimType>&>(*this)
            (matrix_elem);
        }
    };

template <typename InterimType = double, typename GrayImageView>
void remap_cast(const gil::matrix& src, const GrayImageView& dst,
                InterimType src_range_min, InterimType src_range_max,
                InterimType dst_min = std::numeric_limits<typename channel_type<GrayImageView>::type>::min(),
                InterimType dst_max = std::numeric_limits<typename channel_type<GrayImageView>::type>::max()) {
    using pixel_t = typename GrayImageView::value_type;
    auto converter = remap_converter<pixel_t, double>{};
    converter.src_min = src_range_min;
    converter.src_max = src_range_max;
    converter.dst_min = dst_min;
    converter.dst_max = dst_max;
    gil::transform_pixels(src, dst, converter);
}

template <typename InterimType = double, typename GrayImageView>
void abs_remap_cast(const gil::matrix& src, const GrayImageView& dst,
                    InterimType src_range_min, InterimType src_range_max,
                    InterimType dst_min = std::numeric_limits<typename channel_type<GrayImageView>::type>::min(),
                    InterimType dst_max = std::numeric_limits<typename channel_type<GrayImageView>::type>::max()) {
    using pixel_t = typename GrayImageView::value_type;
    auto converter = abs_remap_converter<pixel_t, double>{};
    converter.src_min = src_range_min;
    converter.src_max = src_range_max;
    converter.dst_min = dst_min;
    converter.dst_max = dst_max;
    gil::transform_pixels(src, dst, converter);
}

template <typename PixelType>
struct hypot_converter {
    PixelType operator()(boost::gil::elemc_ref_t lhs, boost::gil::elemc_ref_t rhs) {
        return PixelType(std::hypot(lhs[0], rhs[0]));
    }
};

template <typename DstGrayView>
void gradient(const boost::gil::matrix& dx, const boost::gil::matrix& dy, const DstGrayView& dst) {
    using pixel_t = typename DstGrayView::value_type;
    gil::transform_pixels(dx, dy, dst, hypot_converter<pixel_t>{});
}
}}

#endif // BOOST_GIL_ARITHMETICS_HPP
