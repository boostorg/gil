// Copyright Tom Brinkman 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GIL_EXTENSION_TOOLBOX_METAFUNCTIONS_SHRINK_HPP
#define BOOST_GIL_EXTENSION_TOOLBOX_METAFUNCTIONS_SHRINK_HPP

#include <boost/gil.hpp>

namespace boost { namespace gil {

/// \brief Assigns a smaller subimage view of the view given as first parameter to the
/// destination/resultant view given as second parameter to the overloaded '()' operator.
/// The dimensions of the smaller view are specified by struct constructor parameters.
template <typename View>
struct shrink
{
    double left;
    double top;
    double right;
    double bottom;

    shrink(double left_param = 5, double top_param = 5, double right_param = 5,
        double bottom_param = 5)
        : left(left_param)
        , top(top_param)
        , right(right_param)
        , bottom(bottom_param)
    {
    }
 
    void operator()(View& view, View& dstView)
    {
        double const view_width = static_cast<double>(view.width());
        double const view_height = static_cast<double>(view.height());
        if (left < 1.0)
            left *= view_width;
        if (right < 1.0)
            right *= view_width;
        if (top < 1.0)
            top *= view_height;
        if (bottom < 1.0)
            bottom *= view_height;

        double const width = view_width - left - right;
        double const height = view_height - top - bottom;
        dstView = boost::gil::subimage_view(view, static_cast<std::ptrdiff_t>(left),
            static_cast<std::ptrdiff_t>(top), static_cast<std::ptrdiff_t>(width),
            static_cast<std::ptrdiff_t>(height));
    }
}; // shrink
}} // namespace boost::gil

#endif
