#ifndef BOOST_GIL_THRESHOLD_HPP
#define BOOST_GIL_THRESHOLD_HPP

template <typename SrcView, typename DstView,typename typ>
void bin_thresh(SrcView const& src, DstView const& dst,typ max,typ min,typ thresh)
{
    
    for (int y = 0; y < src.height(); ++y)
    {
        typename SrcView::x_iterator src_it = src.row_begin(y);
        typename DstView::x_iterator dst_it = dst.row_begin(y);

        for (int x = 1; x < src.width() - 1; ++x)
        {
        	if(src_it[x]<thresh){
        		dst_it[x]=min;
        	}else{
        		dst_it[x]=max;
        	}
        }
    }
}
#endif