//
// Copyright 2020 Debabrata Mandal <mandaldebabrata123@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//

#ifndef BOOST_GIL_HISTOGRAM_HPP
#define BOOST_GIL_HISTOGRAM_HPP

#include <boost/gil/concepts/concept_check.hpp>
#include <boost/gil/metafunctions.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/container_hash/extensions.hpp>
#include <boost/mp11.hpp>

#include <type_traits>
#include <unordered_map>

namespace boost { namespace gil {

namespace detail {

template<typename... T>
struct hash_tuple {
    std::size_t operator()(std::tuple<T...> const& arg) const
    {
        return boost::hash_value(arg);
    }
};

// With C++14 and using auto we don't need the decltype anymore
template<typename Pixel, std::size_t... I>
auto pixel_to_tuple(Pixel const& p, boost::mp11::index_sequence<I...>) 
    -> decltype(std::make_tuple(p[I]...))
{
    return std::make_tuple(p[I]...);
}

template<typename Tuple, std::size_t... I>
auto tuple_to_tuple(Tuple const& t, boost::mp11::index_sequence<I...>) 
    -> decltype(std::make_tuple(std::get<I>(t)...))
{
    return std::make_tuple(std::get<I>(t)...);
}

} //namespace detail

template<typename... T>
class histogram : public std::unordered_map<std::tuple<T...>, int, detail::hash_tuple<T...> >
{
    using parent_t = std::unordered_map<std::tuple<T...>, int, detail::hash_tuple<T...> >;
    using bin_t    = boost::mp11::mp_list<T...>;
    using key_t    = typename parent_t::key_type;
    using mapped_t = typename parent_t::mapped_type;
    using value_t  = typename parent_t::value_type;
public:
    histogram() = default;

    mapped_t& operator()(T... indices) {
        auto key = std::make_tuple(indices...);
        const size_t index_dimension = std::tuple_size<std::tuple<T...>>::value;
        const size_t histogram_dimension = get_dimension();
        static_assert(histogram_dimension == index_dimension, "Dimensions do not match.");

        return parent_t::operator[](key);
    }
    
    static constexpr std::size_t get_dimension() {
        return std::tuple_size<key_t>::value;
    }

    // Optional: Implement a is_tuple_compatible() also to show error
    // before static_cast happens.
    static constexpr bool is_pixel_compatible() {
        using bin_types = boost::mp11::mp_list<T...>;
        return boost::mp11::mp_all_of<bin_types, std::is_arithmetic>::value;
    }

    template<std::size_t... Dimensions, typename Tuple>
    key_t key_from_tuple(Tuple const& t) const {
        using index_list = boost::mp11::mp_list_c<std::size_t, Dimensions...>;
        const std::size_t index_list_size = boost::mp11::mp_size<index_list>::value;
        const std::size_t tuple_size = std::tuple_size<Tuple>::value;
        const std::size_t histogram_dimension = get_dimension();

        // Tuple size check not needed if we have a is_tuple_compatible()
        static_assert( ((index_list_size != 0 && index_list_size == histogram_dimension) || 
                        (tuple_size == histogram_dimension)),
                        "Tuple and histogram key are not compatible.");
        
        using new_index_list = typename std::conditional<
                                index_list_size == 0,
                                boost::mp11::mp_list_c<std::size_t, 0>,
                                index_list>::type;

        const std::size_t min = boost::mp11::mp_min_element<
                                    new_index_list,
                                    boost::mp11::mp_less>::value;
        const std::size_t max = boost::mp11::mp_max_element<
                                    new_index_list,
                                    boost::mp11::mp_less>::value;

        static_assert( (0 <= min && max < tuple_size) || 
                        index_list_size == 0,
                        "Index out of Range");
        
        // With if-constexpr this code can be simplified
        using sequence_type = typename std::conditional<
                                index_list_size == 0,
                                boost::mp11::make_index_sequence<histogram_dimension>,
                                boost::mp11::index_sequence<Dimensions...>>::type;

        auto key = detail::tuple_to_tuple(t, sequence_type{});
        return cast_to_histogram_key(key, boost::mp11::make_index_sequence<histogram_dimension>{});
    }


    template<std::size_t... Dimensions, typename Pixel>
    key_t key_from_pixel(Pixel const& p) const {
        using index_list = boost::mp11::mp_list_c<std::size_t, Dimensions...>;
        const std::size_t index_list_size = boost::mp11::mp_size<index_list>::value;
        const std::size_t pixel_dimension = num_channels<Pixel>::value;
        const std::size_t histogram_dimension = get_dimension();

        static_assert( ((index_list_size != 0 && index_list_size == histogram_dimension) || 
                        (index_list_size == 0 && pixel_dimension == histogram_dimension)) &&
                        is_pixel_compatible(),
                        "Pixels and histogram key are not compatible.");

        using  new_index_list = typename std::conditional<
                                index_list_size == 0,
                                boost::mp11::mp_list_c<std::size_t, 0>,
                                index_list>::type;

        const std::size_t min = boost::mp11::mp_min_element<
                                    new_index_list,
                                    boost::mp11::mp_less>::value;
        const std::size_t max = boost::mp11::mp_max_element<
                                    new_index_list,
                                    boost::mp11::mp_less>::value;

        static_assert( (0 <= min && max < pixel_dimension) || 
                        index_list_size == 0,
                        "Index out of Range");
        
        using sequence_type = typename std::conditional<
                                index_list_size == 0,
                                boost::mp11::make_index_sequence<histogram_dimension>,
                                boost::mp11::index_sequence<Dimensions...>>::type;

        auto key =  detail::pixel_to_tuple(p, sequence_type{});
        return cast_to_histogram_key(key, boost::mp11::make_index_sequence<histogram_dimension>{});
    }

    template<std::size_t... Dimensions, typename SrcView>
    void fill(SrcView const& srcview) {
        gil_function_requires<ImageViewConcept<SrcView>>();
        static constexpr std::size_t image_dimension = num_channels<SrcView>::value;
        using dimension_list = boost::mp11::mp_list_c<size_t, Dimensions...>;
        using index_list = boost::mp11::index_sequence<Dimensions...>;
        using pixel_t = typename SrcView::value_type;

        static constexpr std::size_t num_dimensions = boost::mp11::mp_size<dimension_list>::value;

        for_each_pixel(srcview, [&](pixel_t const& p){
            parent_t::operator[](key_from_pixel<Dimensions...>(p))++;
        });
    }

    template<std::size_t... Dimensions, typename Tuple>
    histogram sub_histogram(Tuple const& t) {
        using index_list = boost::mp11::mp_list_c<std::size_t, Dimensions...>;
        const std::size_t index_list_size = boost::mp11::mp_size<index_list>::value;
        const std::size_t histogram_dimension = get_dimension();

        const std::size_t min = boost::mp11::mp_min_element<
                                    index_list,
                                    boost::mp11::mp_less>::value;
        const std::size_t max = boost::mp11::mp_max_element<
                                    index_list,
                                    boost::mp11::mp_less>::value;
        // Also check is_tuple_compatible()
        static_assert( (0 <= min && max < histogram_dimension) && 
                        index_list_size < histogram_dimension,
                        "Index out of Range");

        auto key = cast_to_histogram_key(t, boost::mp11::make_index_sequence<get_dimension()>{});
        auto sub_key = detail::tuple_to_tuple(key, boost::mp11::index_sequence<Dimensions...>{});
        
        histogram sub_h;
        std::for_each( parent_t::begin(), parent_t::end(), [&](value_t const& k) {
            if (sub_key == detail::tuple_to_tuple(k.first, 
                                        boost::mp11::index_sequence<Dimensions...>{}))
                sub_h[k.first] += parent_t::operator[](k.first);
        });
        return sub_h;
    }

    // template
    // <
    //     std::size_t... Dimensions
    // >
    // histogram<boost::mp11::mp_at_c<bin_t, Dimensions>...> sub_histogram() {
    //     using index_list = boost::mp11::mp_list_c<std::size_t, Dimensions...>;
    //     const std::size_t index_list_size = boost::mp11::mp_size<index_list>::value;
    //     const std::size_t histogram_dimension = get_dimension();

    //     const std::size_t min = boost::mp11::mp_min_element<
    //                                 index_list,
    //                                 boost::mp11::mp_less>::value;
    //     const std::size_t max = boost::mp11::mp_max_element<
    //                                 index_list,
    //                                 boost::mp11::mp_less>::value;
    //     // Also check is_tuple_compatible()
    //     static_assert( (0 <= min && max < histogram_dimension) && 
    //                     index_list_size < histogram_dimension,
    //                     "Index out of Range");
                        
    //     // histogram<boost::mp11::mp_at_c<bin_t, Dimensions>...> sub_h;

    //     // std::for_each( parent_t::begin(), parent_t::end(), [&](value_t const& k) {
    //     //     auto sub_key = detail::tuple_to_tuple(k.first, 
    //     //                                 boost::mp11::index_sequence<Dimensions...>{});
    //     //     sub_h[sub_key] += parent_t::operator[](k.first);
    //     // });
    //     // return sub_h;
    // }

private:
    
    template<typename Tuple, std::size_t... I>
    key_t cast_to_histogram_key(Tuple const& t, boost::mp11::index_sequence<I...>) const
    {
        using bin_types = boost::mp11::mp_list<T...>;
        return std::make_tuple(
                    static_cast<typename boost::mp11::mp_at_c<bin_types, I>>(std::get<I>(t))...);
    }
};

template<typename... T>
struct check_struct {
    using type_list = boost::mp11::mp_list<T...>;
    template<std::size_t... D>
    check_struct<boost::mp11::mp_at_c<type_list, D>...> check() {
        check_struct<boost::mp11::mp_at_c<type_list, D>...> f;
        return f;
    }
};

///
/// fill_histogram - Overload this function to provide support for
/// boost::gil::histogram or any other external histogram
///

template<typename SrcView, typename Container>
void fill_histogram(SrcView const& srcview, Container& h);

template<std::size_t... Dimensions, typename SrcView, typename... T>
void fill_histogram(SrcView const& srcview, histogram<T...> &h, bool accumulate = false)
{
    if(!accumulate)
        h.clear();
    h.template fill<Dimensions...>(srcview);
}

}} //namespace boost::gil

#endif
