


#include <boost/core/lightweight_test.hpp>
#include <boost/gil.hpp>
#include<bits/stdc++.h>
#include<boost/gil/extension/io/png.hpp>
#include<boost/gil/extension/io/tiff.hpp>
#include<boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/gil/concepts/pixel.hpp>
#include <boost/gil/typedefs.hpp>
#include<boost/multi_array.hpp>
#include<boost/assert.hpp>
#include <boost/gil/color_convert.hpp>
#include <boost/gil/image_processing/numeric.hpp>
//#define PI 3.14159265
namespace boost
{
   namespace gil
  {


        namespace detail
        {
            template<typename T,typename U>
              void carttopolar(const T &dx,const T &dy,U &magnitude,U &angle)
              {
                  //for calculating gradient magnitude and direction, to be used for constructing orientation histogram
                  for(std::ptrdiff_t y=0;y<dx.shape()[0];y++)
                  {
                    for(std::ptrdiff_t x=0;x<dx.shape()[1];x++)
                    {
                        for(std::ptrdiff_t z=0;z<dx.shape()[2];z++)
                        {
                            double px=(double)dx[y][x][z];
                            double py=(double)dy[y][x][z];
                            double ang=atan2(py,px)*(float)180/M_PI;
                            magnitude[y][x][z]=(std::sqrt(px*px+py*py));
                            if(ang==180)
                            angle[y][x][z]=0;
                            else
                            angle[y][x][z]=ang<0?180.000+ang:ang;
                        }

                    }
                  }
              }
              template<typename T,typename U>
              void calculate_histogram(T &histogram,U&magnitude_view,U&angle_view,int number_of_cells_vertical,int number_of_cells_horizontal,int pixels_per_cell,double number_of_orientations_per_180)
              {
                typedef boost::multi_array<double,2> array_type;
                for(int i=0;i<number_of_cells_vertical;i++)
                 {
                    for(int j=0;j<number_of_cells_horizontal;j++)
                    {
                        int cell_row_index=i*pixels_per_cell;
                        int cell_column_index=j*pixels_per_cell;
                        typedef boost::multi_array_types::index_range range;
                        array_type::array_view<2>::type magnitude_sub_view =magnitude_view[ boost::indices[range(cell_row_index,cell_row_index+pixels_per_cell)][range(cell_column_index,cell_column_index+pixels_per_cell)]];
                        array_type::array_view<2>::type angle_sub_view =angle_view[ boost::indices[range(cell_row_index,cell_row_index+pixels_per_cell)][range(cell_column_index,cell_column_index+pixels_per_cell)]];

                        for(array_type::index k=0;k<pixels_per_cell;k++)
                        {
                           for(array_type::index m=0;m<pixels_per_cell;m++)
                           {
                                 double gradient_magnitude_pixel=magnitude_sub_view[k][m];
                                 double gradient_direction_pixel=angle_sub_view[k][m];
                                 int orientation_bin_index=std::trunc(gradient_direction_pixel)/number_of_orientations_per_180;
                                 histogram[i][j][orientation_bin_index]+=gradient_magnitude_pixel;
                           }
                        }
                        for(auto &u:histogram[i][j])
                        {
                          u/=(pixels_per_cell*pixels_per_cell);
                        }
                    }
                 }
               }
               template<typename SrcView,typename Grad_X_Matrix>
               void calculate_horizontal_gradient(SrcView &img_in,Grad_X_Matrix &img_out_view_x,int number_of_channels)
               {
                   for(std::ptrdiff_t y=0;y<img_in.height();y++)
                   {
                       typename SrcView::x_iterator src_it = img_in.row_begin(y);

                       for(std::ptrdiff_t x=1;x<img_in.width()-1;x++)
                       {


                             for(std::ptrdiff_t z=0;z<number_of_channels;z++)
                             {

                                 img_out_view_x[y][x][z]=src_it[x+1][z]-src_it[x-1][z];
                             }
                       }

                   }
                   for(std::ptrdiff_t y=0;y<img_in.height();y++)
                   {
                     for(std::ptrdiff_t z=0;z<number_of_channels;z++)
                     {
                       img_out_view_x[y][0][z]=0;
                       img_out_view_x[y][img_in.width()-1][z]=0;
                     }

                   }
               }
                template<typename SrcView,typename Grad_Y_Matrix>
                void calculate_vertical_gradient(SrcView &img_in,Grad_Y_Matrix &img_out_view_y,int number_of_channels)
                {
                    for(std::ptrdiff_t y=1;y<img_in.height()-1;y++)
                    {
                      typename SrcView::x_iterator src1_it = img_in.row_begin(y-1);
                      typename SrcView::x_iterator src2_it = img_in.row_begin(y+1);
                        for(std::ptrdiff_t x=0;x<img_in.width();x++)
                        {
                          for(std::ptrdiff_t z=0;z<number_of_channels;z++)
                          {

                               img_out_view_y[y][x][z]=src2_it[x][z]-src1_it[x][z];
                          }
                        }

                    }
                    for(std::ptrdiff_t x=0;x<img_in.width();x++)
                   {
                     for(std::ptrdiff_t z=0;z<number_of_channels;z++)
                     {
                       img_out_view_y[0][x][z]=0;
                       img_out_view_y[img_in.height()-1][x][z]=0;
                     }

                   }
                }

              struct node
              {
                std::vector<double>arr;
                //float arr[9];
              };



            }

        template<typename SrcView>
        std::vector<double>hog(SrcView&img_in,int number_of_orientations=9,int pixels_per_cell=8,int cells_per_block=2,bool l1_norm=false,bool l2_norm=true)
        {


            BOOST_ASSERT_MSG(number_of_orientations!=0,"Total orientations must be atleast 1");


            int number_of_channels=boost::gil::num_channels<SrcView>::value;

            typedef boost::multi_array<double, 3> array_type;
            array_type img_out_view_x(extents[img_in.height()][img_in.width()][number_of_channels]);
            array_type img_out_view_y(extents[img_in.height()][img_in.width()][number_of_channels]);


            //calculating gradient along horizontal direction
            detail::calculate_horizontal_gradient(img_in,img_out_view_x,number_of_channels);
            detail::calculate_vertical_gradient(img_in,img_out_view_y,number_of_channels);
             


              array_type channel_magnitude_view(extents[img_in.height()][img_in.width()][number_of_channels]);
              array_type channel_angle_view(extents[img_in.height()][img_in.width()][number_of_channels]);
              boost::multi_array<double,2> magnitude_view(extents[img_in.height()][img_in.width()]);
              boost::multi_array<double,2> angle_view(extents[img_in.height()][img_in.width()]);


              detail::carttopolar(img_out_view_x,img_out_view_y,channel_magnitude_view,channel_angle_view);

              int total_rows=img_in.height();
              int total_columns=img_in.width();
              int number_of_cells_horizontal=(total_columns)/pixels_per_cell;
              int number_of_cells_vertical=(total_rows)/pixels_per_cell;

              //for multichannel images orientation of each pixel must be set corresponding to the channel with highest gradient magnitude
              for(std::ptrdiff_t y=0;y<img_in.height();y++)
              {
                for(std::ptrdiff_t x=0;x<img_in.width();x++)
                {
                    magnitude_view[y][x]=*(std::max_element(channel_magnitude_view[y][x].begin(),channel_magnitude_view[y][x].end()));
                    angle_view[y][x]=channel_angle_view[y][x][std::max_element(channel_magnitude_view[y][x].begin(),channel_magnitude_view[y][x].end())-channel_magnitude_view[y][x].begin()];
                }
              }
               double number_of_orientations_per_180=(double)180/number_of_orientations;

              array_type histogram(extents[number_of_cells_vertical][number_of_cells_horizontal][number_of_orientations]);


            //histogram calculation
              detail::calculate_histogram(histogram,magnitude_view,angle_view,number_of_cells_vertical,number_of_cells_horizontal,pixels_per_cell,number_of_orientations_per_180);


              int n_blocks_row=number_of_cells_vertical-cells_per_block+1;
              int n_blocks_col=number_of_cells_horizontal-cells_per_block+1;
              detail::node normalized_histogram[n_blocks_row][n_blocks_col];
              //block normalization
              if(l1_norm)
              {
                    for(std::ptrdiff_t y=0;y<n_blocks_row;y++)
                    {
                         for(std::ptrdiff_t x=0;x<n_blocks_col;x++)
                         {
                           double norm=0;
                            for(std::ptrdiff_t cell_row_index=y;cell_row_index<y+cells_per_block;cell_row_index++)
                            {
                                for(std::ptrdiff_t cell_column_index=x;cell_column_index<x+cells_per_block;cell_column_index++)
                                {



                                      for(auto u:histogram[cell_row_index][cell_column_index])
                                      {
                                        normalized_histogram[y][x].arr.push_back(u);
                                        norm+=std::abs(u);

                                      }


                                }
                            }


                              if(norm!=0)
                            {

                                for(auto &u:normalized_histogram[y][x].arr)
                                {
                                   u=u/norm;
                                }


                             }
                          }

                       }
              }

             else if(l2_norm)
             {
                  for(std::ptrdiff_t y=0;y<n_blocks_row;y++)
                  {
                     for(std::ptrdiff_t x=0;x<n_blocks_col;x++)
                     {
                       double norm_squared=0;
                        for(std::ptrdiff_t cell_row_index=y;cell_row_index<y+cells_per_block;cell_row_index++)
                        {
                            for(std::ptrdiff_t cell_column_index=x;cell_column_index<x+cells_per_block;cell_column_index++)
                            {



                                  for(auto u:histogram[cell_row_index][cell_column_index])
                                  {
                                    normalized_histogram[y][x].arr.push_back(u);
                                    norm_squared+=u*u;

                                  }


                            }
                        }


                          if(norm_squared!=0)
                        {
                          double norm=std::sqrt(norm_squared);
                          for(auto &u:normalized_histogram[y][x].arr)
                          {
                             u=u/norm;
                          }


                       }
                      }

                   }
               }

           //calculating the final feature vector that represents the hog feature for the given image
               std::vector<double>result;
              for(std::ptrdiff_t block_row_index=0;block_row_index<n_blocks_row;block_row_index++)
              {
                  for(std::ptrdiff_t block_column_index=0;block_column_index<n_blocks_col;block_column_index++)
                  {

                     for(auto u:normalized_histogram[block_row_index][block_column_index].arr)
                     {
                        result.push_back(u);
                     }

                  }
              }

              return result;


         }
       }
   }
