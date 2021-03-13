//
// Copyright 2005-2007 Adobe Systems Incorporated
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_EXTENSION_NUMERIC_SAMPLER_HPP
#define BOOST_GIL_EXTENSION_NUMERIC_SAMPLER_HPP

#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>

namespace boost { namespace gil {

// Nearest-neighbor and bilinear image samplers.
// NOTE: The code is for example use only. It is not optimized for performance

///////////////////////////////////////////////////////////////////////////
////
////     resample_pixels: set each pixel in the destination view as the result of a sampling function over the transformed coordinates of the source view
////
///////////////////////////////////////////////////////////////////////////
/*
template <typename Sampler>
concept SamplerConcept {
    template <typename DstP,      // Models PixelConcept
              typename SrcView,    // Models RandomAccessNDImageViewConcept
              typename S_COORDS>  // Models PointNDConcept, where S_COORDS::num_dimensions == SrcView::num_dimensions
    bool sample(const Sampler& s, const SrcView& src, const S_COORDS& p, DstP result);
};
*/

/// \brief A sampler that sets the destination pixel to the closest one in the source. If outside the bounds, it doesn't change the destination
/// \ingroup ImageAlgorithms
struct nearest_neighbor_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample(nearest_neighbor_sampler, SrcView const& src, point<F> const& p, DstP& result)
{
    typename SrcView::point_t center(iround(p));
    if (center.x >= 0 && center.y >= 0 && center.x < src.width() && center.y < src.height())
    {
        result=src(center.x,center.y);
        return true;
    }
    return false;
}

struct cast_channel_fn {
    template <typename SrcChannel, typename DstChannel>
    void operator()(const SrcChannel& src, DstChannel& dst) {
        using dst_value_t = typename channel_traits<DstChannel>::value_type;
        dst = dst_value_t(src);
    }
};

template <typename SrcPixel, typename DstPixel>
void cast_pixel(const SrcPixel& src, DstPixel& dst) {
    static_for_each(src,dst,cast_channel_fn());
}

namespace detail {

template <typename Weight>
struct add_dst_mul_src_channel {
    Weight _w;
    add_dst_mul_src_channel(Weight w) : _w(w) {}

    template <typename SrcChannel, typename DstChannel>
    void operator()(const SrcChannel& src, DstChannel& dst) const {
        dst += DstChannel(src*_w);
    }
};

// dst += DST_TYPE(src * w)
template <typename SrcP,typename Weight,typename DstP>
struct add_dst_mul_src {
    void operator()(const SrcP& src, Weight weight, DstP& dst) const {
        static_for_each(src,dst, add_dst_mul_src_channel<Weight>(weight));
//        pixel_assigns_t<DstP,DstP&>()(
//            pixel_plus_t<DstP,DstP,DstP>()(
//                pixel_multiplies_scalar_t<SrcP,Weight,DstP>()(src,weight),
//                dst),
//            dst);
    }
};


template <typename SrcView, typename SrcP, typename Weight, typename DstP>
struct bicubic_interpolate
{
    void operator()(const SrcP& p0, const SrcP& p1, const SrcP& p2, const SrcP& p3, const Weight x, DstP& dst) const 
    {
        using add_dst_mul_src_current = add_dst_mul_src<SrcP,Weight,pixel<Weight,devicen_layout_t<num_channels<SrcView>::value> > >;

        add_dst_mul_src_current()(p0, ((-0.5 * x) + (x*x) - (0.5 * x*x*x)), dst);
        add_dst_mul_src_current()(p1, (1 - (2.5 * x*x) + (1.5 * x*x*x)), dst);
        add_dst_mul_src_current()(p2, ((0.5 * x) + (2 * x*x) - (1.5 * x*x*x)), dst);
        add_dst_mul_src_current()(p3, ((-0.5 * x*x) + (0.5 * x*x*x)), dst);
    }
};



} // namespace detail

/// \brief A sampler that sets the destination pixel as the bilinear interpolation of the four closest pixels from the source.
/// If outside the bounds, it doesn't change the destination
/// \ingroup ImageAlgorithms
struct bilinear_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample(bilinear_sampler, SrcView const& src, point<F> const& p, DstP& result)
{
    using SrcP = typename SrcView::value_type;
    point_t p0(ifloor(p.x), ifloor(p.y)); // the closest integer coordinate top left from p
    point<F> frac(p.x-p0.x, p.y-p0.y);

    if (p0.x < -1 || p0.y < -1 || p0.x>=src.width() || p0.y>=src.height())
    {
        return false;
    }

	pixel<F,devicen_layout_t<num_channels<SrcView>::value> > mp(0); // suboptimal
	typename SrcView::xy_locator loc=src.xy_at(p0.x,p0.y);

	if (p0.x == -1)
    {
		if (p0.y == -1)
        {
		    // the top-left corner pixel
			++loc.y();
			detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(loc.x()[1],  1        ,mp);
		}
        else if (p0.y+1<src.height())
        {
            // on the first column, but not the top-left nor bottom-left corner pixel
			detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(loc.x()[1], (1-frac.y),mp);
			++loc.y();
			detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(loc.x()[1],    frac.y ,mp);
		}
        else
        {
			// the bottom-left corner pixel
			detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(loc.x()[1],  1        ,mp);
		}
	}
    else if (p0.x+1<src.width())
    {
		if (p0.y == -1)
        {
		    // on the first row, but not the top-left nor top-right corner pixel
			++loc.y();
			detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc,      (1-frac.x)           ,mp);
			detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(loc.x()[1],   frac.x            ,mp);
		}
        else if (p0.y+1<src.height())
        {
			// most common case - inside the image, not on the frist nor last row/column
			detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc,      (1-frac.x)*(1-frac.y),mp);
			detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(loc.x()[1],   frac.x *(1-frac.y),mp);
			++loc.y();
			detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc,      (1-frac.x)*   frac.y ,mp);
			detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(loc.x()[1],   frac.x *   frac.y ,mp);
		}
        else
        {
			// on the last row, but not the bottom-left nor bottom-right corner pixel
			detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc,      (1-frac.x)           ,mp);
			detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(loc.x()[1],   frac.x            ,mp);
		}
	}
    else
    {
        if (p0.y == -1)
        {
            // the top-right corner pixel
            ++loc.y();
            detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc,  1        ,mp);
        }
        else if (p0.y+1<src.height())
        {
			// on the last column, but not the top-right nor bottom-right corner pixel
			detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc, (1-frac.y),mp);
			++loc.y();
			detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc,    frac.y ,mp);
		}
        else
        {
			// the bottom-right corner pixel
			detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc,  1        ,mp);
		}
	}

	// Convert from floating point average value to the source type
	SrcP src_result;
	cast_pixel(mp,src_result);

	color_convert(src_result, result);
	return true;
}

/// \brief A sampler that sets the destination pixel as the bicubic interpolation of the four closest pixels from the source.
/// If outside the bounds, it doesn't change the destination
/// \ingroup ImageAlgorithms
struct bicubic_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample(bicubic_sampler, SrcView const& src, point<F> const& p, DstP& result)
{
    using SrcP = typename SrcView::value_type;
    using bicubic_interpolate_current = typename detail::bicubic_interpolate<SrcView, SrcP, F, pixel<F, devicen_layout_t<num_channels<SrcView>::value> > >;

    point_t p0(ifloor(p.x), ifloor(p.y)); // the closest integer coordinate top left from p
    point<F> frac(p.x-p0.x, p.y-p0.y);

    if (p0.x < -1 || p0.y < -1 || p0.x>=src.width() || p0.y>=src.height())
    {
        return false;
    }

	pixel<F,devicen_layout_t<num_channels<SrcView>::value> > mp0(0), mp1(0), mp2(0), mp3(0), mpres(0); // suboptimal
    
	typename SrcView::xy_locator loc=src.xy_at(p0.x,p0.y);

    if(p0.y == -1)
    {
        // Topmost row pixels
        ++loc.y();
        if(p0.x == -1)
        {
            // Top left corner
            detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(loc.x()[1], 1, mpres); 
        }
        else if(p0.x == 0)
        {
            // Top left after corner pixel
            bicubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mpres);
        }
        else if(p0.x+1 == src.width())
        {
            // Top right pixel
            detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc, 1, mpres); 
        }
        else if(p0.x+2 == src.width())
        {
            // Top right pixel before corner pixel
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mpres);
        }
        else
        {
            // All other top row pixels
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mpres);
        }
    }
    else if(p0.y == 0)
    {
        // Row after topmost row, 2nd row
        if(p0.x == -1)
        {
            // Left corner pixel of 2nd row
            ++loc.x();
            bicubic_interpolate_current()(*loc, *loc, *(loc.y()+1), *(loc.y()+2), frac.y, mpres);
        }
        else if(p0.x == 0)
        {
            // Pixel after left corner pixel of 2nd row
            bicubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp1);
            mp0 = mp1;
            ++loc.y();
            bicubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp2);
            ++loc.y();
            bicubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp3);
            
            bicubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);
        }
        else if(p0.x+1 == src.width())
        {
            // Right corner pixel of 2nd row
            bicubic_interpolate_current()(*loc, *loc, *(loc.y()+1), *(loc.y()+2), frac.y, mpres);
        }
        else if(p0.x+2 == src.width())
        {
            // Pixel before right corner pixel of 2nd row
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp1);
            mp0 = mp1;
            ++loc.y();
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp2);
            ++loc.y();
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp3);
            
            bicubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);
        }
        else
        {
            // All other pixels of 2nd row
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp1);
            mp0 = mp1;
            ++loc.y();
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp2);
            ++loc.y();
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp3);
            
            bicubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);
        }
        
    }
    else if(p0.y+1 == src.height())
    {
        // Bottom most row
        if(p0.x == -1)
        {
            // Left corner pixel of last row
            detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(loc.x()[1], 1, mpres); 
        }
        else if(p0.x == 0)
        {
            // Pixel after left corner of last row
            bicubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mpres);
        }
        else if(p0.x+1 == src.width())
        {
            // Right corner of last row
            detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc, 1, mpres); 
        }
        else if(p0.x+2 == src.width())
        {
            // Pixel before right corner of last row
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mpres);
        }
        else
        {
            // All other pixels of last row
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mpres);
        }
    }
    else if(p0.y+2 == src.height())
    {
        // Row before bottom most row
        if(p0.x == -1)
        {
            // Leftmost corner of 2nd last row
            ++loc.x();
            bicubic_interpolate_current()(*(loc.y()-1), *loc, *(loc.y()+1), *(loc.y()+1), frac.y, mpres);
        }
        else if(p0.x == 0)
        {
            // Pixel after left corner of second last row
            --loc.y();
            bicubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp0);
            ++loc.y();
            bicubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp1);
            ++loc.y();
            bicubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp2);
            mp3 = mp2;
            
            bicubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);
        }
        else if(p0.x+1 == src.width())
        {
            // Right corner of second last row
            bicubic_interpolate_current()(*(loc.y()-1), *loc, *(loc.y()+1), *(loc.y()+1), frac.y, mpres);
        }
        else if(p0.x+2 == src.width())
        {
            // Pixel before right corner of second last row
            --loc.y();
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp0);
            ++loc.y();
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp1);
            ++loc.y();
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp2);
            mp3 = mp2;
            
            bicubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);
        }
        else
        {
            // All other pixels in second last row
            --loc.y();
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp0);
            ++loc.y();
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp1);
            ++loc.y();
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp2);
            mp3 = mp2;

            bicubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);
        }
    }
    else
    {
        // All other rows
        if(p0.x == -1)
        {
            // First column
            ++loc.x();
            bicubic_interpolate_current()(*(loc.y()-1), *loc, *(loc.y()+1), *(loc.y()+2), frac.y, mpres);
        }
        else if(p0.x == 0)
        {
            // 2nd column
            --loc.y();
            bicubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp0);
            ++loc.y();
            bicubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp1);
            ++loc.y();
            bicubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp2);
            ++loc.y();
            bicubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp3);
            
            bicubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);
        }
        else if(p0.x+1 == src.width())
        {
            // Last column
            bicubic_interpolate_current()(*(loc.y()-1), *loc, *(loc.y()+1), *(loc.y()+2), frac.y, mpres);
        }
        else if(p0.x+2 == src.width())
        {
            // 2nd last column
            --loc.y();
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp0);
            ++loc.y();
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp1);
            ++loc.y();
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp2);
            ++loc.y();
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp3);
            
            bicubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);
        }
        else
        {
            // All general cases, center pixels
            --loc.y();
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp0);
            ++loc.y();
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp1);
            ++loc.y();
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp2);
            ++loc.y();
            bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp3);
            
            bicubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);
        }
    }
    
    // if(p0.y == -1)
    // {
    //     ++loc.y();
    //     detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc, 1, mpres);
    // }
    // else if(p0.y == 0 || p0.y+1 == src.height() || p0.y+2 == src.height())
    // {
    //     detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc, 1, mpres);
    // }
    // else if (p0.y+2 < src.height())
    // {
    //     if(p0.x == -1)
    //     {
    //         ++loc.x();
    //         detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc, 1, mpres);
    //     }
    //     if(p0.x+1 == src.width())
    //     {
    //         detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc, 1, mpres);
    //     }
    //     // if(p0.x == -1 || p0.x+1 == src.width())
    //     // {
    //     //     // First or last column pixels
    //     //     bicubic_interpolate_current()(*(loc.y()-1), *loc, *(loc.y()+1), *(loc.y()+2), frac.y, mpres);
    //     // }
    //     else if(p0.x == 0)
    //     {
    //         // Most common case, inside image not any corner or last rows or columns
    //         --loc.y();
    //         bicubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp0);
    //         ++loc.y();
    //         bicubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp1);
    //         ++loc.y();
    //         bicubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp2);
    //         ++loc.y();
    //         bicubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp3);
            
    //         bicubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);
    //     }
    //     else if(p0.x+2 == src.width())
    //     {
    //         // 
    //         --loc.y();
    //         bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp0);
    //         ++loc.y();
    //         bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp1);
    //         ++loc.y();
    //         bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp2);
    //         ++loc.y();
    //         bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp3);

    //         bicubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);
    //     }
    //     else if(p0.x+2 < src.width())
    //     {
    //         // Most common case, inside image not any corner or last rows or columns
    //         --loc.y();
    //         bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp0);
    //         ++loc.y();
    //         bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp1);
    //         ++loc.y();
    //         bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp2);
    //         ++loc.y();
    //         bicubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp3);
            
    //         bicubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);
    //     }
    // }

    SrcP src_result;
	cast_pixel(mpres, src_result);

	color_convert(src_result, result);
	return true;
}

}}  // namespace boost::gil

#endif
