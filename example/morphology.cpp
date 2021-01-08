#include <boost/gil/image_processing/threshold.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/image_processing/numeric.hpp>
#include <vector>
#include <functional>
#include <set>
#include <iostream>
#include <fstream>
#include <boost/gil/extension/io/jpeg.hpp>


#include<bits/stdc++.h>

namespace gil = boost::gil;
int operation = 0;//0:dilation,1:erosion

template <typename SrcView, typename DstView, typename Kernel>
void convolve_2d_impl_custom(SrcView const& src_view, DstView const& dst_view, Kernel const& kernel)
{
    int flip_ker_row, flip_ker_col, row_boundary, col_boundary;
    float aux_total;
    for (std::ptrdiff_t view_row = 0; view_row < src_view.height(); ++view_row)
    {
        for (std::ptrdiff_t view_col = 0; view_col < src_view.width(); ++view_col)
        {
            aux_total = 0.0f;
            for (std::size_t kernel_row = 0; kernel_row < kernel.size(); ++kernel_row)
            {
                flip_ker_row = kernel.size() - 1 - kernel_row;      // row index of flipped kernel

                for (std::size_t kernel_col = 0; kernel_col < kernel.size(); ++kernel_col)
                {
                    flip_ker_col = kernel.size() - 1 - kernel_col; // column index of flipped kernel

                    // index of input signal, used for checking boundary
                    row_boundary = view_row + (kernel.center_y() - flip_ker_row);
                    col_boundary = view_col + (kernel.center_x() - flip_ker_col);

                    // ignore input samples which are out of bound
                    if (row_boundary >= 0 && row_boundary < src_view.height() &&
                        col_boundary >= 0 && col_boundary < src_view.width())
                    {
                        aux_total +=
                            src_view(col_boundary, row_boundary) *
                            kernel.at(flip_ker_row, flip_ker_col);
                    }
                }
            }
            if(!operation)
            {
                if(aux_total >= 510)
                    dst_view(view_col,view_row) = 255;
                else
                    dst_view(view_col, view_row) = 0;
            }
            else
            {
                if(aux_total > 1275)
                    dst_view(view_col,view_row) = 255;
                else 
                    dst_view(view_col,view_row) = 0;
            }
        }
    }
}

template <typename SrcView, typename DstView, typename Kernel>
void convolve_2d_custom(SrcView const& src_view, Kernel const& kernel, DstView const& dst_view)
{
    BOOST_ASSERT(src_view.dimensions() == dst_view.dimensions());
    BOOST_ASSERT(kernel.size() != 0);

    boost::gil::gil_function_requires<boost::gil::ImageViewConcept<SrcView>>();
    boost::gil::gil_function_requires<boost::gil::MutableImageViewConcept<DstView>>();

    for (std::size_t i = 0; i < src_view.num_channels(); i++)
    {
        convolve_2d_impl_custom(
            nth_channel_view(src_view, i),
            nth_channel_view(dst_view, i),
            kernel
        );
    }
}

int main()
{
    using namespace boost::gil;
    gray8_image_t img;
    read_image("opening.png", img, png_tag{});
    std::vector<float>ker_vec = {1,1,1,1,1,1,1,1,1};
    detail::kernel_2d<float> ker_mat(ker_vec.begin(), ker_vec.size(), 1, 1);
    gray8_image_t img_out_dilation(img.dimensions()),img_out_erosion(img.dimensions());
    threshold_binary(const_view(img), view(img),170, 255);
    convolve_2d_custom(view(img), ker_mat, view(img_out_dilation));
    write_view("out-morphological-example-dilation.png", view(img_out_dilation), png_tag{});
    operation = 1;
    convolve_2d_custom(view(img), ker_mat, view(img_out_erosion));
    write_view("out-morphological-example-erosion.png", view(img_out_erosion), png_tag{});
}
