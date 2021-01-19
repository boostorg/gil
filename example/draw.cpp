#include <boost/gil/extension/io/png.hpp>
#include<boost/gil/extension/draw/circle_and_rect.hpp>

int main()
{
    using namespace boost::gil;
    rgb8_image_t img;//Can be used for rgb as well as grayscale images.
    read_image("draw.png", img, png_tag{});
    rgb8_image_t img_rect = img,img_circle = img;
    //draw_rect(Input_image_view,x_co-ordinate_of_top_left_corner,y_co_ordinate_of_top_left_corner,
    //x_co-odinate_of_bottom_right_corner,y_co-ordinate_of_bottom_right_corner,
    //desired_colour_of_border_in_r,g,b_format,thickness of border)
    draw_rect(view(img_rect),275,300,600,600,0,0,105,10);
    write_view("out-draw_add_rectangle.png", view(img_rect), png_tag{});
    //draw_circle(Input_image_view,x_co-ordinate_of_center,y_co-ordinate_of_center,radius
    //desired_colour_of_border_in_r,g,b_format,thickness_of_border)
    draw_circle(view(img_circle),600,275,40,0,105,0,10);
    write_view("out-draw_add_circle.png", view(img_circle), png_tag{});
}