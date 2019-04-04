#include <boost/gil.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
// #include<boost/gil/threshold.hpp>
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
int main(){
	using namespace boost::gil;
	rgb8_image_t img;
    read_image("test.jpg", img, jpeg_tag());
    gray8_image_t output(img.dimensions());
    bin_thresh(color_converted_view<gray8_pixel_t>(const_view(img)),view(output),255,0,100);
    write_view("out-bin_thresh.jpeg", view(output), jpeg_tag{});
}