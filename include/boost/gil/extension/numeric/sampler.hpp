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

// Nearest-neighbor, bilinear and bicubic image samplers.
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

// template <typename SrcPixel, typename DstPixel>
// void cast_pixel_bicubic(const SrcPixel& src, DstPixel& dst) {
//     static_for_each(src, dst, cubic_interpolate_check());
// }

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

struct cubic_interpolate_check {
    template <typename SrcChannel, typename DstChannel>
    void operator()(const SrcChannel& , DstChannel& dst) {
        // using dst_value_t = typename channel_traits<DstChannel>::value_type;
        using src_value_t = typename channel_traits<SrcChannel>::value_type;

        if(dst > channel_traits<SrcChannel>::max_value()){
            dst = src_value_t(channel_traits<SrcChannel>::max_value());
        }
        else if(dst < channel_traits<SrcChannel>::min_value()){
            dst = src_value_t(channel_traits<SrcChannel>::min_value());
        }
        else{
            dst = src_value_t(dst);
        }
    }
};

template <typename Weight>
struct cubic_interpolate_channel {
    Weight _w1;
    Weight _w2;
    Weight _w3;
    Weight _w4;
    cubic_interpolate_channel(Weight w1, Weight w2, Weight w3, Weight w4) : _w1(w1), _w2(w2), _w3(w3), _w4(w4) {}

    template <typename SrcChannel, typename DstChannel>
    void operator()(const SrcChannel& src1, const SrcChannel& src2, const SrcChannel& src3, const SrcChannel& src4, DstChannel& dst) const {
        dst = DstChannel(src1*_w1 + src2*_w2 + src3*_w3 + src4*_w4);
    }
};

template <typename SrcView, typename SrcP, typename Weight, typename DstP>
struct cubic_interpolate
{
    void operator()(const SrcP& p0, const SrcP& p1, const SrcP& p2, const SrcP& p3, const Weight x, DstP& dst) const 
    {
        Weight w0 = ((-0.5 * x) + (x*x) - (0.5 * x*x*x));
        Weight w1 = (1 - (2.5 * x*x) + (1.5 * x*x*x));
        Weight w2 = ((0.5 * x) + (2 * x*x) - (1.5 * x*x*x));
        Weight w3 = ((-0.5 * x*x) + (0.5 * x*x*x));

        static_for_each(p0, p1, p2, p3, dst, cubic_interpolate_channel<Weight>(w0, w1, w2, w3));
        
        // Overflow check. Should optimally be after all cubic calculations
        static_for_each(p0, dst, cubic_interpolate_check());
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

/// \brief A sampler that sets the destination pixel as the bicubic interpolation of the 16 closest pixels from the source.
/// If outside the bounds, it doesn't change the destination. Boundary pixels are the cubic interpolation of 4 adjacent
/// pixels.
/// \ingroup ImageAlgorithms
struct bicubic_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample(bicubic_sampler, SrcView const& src, point<F> const& p, DstP& result)
{
    using SrcP = typename SrcView::value_type;
    using cubic_interpolate_current = typename detail::cubic_interpolate<SrcView, SrcP, F, pixel<F, devicen_layout_t<num_channels<SrcView>::value> > >;

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
            cubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mpres);
        }
        else if(p0.x+1 == src.width())
        {
            // Top right pixel
            detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc, 1, mpres); 
        }
        else if(p0.x+2 == src.width())
        {
            // Top right pixel before corner pixel
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mpres);
        }
        else
        {
            // All other top row pixels
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mpres);
        }
    }
    else if(p0.y == 0)
    {
        // Row after topmost row, 2nd row
        if(p0.x == -1)
        {
            // Left corner pixel of 2nd row
            ++loc.x();
            cubic_interpolate_current()(*loc, *loc, *(loc.y()+1), *(loc.y()+2), frac.y, mpres);
        }
        else if(p0.x == 0)
        {
            // Pixel after left corner pixel of 2nd row
            cubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp1);
            mp0 = mp1;
            ++loc.y();
            cubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp2);
            ++loc.y();
            cubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp3);
            
            cubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);
        }
        else if(p0.x+1 == src.width())
        {
            // Right corner pixel of 2nd row
            cubic_interpolate_current()(*loc, *loc, *(loc.y()+1), *(loc.y()+2), frac.y, mpres);
        }
        else if(p0.x+2 == src.width())
        {
            // Pixel before right corner pixel of 2nd row
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp1);
            mp0 = mp1;
            ++loc.y();
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp2);
            ++loc.y();
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp3);
            
            cubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);
        }
        else
        {
            // All other pixels of 2nd row
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp1);
            mp0 = mp1;
            ++loc.y();
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp2);
            ++loc.y();
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp3);
            
            cubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);
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
            cubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mpres);
        }
        else if(p0.x+1 == src.width())
        {
            // Right corner of last row
            detail::add_dst_mul_src<SrcP,F,pixel<F,devicen_layout_t<num_channels<SrcView>::value> > >()(*loc, 1, mpres); 
        }
        else if(p0.x+2 == src.width())
        {
            // Pixel before right corner of last row
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mpres);
        }
        else
        {
            // All other pixels of last row
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mpres);
        }
    }
    else if(p0.y+2 == src.height())
    {
        // Row before bottom most row
        if(p0.x == -1)
        {
            // Leftmost corner of 2nd last row
            ++loc.x();
            cubic_interpolate_current()(*(loc.y()-1), *loc, *(loc.y()+1), *(loc.y()+1), frac.y, mpres);
        }
        else if(p0.x == 0)
        {
            // Pixel after left corner of second last row
            --loc.y();
            cubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp0);
            ++loc.y();
            cubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp1);
            ++loc.y();
            cubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp2);
            mp3 = mp2;
            
            cubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);
        }
        else if(p0.x+1 == src.width())
        {
            // Right corner of second last row
            cubic_interpolate_current()(*(loc.y()-1), *loc, *(loc.y()+1), *(loc.y()+1), frac.y, mpres);
        }
        else if(p0.x+2 == src.width())
        {
            // Pixel before right corner of second last row
            --loc.y();
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp0);
            ++loc.y();
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp1);
            ++loc.y();
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp2);
            mp3 = mp2;
            
            cubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);
        }
        else
        {
            // All other pixels in second last row
            --loc.y();
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp0);
            ++loc.y();
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp1);
            ++loc.y();
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp2);
            mp3 = mp2;

            cubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);
        }
    }
    else
    {
        // All other rows
        if(p0.x == -1)
        {
            // First column
            ++loc.x();
            cubic_interpolate_current()(*(loc.y()-1), *loc, *(loc.y()+1), *(loc.y()+2), frac.y, mpres);
        }
        else if(p0.x == 0)
        {
            // 2nd column
            --loc.y();
            cubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp0);
            ++loc.y();
            cubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp1);
            ++loc.y();
            cubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp2);
            ++loc.y();
            cubic_interpolate_current()(*loc, *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp3);
            
            cubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);
        }
        else if(p0.x+1 == src.width())
        {
            // Last column
            cubic_interpolate_current()(*(loc.y()-1), *loc, *(loc.y()+1), *(loc.y()+2), frac.y, mpres);
        }
        else if(p0.x+2 == src.width())
        {
            // 2nd last column
            --loc.y();
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp0);
            ++loc.y();
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp1);
            ++loc.y();
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp2);
            ++loc.y();
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+1), frac.x, mp3);
            
            cubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);
        }
        else
        {
            // All general cases, center pixels
            --loc.y();
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp0);
            ++loc.y();
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp1);
            ++loc.y();
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp2);
            ++loc.y();
            cubic_interpolate_current()(*(loc.x()-1), *loc, *(loc.x()+1), *(loc.x()+2), frac.x, mp3);
            
            cubic_interpolate_current()(mp0, mp1, mp2, mp3, frac.y, mpres);

        }
    }

    SrcP src_result;
    
	cast_pixel(mpres, src_result);

	color_convert(src_result, result);
	return true;
}

}}  // namespace boost::gil

#endif
