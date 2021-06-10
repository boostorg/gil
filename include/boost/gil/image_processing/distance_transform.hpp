//
// Copyright 2021 Harshit Pant <harshitpant83@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_GIL_IMAGE_PROCESSING_DISTANCE_TRANSFORM_HPP
#define BOOST_GIL_IMAGE_PROCESSING_DISTANCE_TRANSFORM_HPP

#include <boost/gil/concepts/concept_check.hpp>
#include <boost/gil/gray.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>

#include <boost/assert.hpp>

#include <cmath>
#include <limits>
#include <vector>

namespace boost { namespace gil {

namespace distance_type {

struct euclidean_approximation_t {};
struct manhattan_t {};
struct chessboard_t {};
struct precise_euclidean_t {};

static euclidean_approximation_t euclidean_approximation;
static manhattan_t manhattan;
static chessboard_t chessboard;
static precise_euclidean_t precise_euclidean;

/// \breif Checks if distance_type used exists.
template <typename DistanceType>
struct check_distance_type
{
    static const bool value =  std::is_same<DistanceType, euclidean_approximation_t>::value  ||
                               std::is_same<DistanceType, manhattan_t>::value  ||
                               std::is_same<DistanceType, chessboard_t>::value ||
                               std::is_same<DistanceType, precise_euclidean_t>::value;
};
} // namespace distance_type

namespace mask_size {

struct three_t {};
struct five_t {};
struct not_applicable_t {};

static three_t three;
static five_t five;
static not_applicable_t not_applicable;

/// \breif Checks if mask_size used exists.
template <typename MaskSize>
struct check_mask_size
{
    static const bool value =  std::is_same<MaskSize, three_t>::value ||
                               std::is_same<MaskSize, five_t>::value  ||
                               std::is_same<MaskSize, not_applicable_t>::value;
};
} // namespace mask_size

enum class distance_from
{
    off_pixels,
    on_pixels
};

namespace detail {

/// \breif Checks compatiblity of distance_type and mask_size used together.
template <typename DistanceType, typename MaskSize>
struct dt_parameters_are_compatible
{
    static const bool value = true;
};

template <typename DistanceType>
struct dt_parameters_are_compatible<DistanceType, mask_size::not_applicable_t>
{
    static const bool value = false;
};

template <typename MaskSize>
struct dt_parameters_are_compatible<distance_type::precise_euclidean_t, MaskSize>
{
    static const bool value = false;
};

template <>
struct dt_parameters_are_compatible<distance_type::precise_euclidean_t, mask_size::not_applicable_t>
{
    static const bool value = true;
};

/// \breif Value used as infinite distance for distance_transform.
float constexpr dt_infinite = 1000000000;

/// \breif Calculates distance transfrom with mask size three.
///  Optimal local distances a, b for euclidean approximation from,
///  http://www.cmm.mines-paristech.fr/~marcoteg/cv/publi_pdf/MM_refs/1986_Borgefors_distance.pdf
template<typename SrcView, typename DstView, typename DistanceType>
void distance_transform_mask_size_three_impl(SrcView const& src_view, 
                                             DstView const& dst_view, 
                                             distance_from dist_from, 
                                             DistanceType /*Passes type*/)
{
    int const padding = 1;
    gray32f_image_t intermediate_image(src_view.width()  + 2 * padding,
                                       src_view.height() + 2 * padding);
    gray32f_view_t  intermediate_image_view = view(intermediate_image);

    for (std::ptrdiff_t x = 0; x < intermediate_image_view.width(); ++x)
    {
        intermediate_image_view(x, 0)[0] = dt_infinite ;
        intermediate_image_view(x, intermediate_image_view.height() - 1)[0] = dt_infinite;
    }
    for (std::ptrdiff_t y = 0; y < intermediate_image_view.height(); ++y)
    {
        intermediate_image_view(0, y)[0] = dt_infinite ;
        intermediate_image_view(intermediate_image_view.width() - 1, y)[0] = dt_infinite ;
    }

    float constexpr src_channel_min =
                      (std::numeric_limits <typename channel_type<SrcView>::type>::min)();

    for (std::ptrdiff_t y = 0; y < src_view.height(); ++y)
    {
        for (std::ptrdiff_t x = 0; x < src_view.width(); ++x)
        {
            if(dist_from == distance_from::off_pixels)
            {
                if (src_view(x, y)[0] == src_channel_min)
                    intermediate_image_view(x + padding, y + padding)[0] = 0;
                else 
                    intermediate_image_view(x + padding, y + padding)[0] = dt_infinite;
            }
            else
            {
                if (src_view(x, y)[0] != src_channel_min) 
                    intermediate_image_view(x + padding, y + padding)[0] = 0;
                else 
                    intermediate_image_view(x + padding, y + padding)[0] = dt_infinite;
            }
        }
    }

    float a, b;

    if(std::is_same<DistanceType, distance_type::euclidean_approximation_t>::value)
    {
        a = 0.95509f;
        b = 1.36930f;
    }
    else if(std::is_same<DistanceType, distance_type::manhattan_t>::value)
    { 
        a = 1;
        b = 2;
    }
    else
    { 
        a = 1;
        b = 1;
    }

    for (std::ptrdiff_t y = 0; y < src_view.height(); ++y)
    {
        for (std::ptrdiff_t x = 0; x < src_view.width(); ++x)
        {
            if(intermediate_image_view(x + padding, y + padding)[0] > 0)
            { 
                float const d1 = a + intermediate_image_view(x - 1 + padding, y +     padding)[0];
                float const d2 = b + intermediate_image_view(x - 1 + padding, y - 1 + padding)[0];
                float const d3 = a + intermediate_image_view(x +     padding, y - 1 + padding)[0];
                float const d4 = b + intermediate_image_view(x + 1 + padding, y - 1 + padding)[0];

                intermediate_image_view(x + padding, y + padding)[0] = std::min({d1, d2, d3, d4});
            }
        }
    }

    for (std::ptrdiff_t y = src_view.height() - 1; y >= 0; --y)
    {
        for (std::ptrdiff_t x = src_view.width() - 1; x >= 0; --x)
        {
            if (intermediate_image_view(x + padding, y + padding)[0] > 0)
            {
                float const d1 = a + intermediate_image_view(x + 1 + padding, y +     padding)[0];
                float const d2 = b + intermediate_image_view(x + 1 + padding, y + 1 + padding)[0];
                float const d3 = a + intermediate_image_view(x +     padding, y + 1 + padding)[0];
                float const d4 = b + intermediate_image_view(x - 1 + padding, y + 1 + padding)[0];

                intermediate_image_view(x + padding, y + padding)[0] = std::min({ d1, d2, d3, d4,
                       static_cast<float>(intermediate_image_view(x + padding, y + padding)[0]) });
                
                float const distance_transform = intermediate_image_view
                                                (x + padding, y + padding)[0];

                float constexpr dst_channel_max =
                      (is_same<DstView, gray32f_view_t>::value) ? dt_infinite : static_cast<float>(
                      (std::numeric_limits<typename channel_type<DstView>::type>::max)());

                float constexpr dst_channel_min =
                      (is_same<DstView, gray32f_view_t>::value) ? 0.f : static_cast<float>(
                      (std::numeric_limits<typename channel_type<DstView>::type>::min)());

                dst_view(x, y)[0] =
                      static_cast<typename channel_type<typename DstView::value_type>::type>(
                                   (distance_transform > dst_channel_max) ? dst_channel_max :
                                   (distance_transform < dst_channel_min) ? dst_channel_min :
                                    distance_transform);
            }
        }
    }
}  

/// \breif Calculates distance transfrom with mask size five.
///  Optimal local distances a, b, c for euclidean approximation from,
///  http://www.cmm.mines-paristech.fr/~marcoteg/cv/publi_pdf/MM_refs/1986_Borgefors_distance.pdf
template<typename SrcView, typename DstView, typename DistanceType>
void distance_transform_mask_size_five_impl(SrcView const& src_view,
                                            DstView const& dst_view, 
                                            distance_from dist_from,
                                            DistanceType /*Passes type*/)
{
    int const padding = 2;
    gray32f_image_t intermediate_image(src_view.width()  + 2 * padding, 
                                       src_view.height() + 2 * padding);
    gray32f_view_t intermediate_image_view = view(intermediate_image);

    for (std::ptrdiff_t x = 0; x < intermediate_image_view.width(); ++x)
    {
        intermediate_image_view(x, 0)[0] = dt_infinite;
        intermediate_image_view(x, 1)[0] = dt_infinite;
        intermediate_image_view(x, intermediate_image_view.height() - 1)[0] = dt_infinite;
        intermediate_image_view(x, intermediate_image_view.height() - 2)[0] = dt_infinite;
    }
    for (std::ptrdiff_t y = 0; y < intermediate_image_view.height(); ++y)
    {
        intermediate_image_view(0, y)[0] = dt_infinite ;
        intermediate_image_view(1, y)[0] = dt_infinite ;
        intermediate_image_view(intermediate_image_view.width() - 1, y)[0] = dt_infinite;
        intermediate_image_view(intermediate_image_view.width() - 2, y)[0] = dt_infinite;
    }

    float constexpr src_channel_min =
                      (std::numeric_limits<typename channel_type<SrcView>::type>::min)();

    for (std::ptrdiff_t y = 0; y < src_view.height(); ++y)
    {
        for (std::ptrdiff_t x = 0; x < src_view.width(); ++x)
        {
            if(dist_from == distance_from::off_pixels)
            {
                if (src_view(x, y)[0] == src_channel_min)
                    intermediate_image_view(x + padding, y + padding)[0] = 0;
                else
                    intermediate_image_view(x + padding, y + padding)[0] = dt_infinite;
            }
            else
            {
                if (src_view(x, y)[0] != src_channel_min)
                    intermediate_image_view(x + padding, y + padding)[0] = 0;
                else
                    intermediate_image_view(x + padding, y + padding)[0] = dt_infinite;
            }
        }
    }

    float a, b, c;

    if(std::is_same<DistanceType, distance_type::euclidean_approximation_t>::value)
    {
        a = 1;
        b = 1.4f;
        c = 2.19691f;
    }
    else if(std::is_same<DistanceType, distance_type::manhattan_t>::value)
    { 
        a = 1;
        b = 2;
        c = 3;
    }
    else
    { 
        a = 1;
        b = 1;
        c = 2;
    }

    for (std::ptrdiff_t y = 0; y < src_view.height(); ++y)
    {
        for (std::ptrdiff_t x = 0; x < src_view.width(); ++x)
        {
            if(intermediate_image_view(x + padding, y + padding)[0] > 0)
            { 
                float const d1 = c + intermediate_image_view(x - 1 + padding, y - 2 + padding)[0];
                float const d2 = c + intermediate_image_view(x + 1 + padding, y - 2 + padding)[0];
                float const d3 = c + intermediate_image_view(x - 2 + padding, y - 1 + padding)[0];
                float const d4 = b + intermediate_image_view(x - 1 + padding, y - 1 + padding)[0];
                float const d5 = a + intermediate_image_view(x +     padding, y - 1 + padding)[0];
                float const d6 = b + intermediate_image_view(x + 1 + padding, y - 1 + padding)[0];
                float const d7 = c + intermediate_image_view(x + 2 + padding, y - 1 + padding)[0];
                float const d8 = a + intermediate_image_view(x - 1 + padding, y +     padding)[0];

                intermediate_image_view(x + padding, y + padding)[0] = 
                                                       std::min({d1, d2, d3, d4, d5, d6, d7, d8});
            }
        }
    }

    for (std::ptrdiff_t y = src_view.height() - 1; y >= 0; --y)
    {
        for (std::ptrdiff_t x = src_view.width() - 1; x >= 0; --x)
        {
            if (intermediate_image_view(x + padding, y + padding)[0] > 0)
            {
                float const d1 = c + intermediate_image_view(x + 1 + padding, y + 2 + padding)[0];
                float const d2 = c + intermediate_image_view(x - 1 + padding, y + 2 + padding)[0];
                float const d3 = c + intermediate_image_view(x + 2 + padding, y + 1 + padding)[0];
                float const d4 = b + intermediate_image_view(x + 1 + padding, y + 1 + padding)[0];
                float const d5 = a + intermediate_image_view(x +     padding, y + 1 + padding)[0];
                float const d6 = b + intermediate_image_view(x - 1 + padding, y + 1 + padding)[0];
                float const d7 = c + intermediate_image_view(x - 2 + padding, y + 1 + padding)[0];
                float const d8 = a + intermediate_image_view(x + 1 + padding, y +     padding)[0];

                intermediate_image_view(x + padding, y + padding)[0] = 
                      std::min({ d1, d2, d3, d4, d5, d6, d7, d8, 
                      static_cast<float>(intermediate_image_view(x + padding, y + padding)[0]) });

                float const distance_transform =
                                            intermediate_image_view (x + padding, y + padding)[0];

                float constexpr dst_channel_max =
                     (is_same<DstView, gray32f_view_t>::value) ? dt_infinite : static_cast<float>(
                     (std::numeric_limits<typename channel_type<DstView>::type>::max)());

                float constexpr dst_channel_min =
                     (is_same<DstView, gray32f_view_t>::value) ? 0.f : static_cast<float>(
                     (std::numeric_limits<typename channel_type<DstView>::type>::min)());
                
                dst_view(x, y)[0] =
                      static_cast<typename channel_type<typename DstView::value_type>::type>(
                                   (distance_transform > dst_channel_max) ? dst_channel_max :
                                   (distance_transform < dst_channel_min) ? dst_channel_min :
                                    distance_transform);
            }
        }
    }
}  

/// \breif Calculates one-dimensional squared euclidean distance.
/// Reference - http://www.theoryofcomputing.org/articles/v008a019/v008a019.pdf
template<typename ImageIterator>
std::vector<float> calculate_squared_euclidean_distance(ImageIterator f, std::ptrdiff_t n)
{
    std::ptrdiff_t k = 0;
    std::vector<float> d(n);
    std::vector<int> v(n);
    std::vector<int> z(n + 1);
    v[0] = 0;
    z[0] = static_cast<int>(-dt_infinite);
    z[1] = static_cast<int>(dt_infinite);
    for(int q = 1; q < n ; ++q)
    {
        float s = static_cast<float>(((f[q][0] + std::pow(q, 2)) - 
                                    (f[v[k]][0] + std::pow(v[k], 2))) / (2 * q - 2 * v[k]));
        while (s <= z[k])
        {
            --k;
            s = static_cast<float>(((f[q][0] + std::pow(q, 2)) - 
                                  (f[v[k]][0] + std::pow(v[k], 2))) / (2 * q - 2 * v[k]));
        }
        {
            k = k + 1;
            v[k]     = static_cast<int>(q);
            z[k]     = static_cast<int>(s);
            z[k + 1] = static_cast<int>(dt_infinite);
        }
    }
    k = 0;
    for (int q = 0; q < n; ++q)
    {
        while(z[k+1] < q)
        { 
            ++k;
        }
        d[q] = static_cast<float>(std::pow((q - v[k]), 2) + f[v[k]][0]);
    }
    return d;
}

/// \breif Calculates precise euclidean distance transform.
template<typename SrcView, typename DstView>
void distance_transorm_precise_impl(SrcView const& src_view, 
                                    DstView const& dst_view,
                                    distance_from dist_from)
{
    gray32f_image_t intermediate_image(src_view.dimensions());
    gray32f_view_t  intermediate_image_view = view(intermediate_image);
    float constexpr src_channel_min =
                            (std::numeric_limits<typename channel_type<SrcView>::type>::min)();

    for (std::ptrdiff_t y = 0; y < src_view.height(); ++y)
    {
        for (std::ptrdiff_t x = 0; x < src_view.width(); ++x)
        {
            if (dist_from == distance_from::off_pixels)
            {
                if (src_view(x, y)[0] == src_channel_min) intermediate_image_view(x, y)[0] = 0;
                else intermediate_image_view(x, y)[0] = dt_infinite;
            }
            else
            {
                if (src_view(x, y)[0] != src_channel_min) intermediate_image_view(x, y)[0] = 0;
                else intermediate_image_view(x, y)[0] = dt_infinite;
            }
        }
    }

    for (std::ptrdiff_t y = 0; y < intermediate_image_view.height(); ++y)
    {
        typename gray32f_view_t::x_iterator intermediate_row_itr = 
                                            intermediate_image_view.row_begin(y);

        std::vector<float> one_dimensional_squared_distance = calculate_squared_euclidean_distance
                                          (intermediate_row_itr, intermediate_image_view.width());

        for (std::ptrdiff_t x = 0; x < intermediate_image_view.width(); ++x)
        {
            intermediate_row_itr[x][0] = one_dimensional_squared_distance[x];
        }
    }

    for (std::ptrdiff_t x = 0; x < intermediate_image_view.width(); ++x)
    {
        typename gray32f_view_t::y_iterator intermediate_col_itr = 
                                            intermediate_image_view.col_begin(x);

        std::vector<float> one_dimensional_squared_distance = calculate_squared_euclidean_distance
                                         (intermediate_col_itr, intermediate_image_view.height());

        for (std::ptrdiff_t y = 0; y < intermediate_image_view.height(); ++y)
        {
            intermediate_col_itr[y][0] = one_dimensional_squared_distance[y];
            
            float const distance_transform = std::sqrt(intermediate_col_itr[y][0]);

            float constexpr dst_channel_max =
                     (is_same<DstView, gray32f_view_t>::value) ? dt_infinite : static_cast<float>(
                     (std::numeric_limits<typename channel_type<DstView>::type>::max)());

            float constexpr dst_channel_min =
                     (is_same<DstView, gray32f_view_t>::value) ? 0.f : static_cast<float>(
                     (std::numeric_limits<typename channel_type<DstView>::type>::min)());

            dst_view(x, y)[0] =
                     static_cast<typename channel_type<typename DstView::value_type>::type>(
                               (distance_transform > dst_channel_max) ? dst_channel_max :
                               (distance_transform < dst_channel_min) ? dst_channel_min :
                                distance_transform);
        }
    }
}

} //namespace detail

/// \addtogroup ImageProcessing
/// @{
/// \breif Calculates distance to the nearest 'on' OR 'off' pixel (as specified), 
///        for each pixel of a binary image.
/// 
/// \param src_view  - Source image view.
/// \param dst_view  - Destination image view.
/// \param dist_from - Specifies from where to find distance from, neareast, 'on' OR 'off' pixels.
/// \param dist_type - Specifies formula/method for distance calculation.
/// \tparam SrcView  - Source image view type.
/// \tparam DstView  - Destination image view type.
/// \tparam DistanceType  - Distance type choosen.
/// \tparam MaskSize      - Mask size choosen.
template<typename SrcView, typename DstView, typename DistanceType, typename MaskSize>
void distance_transform(SrcView const& src_view, DstView const& dst_view, distance_from dist_from,
                                                 DistanceType dist_type, MaskSize /*Passes type*/)
{
    gil_function_requires<ImageViewConcept<SrcView>>();
    gil_function_requires<MutableImageViewConcept<DstView>>();

    static_assert(is_same<gray_t, typename color_space_type<SrcView>::type>::value &&
                  is_same<gray_t, typename color_space_type<DstView>::type>::value,
                  "Source and destination image views must use gray color space.");

    static_assert(distance_type::check_distance_type<DistanceType>::value, 
                                                           "Distance type not recognized.");
    static_assert(mask_size::check_mask_size<MaskSize>::value, "Mask Size not recognized.");

    // Generates compile time error if invalid combination of distance_type and mask_size is used.
    static_assert(detail::dt_parameters_are_compatible<DistanceType, MaskSize>::value,
                 "distance_transform is incompatible with combination of "
                                                       "distance_type and mask_size used.");

    BOOST_ASSERT(src_view.dimensions() == dst_view.dimensions());

    if(std::is_same<MaskSize, mask_size::three_t>::value)
        distance_transform_mask_size_three_impl(src_view, dst_view, dist_from, dist_type);

    else if (std::is_same<MaskSize, mask_size::five_t>::value)
        distance_transform_mask_size_five_impl(src_view, dst_view, dist_from, dist_type);

    else
        distance_transorm_precise_impl(src_view, dst_view, dist_from);
}
/// @}
}}     // namespace boost::gil
#endif // BOOST_GIL_IMAGE_PROCESSING_DISTANCE_TRANSFORM_HPP
