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
#include <boost/type_traits.hpp>
#include <boost/functional/hash.hpp>

#include <iostream>
#include <tuple>
#include <utility>
#include <vector>
#include <type_traits>
#include <unordered_map>

namespace boost { namespace gil {

//////////////////////////////////////////////////////////
/// Histogram
//////////////////////////////////////////////////////////
/// \defgroup Histogram Histogram
/// \brief Contains description of the boost.gil.histogram class, extensions provided in place
///        of the default class, algorithms over the histogram class (both extensions and the
///        default class)
///

namespace detail {

template <std::size_t Index, typename... T>
inline typename std::enable_if<Index == sizeof...(T), void>::type
    hash_tuple_impl(std::size_t&, std::tuple<T...> const&)
{
}

template <std::size_t Index, typename... T>
inline typename std::enable_if<Index != sizeof...(T), void>::type
    hash_tuple_impl(std::size_t& seed, std::tuple<T...> const& t)
{
    boost::hash_combine(seed, std::get<Index>(t));
    hash_tuple_impl<Index + 1>(seed, t);
}

/*  
    Functor provided for the hashing of tuples. 
    The following approach makes use hash_combine from 
    boost::container_hash. Although there is a direct hashing 
    available for tuples, this approach will ease adopting in
    future to a std::hash_combine. In case std::hash extends
    support to tuples this functor as well as the helper 
    implementation hash_tuple_impl can be removed.
*/
template <typename... T>
struct hash_tuple
{
    std::size_t operator()(std::tuple<T...> const& t) const
    {
        std::size_t seed = 0;
        hash_tuple_impl<0>(seed, t);
        return seed;
    }
};

// TODO: With C++14 and using auto we don't need the decltype anymore
template <typename Pixel, std::size_t... I>
auto pixel_to_tuple(Pixel const& p, boost::mp11::index_sequence<I...>)
    -> decltype(std::make_tuple(p[I]...))
{
    return std::make_tuple(p[I]...);
}

// TODO: With C++14 and using auto we don't need the decltype anymore
template <typename Tuple, std::size_t... I>
auto tuple_to_tuple(Tuple const& t, boost::mp11::index_sequence<I...>)
    -> decltype(std::make_tuple(std::get<I>(t)...))
{
    return std::make_tuple(std::get<I>(t)...);
}

template <typename Tuple, std::size_t... I>
bool tuple_compare(Tuple const& t1, Tuple const& t2, boost::mp11::index_sequence<I...>)
{
    std::array<bool, std::tuple_size<Tuple>::value> comp_list;
    comp_list = {std::get<I>(t1) <= std::get<I>(t2)...};
    bool comp = true;
    for (std::size_t i = 0; i < comp_list.size(); i++)
    {
        comp = comp & comp_list[i];
    }
    return comp;
}

template <typename Tuple>
bool tuple_compare(Tuple const& t1, Tuple const& t2)
{
    std::size_t const tuple_size = std::tuple_size<Tuple>::value;
    auto index_list = boost::mp11::make_index_sequence<tuple_size>{};
    return tuple_compare(t1, t2, index_list);
}

template<std::size_t Dimension>
struct filler
{
    template<typename Container, typename Tuple>
    void operator()(Container& hist, Tuple& lower, Tuple& upper) {}
};

template<>
struct filler<1>
{
    template<typename Container, typename Tuple>
    void operator()(Container& hist, Tuple& lower, Tuple& upper)
    {
        for(auto i = std::get<0>(lower); i < std::get<0>(upper); ++i)
        {
            hist[i] = 0;
        }
        hist[std::get<0>(upper)]=0;
    }
};

}  //namespace detail

///
/// \class boost::gil::histogram
/// \ingroup Histogram
/// \brief Default histogram class provided by boost::gil.
///
/// The class inherits over the std::unordered_map provided by STL. A complete example/tutorial
/// of how to use the class resides in the docs. 
/// Simple calling syntax for a 3D dimensional histogram :
/// \code
/// histogram<int, int , int> h;
/// h(1, 1, 1) = 0;
/// \endcode
/// This is just a starter to what all can be achieved with it, refer to the docs for the 
/// full demo.
///
template <typename... T>
class histogram : public std::unordered_map<std::tuple<T...>, double, detail::hash_tuple<T...>>
{
    using base_t   = std::unordered_map<std::tuple<T...>, double, detail::hash_tuple<T...>>;
    using bin_t    = boost::mp11::mp_list<T...>;
    using key_t    = typename base_t::key_type;
    using mapped_t = typename base_t::mapped_type;
    using value_t  = typename base_t::value_type;

public:
    histogram() = default;

    /// \brief Returns the number of dimensions(axes) the class supports.
    static constexpr std::size_t dimension()
    {
        return std::tuple_size<key_t>::value;
    }

    /// \brief Returns bin value corresponding to specified tuple
    mapped_t& operator()(T... indices)
    {
        auto key                              = std::make_tuple(indices...);
        std::size_t const index_dimension     = std::tuple_size<std::tuple<T...>>::value;
        std::size_t const histogram_dimension = dimension();
        static_assert(histogram_dimension == index_dimension, "Dimensions do not match.");

        return base_t::operator[](key);
    }
    
    /// \brief Checks if the histogram class is compatible to be used with
    ///        a GIL image type
    static constexpr bool is_pixel_compatible()
    {
        using bin_types = boost::mp11::mp_list<T...>;
        return boost::mp11::mp_all_of<bin_types, std::is_arithmetic>::value;
    }

    /// \brief Checks if the histogram class is compatible to be used with
    ///        the specified tuple type
    template <typename Tuple>
    bool is_tuple_compatible(Tuple const&)
    {
        std::size_t const tuple_size     = std::tuple_size<Tuple>::value;
        std::size_t const histogram_size = dimension();
        // TODO : Explore consequence of using if-constexpr
        using sequence_type = typename std::conditional
        <
            tuple_size >= histogram_size,
            boost::mp11::make_index_sequence<histogram_size>,
            boost::mp11::make_index_sequence<tuple_size>
        >::type;

        if (is_tuple_size_compatible<Tuple>())
            return is_tuple_type_compatible<Tuple>(sequence_type{});
        else
            return false;
    }

    /// \brief Returns a key compatible to be used as the histogram key
    ///        from the input tuple
    template <std::size_t... Dimensions, typename Tuple>
    key_t key_from_tuple(Tuple const& t) const
    {
        using index_list                      = boost::mp11::mp_list_c<std::size_t, Dimensions...>;
        std::size_t const index_list_size     = boost::mp11::mp_size<index_list>::value;
        std::size_t const tuple_size          = std::tuple_size<Tuple>::value;
        std::size_t const histogram_dimension = dimension();

        static_assert(
            ((index_list_size != 0 && index_list_size == histogram_dimension) ||
             (tuple_size == histogram_dimension)),
            "Tuple and histogram key of different sizes");

        using new_index_list = typename std::conditional
        <
            index_list_size == 0,
            boost::mp11::mp_list_c<std::size_t, 0>,
            index_list
        >::type;

        std::size_t const min =
            boost::mp11::mp_min_element<new_index_list, boost::mp11::mp_less>::value;

        std::size_t const max =
            boost::mp11::mp_max_element<new_index_list, boost::mp11::mp_less>::value;

        static_assert((0 <= min && max < tuple_size) || index_list_size == 0, "Index out of Range");

        using seq1 = boost::mp11::make_index_sequence<histogram_dimension>;
        using seq2 = boost::mp11::index_sequence<Dimensions...>;
        // TODO : Explore consequence of using if-constexpr
        using sequence_type = typename std::conditional<index_list_size == 0, seq1, seq2>::type;

        auto key = detail::tuple_to_tuple(t, sequence_type{});
        static_assert(
            is_tuple_type_compatible<Tuple>(seq1{}),
            "Tuple type and histogram type not compatible.");

        return make_histogram_key(key, seq1{});
    }

    /// \brief Returns a histogram compatible key from the input pixel which
    ///        can be directly used
    template <std::size_t... Dimensions, typename Pixel>
    key_t key_from_pixel(Pixel const& p) const
    {
        using index_list                      = boost::mp11::mp_list_c<std::size_t, Dimensions...>;
        std::size_t const index_list_size     = boost::mp11::mp_size<index_list>::value;
        std::size_t const pixel_dimension     = num_channels<Pixel>::value;
        std::size_t const histogram_dimension = dimension();

        static_assert(
            ((index_list_size != 0 && index_list_size == histogram_dimension) ||
            (index_list_size == 0 && pixel_dimension == histogram_dimension)) &&
            is_pixel_compatible(),
            "Pixels and histogram key are not compatible.");

        using  new_index_list = typename std::conditional
        <
            index_list_size == 0,
            boost::mp11::mp_list_c<std::size_t, 0>,
            index_list
        >::type;

        std::size_t const min =
            boost::mp11::mp_min_element<new_index_list, boost::mp11::mp_less>::value;

        std::size_t const max =
            boost::mp11::mp_max_element<new_index_list, boost::mp11::mp_less>::value;

        static_assert(
            (0 <= min && max < pixel_dimension) || index_list_size == 0, "Index out of Range");

        using seq1          = boost::mp11::make_index_sequence<histogram_dimension>;
        using seq2          = boost::mp11::index_sequence<Dimensions...>;
        using sequence_type = typename std::conditional<index_list_size == 0, seq1, seq2>::type;

        auto key = detail::pixel_to_tuple(p, sequence_type{});
        return make_histogram_key(key, seq1{});
    }

    /// \brief Return nearest smaller key to specified histogram key
    key_t nearest_key(key_t const& k) const
    {
        using check_list = boost::mp11::mp_list<boost::has_less<T>...>;
        static_assert(
            boost::mp11::mp_all_of<check_list, boost::mp11::mp_to_bool>::value,
            "Keys are not comparable.");
        auto nearest_k = k;
        if (base_t::find(k) != base_t::end())
        {
            return nearest_k;
        }
        else
        {
            bool once = true;
            std::for_each(base_t::begin(), base_t::end(), [&](value_t const& v) {
                if (v.first <= k)
                {
                    if (once)
                    {
                        once      = !once;
                        nearest_k = v.first;
                    }
                    else if (nearest_k < v.first)
                        nearest_k = v.first;
                }
            });
            return nearest_k;
        }
    }

    /// \brief Fills the histogram with the input image view
    template <std::size_t... Dimensions, typename SrcView>
    void fill(
        SrcView const& srcview,
        bool applymask = false,
        std::vector<std::vector<bool>> mask = {},  
        key_t lower = key_t(),
        key_t upper = key_t(),
        bool setlimits = false)
    {
        gil_function_requires<ImageViewConcept<SrcView>>();

        for(std::ptrdiff_t src_y = 0; src_y < srcview.height(); ++src_y)
        {
            auto src_it = srcview.row_begin(src_y);
            for(std::ptrdiff_t src_x = 0; src_x < srcview.width(); ++src_x)
            {
                if(applymask && !mask[src_y][src_x])
                    continue;
                auto key = key_from_pixel<Dimensions...>(src_it[src_x]);
                if(!setlimits||(detail::tuple_compare(lower, key)&&detail::tuple_compare(key, upper)))
                    base_t::operator[](key)++;
            }
        }
    }

    /// \brief Can return a subset or a mask over the current histogram
    template <std::size_t... Dimensions, typename Tuple>
    histogram sub_histogram(Tuple const& t1, Tuple const& t2)
    {
        using index_list                      = boost::mp11::mp_list_c<std::size_t, Dimensions...>;
        std::size_t const index_list_size     = boost::mp11::mp_size<index_list>::value;
        std::size_t const histogram_dimension = dimension();

        std::size_t const min =
            boost::mp11::mp_min_element<index_list, boost::mp11::mp_less>::value;

        std::size_t const max =
            boost::mp11::mp_max_element<index_list, boost::mp11::mp_less>::value;

        static_assert(
            (0 <= min && max < histogram_dimension) && index_list_size < histogram_dimension,
            "Index out of Range");

        using seq1 = boost::mp11::make_index_sequence<dimension()>;
        using seq2 = boost::mp11::index_sequence<Dimensions...>;

        static_assert(
            is_tuple_type_compatible<Tuple>(seq1{}),
            "Tuple type and histogram type not compatible.");

        auto low      = make_histogram_key(t1, seq1{});
        auto low_key  = detail::tuple_to_tuple(low, seq2{});
        auto high     = make_histogram_key(t2, seq1{});
        auto high_key = detail::tuple_to_tuple(high, seq2{});

        histogram sub_h;
        std::for_each(base_t::begin(), base_t::end(), [&](value_t const& k) {
            auto tmp_key = detail::tuple_to_tuple(k.first, seq2{});
            if (low_key <= tmp_key && tmp_key <= high_key)
                sub_h[k.first] += base_t::operator[](k.first);
        });
        return sub_h;
    }

    /// \brief Returns a sub-histogram over specified axes
    template <std::size_t... Dimensions>
    histogram<boost::mp11::mp_at<bin_t, boost::mp11::mp_size_t<Dimensions>>...> sub_histogram()
    {
        using index_list                      = boost::mp11::mp_list_c<std::size_t, Dimensions...>;
        std::size_t const index_list_size     = boost::mp11::mp_size<index_list>::value;
        std::size_t const histogram_dimension = dimension();

        std::size_t const min =
            boost::mp11::mp_min_element<index_list, boost::mp11::mp_less>::value;

        std::size_t const max =
            boost::mp11::mp_max_element<index_list, boost::mp11::mp_less>::value;

        static_assert(
            (0 <= min && max < histogram_dimension) && index_list_size < histogram_dimension,
            "Index out of Range");

        histogram<boost::mp11::mp_at<bin_t, boost::mp11::mp_size_t<Dimensions>>...> sub_h;

        std::for_each(base_t::begin(), base_t::end(), [&](value_t const& v) {
            auto sub_key =
                detail::tuple_to_tuple(v.first, boost::mp11::index_sequence<Dimensions...>{});
            sub_h[sub_key] += base_t::operator[](v.first);
        });
        return sub_h;
    }

    /// \brief Normalize this histogram class 
    void normalize()
    {
        double sum = 0.0;
        std::for_each(base_t::begin(), base_t::end(), [&](value_t const& v) {
            sum += v.second;
        });
        std::cout<<(long int)sum<<"asfe";
        std::for_each(base_t::begin(), base_t::end(), [&](value_t const& v) {
            base_t::operator[](v.first) = v.second / sum;
        });
    }

private:
    template <typename Tuple, std::size_t... I>
    key_t make_histogram_key(Tuple const& t, boost::mp11::index_sequence<I...>) const
    {
        return std::make_tuple(
            static_cast<typename boost::mp11::mp_at<bin_t, boost::mp11::mp_size_t<I>>>(
                std::get<I>(t))...);
    }

    template <typename Tuple, std::size_t... I>
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

    template <typename Tuple>
    static constexpr bool is_tuple_size_compatible()
    {
        return (std::tuple_size<Tuple>::value == dimension());
    }
};

///
/// \fn void fill_histogram(SrcView const&, Container&)
/// \ingroup Histogram
/// \tparam SrcView Input image view
/// \tparam Container Input histogram container
/// \brief Overload this function to provide support for boost::gil::histogram or 
/// any other external histogram
///
/// Example :
/// \code
/// histogram<int> h;
/// fill_histogram(view(img), h);
/// \endcode
///
template <typename SrcView, typename Container>
void fill_histogram(SrcView const&, Container&);

///
/// \overload void fill_histogram(SrcView const&, histogram<T...>&, bool)
/// \ingroup Histogram
/// \brief Overload version of fill_histogram 
///
/// Takes a third argument to determine whether to clear container before filling.
/// For eg, when there is a need to accumulate the histograms do
/// \code
/// fill_histogram(view(img), hist, true);
/// \endcode
///
template <std::size_t... Dimensions, typename SrcView, typename... T>
void fill_histogram(
    SrcView const& srcview,
    histogram<T...>& hist,
    bool accumulate = false,
    bool sparsefill = true,
    bool applymask = false,
    std::vector<std::vector<bool>> mask = {},
    typename histogram<T...>::key_type lower = {},
    typename histogram<T...>::key_type upper = {},
    bool setlimits = false)
{
    if (!accumulate)
        hist.clear();
    
    detail::filler<histogram<T...>::dimension()> f;
    if(!sparsefill)
        f(hist, lower, upper);
    
    hist.template fill<Dimensions...>(srcview, applymask, mask, lower, upper, setlimits);
}

///
/// \fn void cumulative_histogram(Container&)
/// \ingroup Histogram Algorithms
/// \tparam Container Input histogram container
/// \brief Optionally overload this function with any external histogram class
///
/// Cumulative histogram is calculated over any arbitrary dimensional
/// histogram. The only tradeoff could be the runtime complexity which in
/// the worst case would be max( #pixel_values , #bins ) * #dimensions.
/// For single dimensional histograms the complexity has been brought down to
/// #bins * log( #bins ) by sorting the keys and then calculating the cumulative version.
///
template <typename Container, typename >
Container cumulative_histogram(Container&);

template <typename... T>
histogram<T...> cumulative_histogram(histogram<T...>& hist)
{
    using check_list = boost::mp11::mp_list<boost::has_less<T>...>;
    static_assert(
        boost::mp11::mp_all_of<check_list, boost::mp11::mp_to_bool>::value,
        "Cumulative histogram not possible of this type");
    
    using histogram_t = histogram<T...>;
    using pair_t  = std::pair<typename histogram_t::key_type, typename histogram_t::mapped_type>;
    using value_t = typename histogram_t::value_type;

    histogram_t cumulative_hist;
    std::size_t const dims = histogram_t::dimension();
    if (dims == 1)
    {
        std::vector<pair_t> sorted_keys(hist.size());
        std::size_t counter = 0;
        std::for_each(hist.begin(), hist.end(), [&](value_t const& v) {
            sorted_keys[counter++] = std::make_pair(v.first, v.second);
        });
        std::sort(sorted_keys.begin(), sorted_keys.end());
        auto cumulative_counter = static_cast<typename histogram_t::mapped_type>(0);
        for (std::size_t i = 0; i < sorted_keys.size(); ++i)
        {
            cumulative_counter += sorted_keys[i].second;
            cumulative_hist[(sorted_keys[i].first)] = cumulative_counter;
        }
    }
    else
    {
        std::for_each(hist.begin(), hist.end(), [&](value_t const& v1) {
            auto cumulative_counter = static_cast<typename histogram_t::mapped_type>(0);
            std::for_each(hist.begin(), hist.end(), [&](value_t const& v2) {
                bool comp = detail::tuple_compare(
                    v2.first, v1.first,
                    boost::mp11::make_index_sequence<histogram_t::dimension()>{});
                if (comp)
                    cumulative_counter += hist[v2.first];
            });
            cumulative_hist[v1.first] = cumulative_counter;
        });
    }
    return cumulative_hist;
}

}}  //namespace boost::gil

#endif
