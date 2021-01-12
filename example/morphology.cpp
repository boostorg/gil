#include <iostream>
#include <boost/gil/image_processing/threshold.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <vector>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

boost::gil::gray8_image_t img;

//This function is used for applying basic threshold and convolution operations which are applied 
//under the hood for all morphological transformations.
template <typename SrcView, typename Kernel>
boost::gil::gray8_image_t thresh_con_thresh(SrcView const& src_view, Kernel const& ker_mat,const int threshold1,const int threshold2)
{
    using namespace boost::gil;
    boost::gil::gray8_image_t intermediate_img(src_view.dimensions());
    threshold_binary(src_view, view(intermediate_img),threshold1, 255);
    detail::convolve_2d(view(intermediate_img), ker_mat, view(intermediate_img));
    threshold_binary(const_view(intermediate_img),view(intermediate_img),threshold2,255);
    return intermediate_img;
}

template <typename SrcView, typename Kernel>
boost::gil::gray8_image_t dilate(SrcView const& src_view, Kernel const& ker_mat,const int iterations)
{
    boost::gil::gray8_image_t img_out_dilation(src_view.dimensions());
    img_out_dilation = img;
    for(int i=0;i<iterations;++i)
        img_out_dilation = thresh_con_thresh(view(img_out_dilation),ker_mat,170,26);
    return img_out_dilation;
}

template <typename SrcView, typename Kernel>
boost::gil::gray8_image_t erode(SrcView const& src_view, Kernel const& ker_mat,const int iterations)
{
    boost::gil::gray8_image_t img_out_erosion(src_view.dimensions());
    img_out_erosion = img;
    for(int i=0;i<iterations;++i)
        img_out_erosion = thresh_con_thresh(view(img_out_erosion),ker_mat,170,204);
    return img_out_erosion;
}

template <typename SrcView, typename Kernel>
boost::gil::gray8_image_t opening(SrcView const& src_view, Kernel const& ker_mat)
{
    boost::gil::gray8_image_t img_out_opening(src_view.dimensions());
    img_out_opening = erode(src_view,ker_mat,1);
    return dilate(view(img_out_opening),ker_mat,1);
}

template <typename SrcView, typename Kernel>
boost::gil::gray8_image_t closing(SrcView const& src_view, Kernel const& ker_mat)
{
    boost::gil::gray8_image_t img_out_closing(src_view.dimensions());
    img_out_closing = dilate(src_view,ker_mat,1);
    return erode(view(img_out_closing),ker_mat,1);
}


//This function calculates the difference between pixel values of first image_view and second
//image_view.This function can be used for rgb as well as grayscale images.
template <typename SrcView, typename DstView ,typename DiffView>
void difference_impl(SrcView const& src_view, DstView const& dst_view, DiffView const& diff_view)
{
    for (std::ptrdiff_t view_row = 0; view_row < src_view.height(); ++view_row)
        for (std::ptrdiff_t view_col = 0; view_col < src_view.width(); ++view_col)
            diff_view(view_col, view_row) = src_view(view_col,view_row) - dst_view(view_col,view_row);
}


template <typename SrcView, typename DstView, typename DiffView>
void difference(SrcView const& src_view, DstView const& dst_view , DiffView const& diff_view)
{

    BOOST_ASSERT(src_view.dimensions() == dst_view.dimensions());

    for (std::size_t i = 0; i < src_view.num_channels(); i++)
    {
        difference_impl(
            nth_channel_view(src_view, i),
            nth_channel_view(dst_view, i),
            nth_channel_view(diff_view, i)
        );
    }
}

template <typename SrcView, typename Kernel>
boost::gil::gray8_image_t morphological_gradient(SrcView const& src_view, Kernel const& ker_mat)
{
    using namespace boost::gil;
    gray8_image_t int_dilate(src_view.dimensions()),int_erode(src_view.dimensions()),res_image(src_view.dimensions());
    int_dilate = dilate(src_view,ker_mat,1);
    int_erode = erode(src_view,ker_mat,1);
    difference(view(int_dilate),view(int_erode),view(res_image));
    return res_image;
}

template <typename SrcView, typename Kernel>
boost::gil::gray8_image_t top_hat(SrcView const& src_view, Kernel const& ker_mat)
{
    using namespace boost::gil;
    gray8_image_t int_opening(src_view.dimensions()),res_image(src_view.dimensions());
    int_opening = opening(src_view,ker_mat);
    difference(src_view,view(int_opening),view(res_image));
    return res_image;
}

template <typename SrcView, typename Kernel>
boost::gil::gray8_image_t black_hat(SrcView const& src_view, Kernel const& ker_mat)
{
    using namespace boost::gil;
    gray8_image_t int_closing(src_view.dimensions()),res_image(src_view.dimensions());
    int_closing = closing(src_view,ker_mat);
    difference(view(int_closing), src_view,view(res_image));
    return res_image;
}

//Summary of applied morphological concepts
//Structuring element is SE = [0.1,0.1,0.1]
//                            |0.1,0.1,0.1|
//                            [0.1,0.1,0.1]
//SE(1,1)(center pixel) is the one which coincides with the currently considered pixel of the 
//image to be convolved. 
//1.Dilation:If 1 or more bright pixels coincide with the structuring element during convolution,
//center pixel is made bright.We can vary the number of times dilation happens by varying the 
//argument 'iterations' in the dilate function.
//2.Erosion:If 8 or more bright pixels coincide with the structuring element during convolution,
//center pixel is made bright.We can vary the number of times erosion happens by varying the 
//argument 'iterations' in the erode function.
//3.Opening:Opening is just another name of erosion followed by dilation.
//It is useful in removing noise.
//4.Closing:Closing is reverse of Opening, Dilation followed by Erosion.
//It is useful in closing small holes inside the foreground objects, or small black points 
//on the object.
//5.Morphological Gradient:It is the difference between dilation and erosion of an image.
//The result will look like the outline of the object.
//6.Top Hat:It is the difference between input image and Opening of the image.
//7.Black Hat:It is the difference between the closing of the input image and input image.

//Functions have been made for applying above morphological transformations which
//return the transformed image.

int main()
{
    using namespace boost::gil;
    read_image("original.png", img, png_tag{});
    std::vector<float>ker_vec(9,0.1f);//Structuring element
    detail::kernel_2d<float> ker_mat(ker_vec.begin(), ker_vec.size(), 1, 1);
    gray8_image_t img_out_dilation(img.dimensions()),img_out_erosion(img.dimensions()),img_out_opening(img.dimensions());
    gray8_image_t img_out_closing(img.dimensions()),img_out_mg(img.dimensions()),img_out_top_hat(img.dimensions());
    gray8_image_t img_out_black_hat(img.dimensions());
    //dilate(input_image_view,structuring_element,iterations)
    img_out_dilation = dilate(view(img),ker_mat,1);
    //erode(input_image_view,structuring_element,iterations)
    img_out_erosion = erode(view(img),ker_mat,1);
    //opening(input_image_view,structuring_element)
    img_out_opening = opening(view(img),ker_mat);
    //closing(input_image_view,structuring_element)
    img_out_closing = closing(view(img),ker_mat);
    //morphological_gradient(input_image_view,structuring_element)
    img_out_mg = morphological_gradient(view(img),ker_mat);
    //top_hat(input_image_view,structuring_element)
    img_out_top_hat = top_hat(view(img),ker_mat);
    //black_hat(input_image_view,structuring_element)
    img_out_black_hat = black_hat(view(img),ker_mat);

    //Saving results of above morphological transformations.
    write_view("out-morphological-example-dilation.png", view(img_out_dilation), png_tag{});
    write_view("out-morphological-example-erosion.png", view(img_out_erosion), png_tag{});
    write_view("out-morphological-example-opening.png", view(img_out_opening), png_tag{});
    write_view("out-morphological-example-closing.png", view(img_out_closing), png_tag{});
    write_view("out-morphological-example-morphological_gradient.png", view(img_out_mg), png_tag{});
    write_view("out-morphological-example-top_hat.png", view(img_out_top_hat), png_tag{});
    write_view("out-morphological-example-black_hat.png", view(img_out_black_hat), png_tag{});
}
