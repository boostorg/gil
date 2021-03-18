// Copyright Tom Brinkman 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef _shrink_hpp_
#define _shrink_hpp_

#include <boost/gil.hpp>

namespace boost  {  namespace gil  {

/// \brief Assigns a smaller subimage view of the view given as first parameter to the
/// destination/resultant view given as second parameter to the overloaded '()' operator.
/// The dimensions of the smaller view are specified by struct constructor parameters.
template <typename view_t>
struct shrink
{
    double left;
    double top;
    double right;
    double bottom;

    shrink(double left = 5, double top = 5, double right = 5, double bottom = 5)
        : left(left)
        , top(top)
        , right(right)
        , bottom(bottom)
    {
    }

    void operator()(view_t& view, view_t& dstView)
    {
        if(left < 1.0)
            left *= view.width();
        if(right < 1.0)
            right *= view.width();
        if(top < 1.0)
            top *= view.height();
        if(bottom < 1.0)
            bottom *= view.height();

        double width = view.width() - left - right;
        double height = view.height() - top - bottom;
        dstView = boost::gil::subimage_view(view, static_cast<std::ptrdiff_t>(left),
        static_cast<std::ptrdiff_t>(top), static_cast<std::ptrdiff_t>(width),
        static_cast<std::ptrdiff_t>(height));
    }
};
}  }

#endif
