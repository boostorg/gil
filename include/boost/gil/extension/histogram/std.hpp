//
// Copyright 2020 Debabrata Mandal <mandaldebabrata123@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//

#ifndef BOOST_GIL_EXTENSION_HISTOGRAM_STL_HISTOGRAM_HPP
#define BOOST_GIL_EXTENSION_HISTOGRAM_STL_HISTOGRAM_HPP

#include <boost/gil/concepts/concept_check.hpp>
#include <boost/gil/gray.hpp>
#include <boost/gil/histogram.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_view_factory.hpp>

#include <vector>
#include <array>
#include <unordered_map>
#include <map>
#include <utility>

//////////////////////////////////////////////////////////
/// Histogram
/////////////////////////////////////////////////////////

/// \defgroup Histogram Histogram Filling Algorithms
/// \brief Collection of functions to provide histogram support in GIL using Standard
///        Template Library Containers
/// The conversion from Boost.GIL images to compatible histograms are provided. The supported
/// container types would be std::vector, std::array, std::map, std::unordered_map, std::deque.
/// 2 possible calling syntax are provided. The implementation is for 1D histograms hence
/// multi-channeled image views are color converted to gray color layout.
/// Example : 
/// \code
/// //This is the common case.
/// std::vector<int> v = image_histogram<vector<int>>(view(img));
///
/// //Alternative, when there is a need to accumulate the histograms.
/// image_histogram(view(img), hist); //hist - histogram container
/// \endcode
///
/// Some general constraints on usage:-
/// 1. Cannot use signed images with compatible random access containers.
/// 2. Automatic resize of containers in case of shortage of bins, to ensure
///    correctness comes before performance.
/// 3. Container key type (if exists) has to be one of std::integral types to be 
///    GIL compatible.
/// 4. Container value type has to be of std::arithmetic types.
///

namespace boost { namespace gil {

/// \ingroup Histogram
/// \brief std::vector
///
/// This overload corresponds to std::vector.
///

template<typename SrcView, typename T>
void fill_histogram(SrcView const& srcview, std::vector<T> &histogram, bool accumulate = false)
{
    gil_function_requires<ImageViewConcept<SrcView>>();
    static_assert(std::is_arithmetic<T>::value,
                    "Improper container type for images.");
    static_assert(std::is_unsigned<typename channel_type<SrcView>::type>::value,
                    "Improper container type for signed images.");

    using channel_t = typename channel_type<SrcView>::type;
    using pixel_t = pixel<channel_t, gray_layout_t>;

    if (!accumulate) histogram.clear();
    histogram.resize(std::numeric_limits<channel_t>::max() + 1);

    for_each_pixel(color_converted_view<pixel_t>(srcview), [&](pixel_t const& p) {
        ++histogram[p];
    });
}

/// \ingroup Histogram
/// \brief std::array
///
/// This overload corresponds to std::array.
///
template<typename SrcView, typename T, std::size_t N>
void fill_histogram(SrcView const& srcview, std::array<T, N> &histogram, bool accumulate = false)
{
    gil_function_requires<ImageViewConcept<SrcView>>();
    static_assert(std::is_arithmetic<T>::value && N > 0,
                    "Improper container type for images.");
    static_assert(std::is_unsigned<typename channel_type<SrcView>::type>::value,
                    "Improper container type for signed images.");

    using channel_t = typename channel_type<SrcView>::type;
    using pixel_t = pixel<channel_t, gray_layout_t>;
    
    const size_t pixel_max = std::numeric_limits<channel_t>::max();
    const float scale = (histogram.size() - 1.0f) / pixel_max;
    
    if (!accumulate) std::fill(std::begin(histogram), std::end(histogram), 0);

    for_each_pixel(color_converted_view<pixel_t>(srcview), [&](pixel_t const& p) {
        ++histogram[static_cast<std::size_t>(p * scale)];
    });
}

/// \ingroup Histogram
/// \brief std::unordered_map
///
/// This overload corresponds to std::unordered_map.
///
template<typename SrcView, typename T1, typename T2>
void fill_histogram(SrcView const& srcview, std::unordered_map<T1, T2> &histogram, bool accumulate = false)
{
    gil_function_requires<ImageViewConcept<SrcView>>();
    static_assert(std::is_arithmetic<T1>::value && std::is_integral<T2>::value,
                    "Improper container type for images.");

    using channel_t = typename channel_type<SrcView>::type;
    using pixel_t = pixel<channel_t, gray_layout_t>;

    if (!accumulate) histogram.clear();

    for_each_pixel(color_converted_view<pixel_t>(srcview), [&](pixel_t const& p) {
        ++histogram[p];
    });
}


/// \ingroup Histogram
/// \brief std::map
///
/// This overload corresponds to std::map.
///
template<typename SrcView, typename T1, typename T2>
void fill_histogram(SrcView const& srcview, std::map<T1, T2> &histogram, bool accumulate = false)
{
    gil_function_requires<ImageViewConcept<SrcView>>();
    static_assert(std::is_arithmetic<T1>::value && std::is_integral<T2>::value,
                    "Improper container type for images.");

    using channel_t = typename channel_type<SrcView>::type;
    using pixel_t = pixel<channel_t, gray_layout_t>;

    if (!accumulate) histogram.clear();

    for_each_pixel(color_converted_view<pixel_t>(srcview), [&](pixel_t const& p) {
        ++histogram[p];
    });
}

template<typename T>
void cumulative_histogram(std::vector<T> &histogram)
{
    static_assert(std::is_arithmetic<T>::value,
                    "Improper container type for images.");
    T cumulative_counter = 0;
    for (std::size_t i = 0; i < histogram.size(); i++)
    {
        cumulative_counter += histogram[i];
        histogram[i] = cumulative_counter;
    }
}

template<typename T, std::size_t N>
void cumulative_histogram(std::array<T, N> &histogram)
{
    static_assert(std::is_arithmetic<T>::value && N > 0,
                    "Improper container type for images.");
    T cumulative_counter = 0;
    for (std::size_t i = 0; i < N; i++)
    {
        cumulative_counter += histogram[i];
        histogram[i] = cumulative_counter;
    }
}

template<typename T1, typename T2>
void cumulative_histogram(std::unordered_map<T1, T2> &histogram)
{
    static_assert(std::is_arithmetic<T1>::value && std::is_integral<T2>::value,
                    "Improper container type for images.");
    using value_t = typename std::unordered_map<T1, T2>::value_type;
    std::vector<std::pair<T1, T2>> sorted_keys(histogram.size());
    std::size_t counter=0;

    std::for_each(histogram.begin(), histogram.end(), [&](value_t const& v) {
        sorted_keys[counter++] = std::make_pair(v.first, v.second);
    });
    
    std::sort(sorted_keys.begin(), sorted_keys.end());
    int cumulative_counter=0;
    for(std::size_t i = 0; i < sorted_keys.size(); ++i)
    {
        cumulative_counter += sorted_keys[i].second;
        histogram[sorted_keys[i].first] = cumulative_counter;
    }
}

template<typename T1, typename T2>
void cumulative_histogram(std::map<T1, T2> &histogram)
{
    static_assert(std::is_arithmetic<T1>::value && std::is_integral<T2>::value,
                    "Improper container type for images.");
    T2 cumulative_counter = 0;
    for(auto const& it : histogram)
    {
        cumulative_counter += it.second;
        histogram[it.first] = cumulative_counter;
    }
}

}} // namespace boost::gil

#endif
