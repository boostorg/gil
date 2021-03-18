// Copyright Tom Brinkman 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef _pattern_hpp_
#define _pattern_hpp_

#include <boost/gil.hpp>

namespace boost  {  namespace gil  {

/// \brief Repeatedly copies smaller image view passed through the struct constructor in subimage
/// views of the larger image view passed through overloaded '()' operator.
template <typename view_t>
struct pattern
{
    view_t v2;
    pattern(view_t v2)
        : v2(v2)
    {
    }

    void operator()(view_t& view)
    {
        using namespace boost::gil;

        std::size_t h = v2.height(), w = v2.width();
        
        // For ensuring that view passed through '()' operator has larger dimensions than view
        // passed through the constructor.
        if(h > view.height() || w > view.width())
            return;

        int n = 0;
        for(std::ptrdiff_t x = 0; x < view.width(); x += w)
        {
            for(std::ptrdiff_t y = 0; y < view.height(); y += h)
            {
                std::ptrdiff_t aw = w;
                if(x + w > view.width())
                {
                    std::ptrdiff_t t = x + w - view.width();
                    aw = w - t;
                }

                std::ptrdiff_t ah = h;
                if(y + h > view.height())
                {
                    std::ptrdiff_t t = y + h - view.height();
                    ah = h - t;
                }

                view_t v3 = subimage_view(view, x, y, aw, ah);
                view_t v4 = subimage_view(v2, 0, 0, aw, ah);
                copy_pixels(v4, v3);
            }
        }
    }
};
}  }

#endif
