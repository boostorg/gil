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
#include <boost/mp11.hpp>
#include <boost/functional/hash.hpp>

#include <type_traits>
#include <unordered_map>

namespace boost { namespace gil {

namespace detail {

template<typename... T>
struct hash_tuple
{
    std::size_t operator()(std::tuple<T...> const& arg) const
    {
        return boost::hash_value(arg);
    }
};

// TODO: With C++14 and using auto we don't need the decltype anymore
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
    using base_t   = std::unordered_map<std::tuple<T...>, int, detail::hash_tuple<T...> >;
    using bin_t    = boost::mp11::mp_list<T...>;
    using key_t    = typename base_t::key_type;
    using mapped_t = typename base_t::mapped_type;
    using value_t  = typename base_t::value_type;
public:
    histogram() = default;

    static constexpr std::size_t dimension() 
    {
        return std::tuple_size<key_t>::value;
    }

    mapped_t& operator()(T... indices) 
    {
        auto key = std::make_tuple(indices...);
        std::size_t const index_dimension = std::tuple_size<std::tuple<T...>>::value;
        std::size_t const histogram_dimension = dimension();
        static_assert(histogram_dimension == index_dimension, "Dimensions do not match.");

        return base_t::operator[](key);
    }

    static constexpr bool is_pixel_compatible() 
    {
        using bin_types = boost::mp11::mp_list<T...>;
        return boost::mp11::mp_all_of<bin_types, std::is_arithmetic>::value;
    }

    template<typename Tuple>
    bool is_tuple_compatible(Tuple const& t) 
    {
        std::size_t const tuple_size = std::tuple_size<Tuple>::value;
        std::size_t const histogram_size = dimension();
        // This code can be reduced by using if-constexpr
        using sequence_type = typename std::conditional
        <
            tuple_size >= histogram_size,
            boost::mp11::make_index_sequence<histogram_size>,
            boost::mp11::make_index_sequence<tuple_size>
        >::type;
                                
        if(is_tuple_size_compatible(t))
            return is_tuple_type_compatible<Tuple>(sequence_type{});
        else
            return false;
    }

    template<std::size_t... Dimensions, typename Tuple>
    key_t key_from_tuple(Tuple const& t) const 
    {
        using index_list = boost::mp11::mp_list_c<std::size_t, Dimensions...>;
        std::size_t const index_list_size = boost::mp11::mp_size<index_list>::value;
        std::size_t const tuple_size = std::tuple_size<Tuple>::value;
        std::size_t const histogram_dimension = dimension();

        static_assert(  ((index_list_size != 0 && index_list_size == histogram_dimension) ||
                        (tuple_size == histogram_dimension)),
                        "Tuple and histogram key of different sizes");
        
        using new_index_list = typename std::conditional
        <
            index_list_size == 0,
            boost::mp11::mp_list_c<std::size_t, 0>,
            index_list
        >::type;

        std::size_t const min = boost::mp11::mp_min_element
        <
            new_index_list,
            boost::mp11::mp_less
        >::value;

        std::size_t const max = boost::mp11::mp_max_element
        <
            new_index_list,
            boost::mp11::mp_less
        >::value;

        static_assert(  (0 <= min && max < tuple_size) || 
                        index_list_size == 0,
                        "Index out of Range");
        
        using seq1 = boost::mp11::make_index_sequence<histogram_dimension>;
        using seq2 = boost::mp11::index_sequence<Dimensions...>;
        // With if-constexpr this code can be simplified
        using sequence_type = typename std::conditional
        <
            index_list_size == 0,
            seq1,
            seq2
        >::type;

        auto key = detail::tuple_to_tuple(t, sequence_type{});
        static_assert(  is_tuple_type_compatible<Tuple>(seq1{}), 
                        "Tuple type and histogram type not compatible.");
        
        return make_histogram_key(key, seq1{});
    }


    template<std::size_t... Dimensions, typename Pixel>
    key_t key_from_pixel(Pixel const& p) const 
    {
        using index_list = boost::mp11::mp_list_c<std::size_t, Dimensions...>;
        std::size_t const index_list_size = boost::mp11::mp_size<index_list>::value;
        std::size_t const pixel_dimension = num_channels<Pixel>::value;
        std::size_t const histogram_dimension = dimension();

        static_assert(  ((index_list_size != 0 && index_list_size == histogram_dimension) ||
                        (index_list_size == 0 && pixel_dimension == histogram_dimension)) &&
                        is_pixel_compatible(),
                        "Pixels and histogram key are not compatible.");

        using  new_index_list = typename std::conditional
        <
            index_list_size == 0,
            boost::mp11::mp_list_c<std::size_t, 0>,
            index_list
        >::type;

        std::size_t const min = boost::mp11::mp_min_element
        <
            new_index_list,
            boost::mp11::mp_less
        >::value;
        
        std::size_t const max = boost::mp11::mp_max_element
        <
            new_index_list,
            boost::mp11::mp_less
        >::value;

        static_assert(  (0 <= min && max < pixel_dimension) || 
                        index_list_size == 0,
                        "Index out of Range");
        
        using seq1 = boost::mp11::make_index_sequence<histogram_dimension>;
        using seq2 = boost::mp11::index_sequence<Dimensions...>;
        using sequence_type = typename std::conditional
        <
            index_list_size == 0,
            seq1,
            seq2
        >::type;

        auto key =  detail::pixel_to_tuple(p, sequence_type{});
        return make_histogram_key(key, seq1{});
    }

    template<std::size_t... Dimensions, typename SrcView>
    void fill(SrcView const& srcview) 
    {
        gil_function_requires<ImageViewConcept<SrcView>>();
        using pixel_t = typename SrcView::value_type;

        for_each_pixel(srcview, [&](pixel_t const& p){
            base_t::operator[](key_from_pixel<Dimensions...>(p))++;
        });
    }

    template<std::size_t... Dimensions, typename Tuple>
    histogram sub_histogram(Tuple const& t) 
    {
        using index_list = boost::mp11::mp_list_c<std::size_t, Dimensions...>;
        std::size_t const index_list_size = boost::mp11::mp_size<index_list>::value;
        std::size_t const histogram_dimension = dimension();

        std::size_t const min = boost::mp11::mp_min_element
        <
            index_list,
            boost::mp11::mp_less
        >::value;

        std::size_t const max = boost::mp11::mp_max_element
        <
            index_list,
            boost::mp11::mp_less
        >::value;

        static_assert(  (0 <= min && max < histogram_dimension) && 
                        index_list_size < histogram_dimension,
                        "Index out of Range");

        using seq1 = boost::mp11::make_index_sequence<dimension()>;
        using seq2 = boost::mp11::index_sequence<Dimensions...>;

        static_assert(  is_tuple_type_compatible<Tuple>(seq1{}), 
                        "Tuple type and histogram type not compatible.");

        auto key = make_histogram_key(t, seq1{});
        auto sub_key = detail::tuple_to_tuple(key, seq2{});
        
        histogram sub_h;
        std::for_each(base_t::begin(), base_t::end(), [&](value_t const& k) {
            if (sub_key == detail::tuple_to_tuple(k.first, seq2{}))
                sub_h[k.first] += base_t::operator[](k.first);
        });
        return sub_h;
    }

    template<std::size_t... Dimensions>
    histogram<boost::mp11::mp_at<bin_t, boost::mp11::mp_size_t<Dimensions>>...> sub_histogram() 
    {
        using index_list = boost::mp11::mp_list_c<std::size_t, Dimensions...>;
        std::size_t const index_list_size = boost::mp11::mp_size<index_list>::value;
        std::size_t const histogram_dimension = dimension();

        std::size_t const min = boost::mp11::mp_min_element
        <
            index_list,
            boost::mp11::mp_less
        >::value;

        std::size_t const max = boost::mp11::mp_max_element
        <
            index_list,
            boost::mp11::mp_less
        >::value;

        static_assert(  (0 <= min && max < histogram_dimension) && 
                        index_list_size < histogram_dimension,
                        "Index out of Range");
          
        histogram<boost::mp11::mp_at<bin_t, boost::mp11::mp_size_t<Dimensions>>...> sub_h;

        std::for_each(base_t::begin(), base_t::end(), [&](value_t const& k) {
            auto sub_key = detail::tuple_to_tuple
            (
                k.first, 
                boost::mp11::index_sequence<Dimensions...>{}
            );
            sub_h[sub_key] += base_t::operator[](k.first);
        });
        return sub_h;
    }

private:
    
    template<typename Tuple, std::size_t... I>
    key_t make_histogram_key(Tuple const& t, boost::mp11::index_sequence<I...>) const
    {
        return std::make_tuple(static_cast
        <
            typename boost::mp11::mp_at
            <
                bin_t, 
                boost::mp11::mp_size_t<I>
            >
        >(std::get<I>(t))...);
    }

    template<typename Tuple, std::size_t... I>
    static constexpr bool is_tuple_type_compatible(boost::mp11::index_sequence<I...>)
    {
        using tp = boost::mp11::mp_list
        <
            typename std::is_convertible
                <
                    boost::mp11::mp_at<bin_t, boost::mp11::mp_size_t<I>>,
                    typename std::tuple_element<I, Tuple>::type
                >::type...
        >;
        return boost::mp11::mp_all_of<tp, boost::mp11::mp_to_bool>::value;
    }

    template<typename Tuple>
    static constexpr bool is_tuple_size_compatible(Tuple const& t)
    {
        return (std::tuple_size<Tuple>::value == dimension());
    }
};

///
/// fill_histogram - Overload this function to provide support for
/// boost::gil::histogram or any other external histogram
///

template<typename SrcView, typename Container>
void fill_histogram(SrcView const& srcview, Container& h);

template<std::size_t... Dimensions, typename SrcView, typename... T>
void fill_histogram(SrcView const& srcview, histogram<T...> &histogram, bool accumulate = false)
{
    if(!accumulate)
        histogram.clear();
    histogram.template fill<Dimensions...>(srcview);
}

}} //namespace boost::gil

#endif
