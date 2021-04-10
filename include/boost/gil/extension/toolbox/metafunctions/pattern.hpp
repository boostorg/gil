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
    View v2;
    pattern(View view)
        : v2(view)
    {
    }

    void operator()(View& view)
    {
        long int const h = v2.height(), w = v2.width();
        
        // For ensuring that view passed through '()' operator has dimensions greater than or
        // equal to the dimensions of view passed through constructor.
        if (h > view.height() || w > view.width())
        {
            throw std::length_error("Image view passed through overloaded '()' operator must have"
            " dimensions greater than or equal to the dimensions of image view passed through"
            " struct constructor");
        }

        for (std::ptrdiff_t x = 0; x < view.width(); x += w)
        {
            for (std::ptrdiff_t y = 0; y < view.height(); y += h)
            {
                std::ptrdiff_t aw = w;
                if (x + w > view.width())
                {
                    std::ptrdiff_t t = x + w - view.width();
                    aw = w - t;
                }

                std::ptrdiff_t ah = h;
                if (y + h > view.height())
                {
                    std::ptrdiff_t t = y + h - view.height();
                    ah = h - t;
                }

                View v3 = subimage_view(view, x, y, aw, ah);
                View v4 = subimage_view(v2, 0, 0, aw, ah);
                copy_pixels(v4, v3);
            }
        }
    }
}; // pattern
}} // namespace boost::gil
#endif
