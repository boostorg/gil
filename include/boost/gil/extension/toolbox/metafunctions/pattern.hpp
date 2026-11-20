// Copyright Tom Brinkman 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GIL_EXTENSION_TOOLBOX_METAFUNCTIONS_PATTERN_HPP
#define BOOST_GIL_EXTENSION_TOOLBOX_METAFUNCTIONS_PATTERN_HPP

#include <boost/gil.hpp>

namespace boost { namespace gil {

/// \brief Repeatedly copies smaller image view passed through the struct constructor in subimage
/// views of the larger image view passed through overloaded '()' operator.
template <typename View>
struct pattern
{
    View parent_view;
    pattern(View view)
        : parent_view(view)
    {
    }

    void operator()(View& view)
    {
        long int const parent_view_height = parent_view.height();
        long int const parent_view_width = parent_view.width();
        long int const view_height = view.height(), view_width = view.width();
        
        // For ensuring that view passed through '()' operator has dimensions greater than or
        // equal to the dimensions of view passed through constructor.
        if (parent_view_height > view_height || parent_view_width > view_width)
        {
            throw std::length_error("Image view passed through overloaded '()' operator must have"
            " dimensions greater than or equal to the dimensions of image view passed through"
            " struct constructor");
        }

        for (std::ptrdiff_t x = 0; x < view_width; x += parent_view_width)
        {
            for (std::ptrdiff_t y = 0; y < view_height; y += parent_view_height)
            {
                std::ptrdiff_t aw = x + parent_view_width > view_width ? view_width - x :
                    parent_view_width;

                std::ptrdiff_t ah = y + parent_view_height > view_height ? view_height - y :
                    parent_view_height;

                View v3 = subimage_view(view, x, y, aw, ah);
                View v4 = subimage_view(parent_view, 0, 0, aw, ah);
                copy_pixels(v4, v3);
            }
        }
    }
}; // pattern
}} // namespace boost::gil
#endif
