//
// Copyright 2020 Nedjima Belgacem <gb_nedjima@esi.dz>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_EXTENSION_HISTOGRAM_HISTOGRAM_ALGORITHMS_HPP
#define BOOST_GIL_EXTENSION_HISTOGRAM_HISTOGRAM_ALGORITHMS_HPP

#include "histogram.hpp"

namespace boost {
namespace gil {

template<typename SrcView, typename DstView>
void equalise_histogram(const SrcView& src_view, DstView& dst_view) {
    const int nb_channels = boost::mp11::mp_size<SrcView::value_type::layout_t::color_space_t>::value;

    histogram<SrcView::value_type> hist;
    fill_histogram(src_view, hist);
    long long size = src_view.width() * src_view.height();
    int cumulative_sum[hist.size()];
    cumulative_sum[0] = 0;
    for (int i = 1; i < hist.size(); ++i) cumulative_sum[i] = cumulative_sum[i - 1] + hist[SrcView::value_type(i)];
    for (int y = 0; y < src_view.height(); ++y) {
        for (int x = 0; x < src_view.width(); ++x) {
            // Not sure wether I should consider seperate histograms for each channel or do it like this
            for (int c = 0; c < nb_channels; ++c) {
                nth_channel_view(dst_view, c)(x, y) = (float)(hist.size() - 1) * cumulative_sum[nth_channel_view(src_view, c)(x, y)] / size;
            }
        }
    }
}

template <typename ImageView, typename HistogramColorSpace>
void fill_histogram(const ImageView& img_view, histogram<HistogramColorSpace>& hist) {
    for (int j = 0; j < img_view.height(); ++j) {
        for (int i = 0; i < img_view.width(); ++i) {
            HistogramColorSpace pixel;
            color_convert<ImageView::value_type, HistogramColorSpace>(img_view(i, j), pixel);
            ++hist[pixel];
        }
    }
}


// Match source image according to a reference cumulative distribution function 
template<typename SrcView, typename DstView>
void match_histograms(const SrcView& src_view, float reference_cdf[256], DstView& dst_view) {
    const int nb_channels = boost::mp11::mp_size<SrcView::value_type::layout_t::color_space_t>::value;

    histogram<gray8_pixel_t> original_hist;
    fill_histogram(src_view, original_hist);
    float img_pixel_count = src_view.width() * src_view.height();
    float original_cdf[original_hist.size()];
    original_cdf[0] = (float)original_hist[gray8_pixel_t(0)] / img_pixel_count;
    for (int i = 1; i < original_hist.size(); ++i) original_cdf[i] = original_cdf[i - 1] + (float)original_hist[gray8_pixel_t(i)] / img_pixel_count;
    int gray_level_mapping[256];
    for (int i = 0; i < 256; ++i) {
        int j = 0;
        if (original_cdf[i] <= reference_cdf[0]) j = 0;
        else if (original_cdf[i] >= reference_cdf[255]) j = 255;
        else {
            while (j < 256 && original_cdf[i] > reference_cdf[j]) j++;
        }
        gray_level_mapping[i] = j;
    }
    for (int y = 0; y < src_view.height(); ++y) {
        for (int x = 0; x < src_view.width(); ++x) {
            for (int c = 0; c < nb_channels; ++c) {
                nth_channel_view(dst_view, c)(x, y) = gray_level_mapping[nth_channel_view(src_view, c)(x, y)];
            }
        }
    }
}

template<typename ColorSpace, typename PixelType = gray8_pixel_t>
void save_histogram_img(histogram<ColorSpace, PixelType>& hist, std::string filename) {
    int max = static_cast<int>((*std::max_element(hist.begin(), hist.end()) + 1) * 1.2f);
    const float good_aspect_ratio = 3.0f;
    int multiplier = static_cast<int>(std::ceil(good_aspect_ratio * max / hist.size()));

    gray8_image_t out_img(multiplier * hist.size(), max + 1);
    gray8_view_t out_view = view(out_img);

    fill_pixels(out_view, 0);
    for (int i = 0; i < hist.size(); ++i) {
        PixelType color(i);
        for (int j = 0; j < hist[color]; ++j) {
            for (int k = 0; k < multiplier; ++k) out_view(multiplier * i + k, max - j) = 255;
        }
    }
    write_view(filename, out_view, jpeg_tag{});
}

}
}

#endif