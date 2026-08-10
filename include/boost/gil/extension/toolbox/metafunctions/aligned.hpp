// Copyright Tom Brinkman 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GIL_EXTENSION_TOOLBOX_METAFUNCTIONS_ALIGNED_HPP
#define BOOST_GIL_EXTENSION_TOOLBOX_METAFUNCTIONS_ALIGNED_HPP

#include <boost/gil.hpp>

namespace boost { namespace gil {

/// \brief Aligns the image view passed through struct constructor in a specific direction 
/// inside the image view passed through the overloaded '()' operator. The direction of 
/// alignment is specified by constructor parameters.
template <typename View>
struct aligned
{
    // Following enum variables will be used for specifying alignment of view passed through
    // constructor with respect to the view passed through overloaded '()' operator.
    enum
    {
        left = 0x1 << 0,
        center = 0x1 << 1,
        right = 0x1 << 2,
        top = 0x1 << 3,
        middle = 0x1 << 5,
        bottom = 0x1 << 7,
    };

    View original_view;
    int align;
    aligned(View temp_view, int temp_align = center | middle)
        : original_view(temp_view)
        , align(temp_align)
    {
    }

    void operator()(View view)
    {
        long int const original_view_width = original_view.width();
        long int const original_view_height = original_view.height();
        long int const view_width = view.width(), view_height = view.height();

        // For ensuring that view passed through '()' operator has dimensions greater than or
        // equal to the dimensions of view passed through constructor.
        if (original_view_height > view_height || original_view_width > view_width)
        {
            throw std::length_error("Image view passed through overloaded '()' operator must have"
                " dimensions greater than or equal to the dimensions of image view passed through"
                " struct constructor");
        }
 
        std::ptrdiff_t x = 0;
        if (align & center)
            x = (view_width - original_view_width) / 2;
        else if (align & right)
            x = view_width - original_view_width;

        std::ptrdiff_t y = 0;
        if (align & middle)
            y = (view_height - original_view_height) / 2;
        else if (align & bottom)
            y = view_height - original_view_height;

        View temp_subimage_view = subimage_view(view, x, y, original_view_width, original_view_height);
        copy_pixels(original_view, temp_subimage_view);
    }
}; // aligned
}} // namespace boost::gil
#endif
