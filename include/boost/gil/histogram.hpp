//
// Copyright 2020 Debabrata Mandal <mandaldebabrata123@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//

#ifndef BOOST_GIL_EXTENSION_HISTOGRAM_HISTOGRAM_HPP
#define BOOST_GIL_EXTENSION_HISTOGRAM_HISTOGRAM_HPP

#include <boost/gil/concepts/concept_check.hpp>
#include <boost/gil/gray.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_view_factory.hpp>

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

namespace detail {

// Used to check if container implements operator[] and operator++
template <typename T, typename = int>
struct is_indexable : std::false_type {} ;

template <typename T>
struct is_indexable
        <
            T,
            decltype((void) ++declval<T>()[0] , 0)
        > : std::true_type {} ;

// Used to check if container is resizable i.e. implements resize() member function
template <typename T, typename = int>
struct is_resizable : std::false_type {} ;

template <typename T>
struct is_resizable
        <
            T,
            decltype((void) declval<T>().resize(1), 0)
        > : std::true_type {} ;

// Used to check if container is key-value type and the key is of std::integral type
template <typename T, typename = void>
struct has_integral_key_type : std::false_type {} ;

template <typename T>
struct has_integral_key_type
        <
            T,
            typename std::enable_if<std::is_integral<typename T::key_type >::value >::type 
        > : std::true_type {} ;

} // namespace detail

/// \ingroup Histogram
/// \brief Use for indexable and resizable sequence containers 
///
/// This overload corresponds to resizable sequence containers which have random access capability.
//  This would correspond to containers from STL like std::vector and std::deque. The only 
/// difference from the next function is this function is helpful if older bin values in container
/// is not to be erased i.e. accumulate the histograms.
///
template
<
    typename Container,
    typename SrcView,
    typename std::enable_if<
                detail::is_indexable<Container>::value &&
                detail::is_resizable<Container>::value, int>::type = 0
>
inline void image_histogram(SrcView const& srcview, Container &hist)
{
    gil_function_requires<ImageViewConcept<SrcView>>();
    static_assert(std::is_arithmetic<typename Container::value_type>::value,
                    "Improper container type for images.");
    static_assert(std::is_unsigned<typename channel_type<SrcView>::type>::value,
                    "Improper container type for signed images.");

    using channel_t = typename channel_type<SrcView>::type;
    using pixel_t = pixel<channel_t, gray_layout_t>;

    hist.resize(std::numeric_limits<channel_t>::max() + 1);

    for_each_pixel(color_converted_view<pixel_t>(srcview), [&hist](pixel_t const& p) {
        ++hist[p];
    });
}

/// \ingroup Histogram
/// \brief Use for indexable and resizable sequence containers
///
/// This overload is the same as the above except that it does not produce the
/// accumulate effect for histogram bin values. It returns a new container of
/// size large enough to hold entire range of corresponding pixel value in the input image.
///
template
<
    typename Container,
    typename SrcView,
    typename std::enable_if<
                detail::is_indexable<Container>::value &&
                detail::is_resizable<Container>::value, int>::type = 0
>
inline Container image_histogram(SrcView const& srcview)
{
    gil_function_requires<ImageViewConcept<SrcView>>();
    static_assert(std::is_arithmetic<typename Container::value_type>::value,
                    "Improper container type for images.");
    static_assert(std::is_unsigned<typename channel_type<SrcView>::type>::value,
                    "Improper container type for signed images.");

    using channel_t = typename channel_type<SrcView>::type;
    using pixel_t = pixel<channel_t, gray_layout_t>;

    Container hist(std::numeric_limits<channel_t>::max() + 1);
    
    for_each_pixel(color_converted_view<pixel_t>(srcview), [&hist](pixel_t const& p) {
        ++hist[p];
    });
    return hist;
}

/// \ingroup Histogram
/// \brief Use for indexable and non-resizable sequence containers 
///
/// This overload differs from the first, it gives a partial histogram over the image
/// for non-resizable sequence containers. No overload of accumulate type is provided in
/// this case. Example container would be std::array.
///
template
<
    typename Container,
    typename SrcView,
    typename std::enable_if <
                detail::is_indexable<Container>::value &&
                !detail::is_resizable<Container>::value &&
                std::is_arithmetic<typename Container::value_type>::value, int>::type = 0
>
inline void image_histogram(SrcView const& srcview, Container &hist)
{
    gil_function_requires<ImageViewConcept<SrcView>>();
    static_assert(std::is_arithmetic<typename Container::value_type>::value,
                    "Improper container type for images.");
    static_assert(std::is_unsigned<typename channel_type<SrcView>::type>::value,
                    "Improper container type for signed images.");

    using channel_t = typename channel_type<SrcView>::type;
    using pixel_t = pixel<channel_t, gray_layout_t>;
    
    for_each_pixel(color_converted_view<pixel_t>(srcview), [&hist](pixel_t const& p) {
        if (p < hist.size()) ++hist[p];
    });
}

/// \ingroup Histogram
/// \brief Use for indexable and pair associative containers
///
/// This overload corresponds to providing histogram support for pair associative containers like
/// std::map, std::unordered_map, std::multimap and std::unordered_mulitmap. No extra constraints 
/// other than what is already mentioned. This overload accumulates the histogram.
///
template
<
    typename Container,
    typename SrcView,
    typename std::enable_if<
                detail::is_indexable<Container>::value &&
                detail::has_integral_key_type<Container>::value, int>::type = 0
>
inline void image_histogram(SrcView const& srcview, Container &hist)
{
    gil_function_requires<ImageViewConcept<SrcView>>();
    static_assert(std::is_arithmetic<typename Container::mapped_type>::value,
                    "Improper container type for images.");

    using channel_t = typename channel_type<SrcView>::type;
    using pixel_t = pixel<channel_t, gray_layout_t>;

    for_each_pixel(color_converted_view<pixel_t>(srcview), [&hist](pixel_t const& p) {
        ++hist[p];
    });
}

/// \ingroup Histogram
/// \brief Use for indexable and pair associative containers
///
/// This overload is the same as the above except that it does not produce the
/// accumulate effect for histogram bin values.
///
template
<   
    typename Container,
    typename SrcView,
    typename std::enable_if<
                detail::is_indexable<Container>::value &&
                detail::has_integral_key_type<Container>::value, int>::type = 0
>
inline Container image_histogram(SrcView const& srcview)
{
    gil_function_requires<ImageViewConcept<SrcView>>();
    static_assert(std::is_arithmetic<typename Container::mapped_type>::value,
                    "Improper container type for images.");

    using channel_t = typename channel_type<SrcView>::type;
    using pixel_t = pixel<channel_t, gray_layout_t>;

    Container hist;

    for_each_pixel(color_converted_view<pixel_t>(srcview), [&hist](pixel_t const& p) {
        ++hist[p];
    });
    return hist;
}

}} // namespace boost::gil

#endif
