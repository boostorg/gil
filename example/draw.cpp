#include <iostream>
#include <boost/gil/image_processing/threshold.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <vector>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>
#include <math.h>

template <typename SrcView>
void draw_rect_impl(SrcView const& src_view, const int tlc0, const int tlc1, const int brc0, const int brc1,const int colour, const int thickness)
{
    //Following four loops draw edges of the rectangle.
    for(std::ptrdiff_t view_col = tlc0,view_row = tlc1;view_col <= brc0;++view_col)
        for(int i = 0;i < thickness;++i)
            src_view(view_col,view_row + i) = colour;
    for(std::ptrdiff_t view_row = tlc1,view_col = brc0;view_row <= brc1;++view_row)
        for(int i = 0;i < thickness;++i)
            src_view(view_col + i,view_row) = colour;
    for(std::ptrdiff_t view_row = tlc1,view_col = tlc0;view_row <= brc1;++view_row)
        for(int i = 0;i < thickness;++i)
            src_view(view_col + i,view_row) = colour;
    for(std::ptrdiff_t view_col = tlc0,view_row = brc1;view_col <= brc0;++view_col)
        for(int i = 0;i < thickness;++i)
            src_view(view_col,view_row + i) = colour;
    std::ptrdiff_t view_col = brc0,view_row = brc1;
    //Following loop completes the bottom right part of the rectangle which otherwise would have 
    //remained as it is.
    for(int j = 0;j<thickness;++j)
    {
        for(int i = 0;i < thickness;++i)
            src_view(view_col,view_row + i) = colour;
        ++view_col;
    }
}

template <typename SrcView>
void draw_rect(SrcView const& src_view, const int tlc0, const int tlc1, const int brc0, const int brc1, const int r,const int g,const int b,const int thickness)
{
    int colours[] = {r,g,b};
    for (std::size_t i = 0; i < src_view.num_channels(); i++)
    {
            draw_rect_impl(
            nth_channel_view(src_view, i),
            tlc0,tlc1,brc0,brc1,colours[i],thickness
        );
    }
}

//Circle is made by rotating the radius vector around the center and changing the 
//colour of pixels at its tip. 
template <typename SrcView>
void draw_circle_impl(SrcView const& src_view, const int center_x, const int center_y, const int input_radius, const int colour,const int thickness)
{
    double angle = 0;
    int radius = input_radius;
    for(int i=0;i<thickness;++i)
    {
        for(double i=0.0;i <= 1.570796;i += 0.00001)
        {
            src_view(center_x + (int)(radius*cos(i)),center_y + (int)(radius*sin(i))) = colour;
            src_view(center_x + (int)(radius*cos(i)),center_y - (int)(radius*sin(i))) = colour;
            src_view(center_x - (int)(radius*cos(i)),center_y + (int)(radius*sin(i))) = colour;
            src_view(center_x - (int)(radius*cos(i)),center_y - (int)(radius*sin(i))) = colour; 
        }
        ++radius;
    }
}

template <typename SrcView>
void draw_circle(SrcView const& src_view, const int center_x, const int center_y, const int radius, const int r,const int g,const int b,const int thickness)
{
    int colours[] = {r,g,b};
    for (std::size_t i = 0; i < src_view.num_channels(); i++)
    {
            draw_circle_impl(
            nth_channel_view(src_view, i),
            center_x,center_y,radius,colours[i],thickness
        );
    }
}

int main()
{
    using namespace boost::gil;
    rgb8_image_t img;//Can be used for rgb as well as grayscale images.
    read_image("draw.png", img, png_tag{});
    rgb8_image_t img_rect = img,img_circle = img;
    //draw_rect(Input_image_view,x_co-ordinate_of_top_left_corner,y_co_ordinate_of_top_left_corner,
    //x_co-odinate_of_top_right_corner,y_co-ordinate_of_top_right_corner,
    //desired_colour_of_border_in_r,g,b_format,thickness of border)
    draw_rect(view(img_rect),2,2,70,70,0,0,105,10);
    write_view("out-draw_add_rectangle.png", view(img_rect), png_tag{});
    //draw_circle(Input_image_view,x_co-ordinate_of_center,y_co-ordinate_of_center,
    //desired_colour_of_border_in_r,g,b_format,thickness_of_border)
    draw_circle(view(img_circle),250,250,40,105,0,0,10);
    write_view("out-draw_add_circle.png", view(img_circle), png_tag{});
}