#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/image_processing/morphology.hpp>
#include <vector>

//current structuring element is SE = [0.1,0.1,0.1]
//                                    |0.1,0.1,0.1|
//                                    [0.1,0.1,0.1]
//SE(1,1)(center pixel) is the one which coincides with the currently considered pixel of the 
//image to be convolved. The structuring element can be easily changed by the user.

int main()
{
    using namespace boost::gil;
    boost::gil::gray8_image_t img;
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
