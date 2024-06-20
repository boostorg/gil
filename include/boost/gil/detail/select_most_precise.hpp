#ifndef BOOST_GIL_DETAIL_SELECT_MOST_PRECISE_HPP
#define BOOST_GIL_DETAIL_SELECT_MOST_PRECISE_HPP

#include <type_traits>

namespace boost { namespace gil { namespace detail {

template <typename ...Ts>
struct dependent_false : std::integral_constant<bool, false> {};

template <bool Fundamental1, bool Fundamental2, typename T1, typename T2>
struct select_non_fundamental
{
    using type = void;
};

template <typename T1, typename T2>
struct select_non_fundamental<false, false, T1, T2>
{
    static_assert(dependent_false<T1, T2>::value, "Specialization needed, if both types are not fundamental.");
    using type = void;
};

template <typename T1, typename T2>
struct select_non_fundamental<true, false, T1, T2>
{
    using type = T2;
};

template <typename T1, typename T2>
struct select_non_fundamental<false, true, T1, T2>
{
    using type = T1;
};

template <bool FP1, bool FP2, typename T1, typename T2>
struct select_floating_point
{
    using type = void;
};

template <typename T1, typename T2>
struct select_floating_point<false, false, T1, T2>
{
    static_assert(dependent_false<T1, T2>::value, "At least one of both types has to be floating point.");
    using type = void;
};

template <typename T1, typename T2>
struct select_floating_point<true, false, T1, T2>
{
    using type = T1;
};

template <typename T1, typename T2>
struct select_floating_point<false, true, T1, T2>
{
    using type = T2;
};

template <typename T1, typename T2>
struct select_most_precise
{
    static constexpr bool second_larger = sizeof(T2) > sizeof(T1);
    static constexpr bool one_not_fundamental = !
        (std::is_fundamental<T1>::value
        & std::is_fundamental<T2>::value);

    static constexpr bool both_floating_point =
        std::is_floating_point<T1>::value
        && std::is_floating_point<T2>::value;

    using type = typename std::conditional
        <
            one_not_fundamental,
            typename select_non_fundamental
            <
                std::is_fundamental<T1>::value,
                std::is_fundamental<T2>::value,
                T1,
                T2
            >::type,
            typename std::conditional
            <
                both_floating_point,
                typename std::conditional
                <
                    second_larger,
                    T2,
                    T1
                >::type,
                typename select_floating_point
                <
                    std::is_floating_point<T1>::value,
                    std::is_floating_point<T2>::value,
                    T1,
                    T2
                >::type
            >::type
        >::type;
};

template <typename T1, typename T2>
using select_most_precise_t = typename select_most_precise<T1, T2>::type;

} } } // namespace boost::gil::detail

#endif