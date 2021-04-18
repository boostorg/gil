//Copyright (C) 2011, the scikit-image team All rights reserved.

/*
Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of skimage nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.*/


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
                      /*if(i==0 && j==0)
                      {
                        std::cout<<ang<<" "<<angle(j,i)[0]<<std::endl;
                      }*/
                    }
                  }
              }

            template<typename T>
            float cell_hog(const T& magnitude_view,const T& angle_view,double orientation_start,double orientation_end,int pixels_per_cell,int c,int r,int total_columns,
                                          int total_rows,int range_rows_start,int range_rows_stop,int range_columns_start,int range_columns_stop)
            {
               //voting for the orientation bins
                 int cell_column, cell_row, cell_row_index, cell_column_index;
                 double total=0;
                 for(std::ptrdiff_t cell_row=range_rows_start;cell_row<range_rows_stop;cell_row++)
                 {
                    cell_row_index=cell_row+r;
                    if(cell_row_index<0 || cell_row_index>=total_rows)
                    continue;

                    for(std::ptrdiff_t cell_column=range_columns_start;cell_column<range_columns_stop;cell_column++)
                    {
                       cell_column_index=c+cell_column;
                       if(cell_column_index<0 || cell_column_index>=total_columns || angle_view[cell_row_index][cell_column_index]>=orientation_start || angle_view[cell_row_index][cell_column_index]<orientation_end)
                       continue;
                       total+=magnitude_view[cell_row_index][cell_column_index];
                    }
                 }
                 return total/(float)(pixels_per_cell*pixels_per_cell);
            }

          struct node
          {
            std::vector<double>arr;
            //float arr[9];
          };
        }

        template<typename T>
        std::vector<double>hog(T&img_in,int number_of_orientations=9,int pixels_per_cell=8,int cells_per_block=2,bool l1_norm=false,bool l2_norm=true)
        {


            BOOST_ASSERT_MSG(number_of_orientations!=0,"Total orientations must be atleast 1");


            int number_of_channels=boost::gil::num_channels<T>::value;

            typedef boost::multi_array<double, 3> array_type;
            array_type img_out_view_x(extents[img_in.height()][img_in.width()][number_of_channels]);
            array_type img_out_view_y(extents[img_in.height()][img_in.width()][number_of_channels]);
            //calculating gradient along horizontal direction
             for(std::ptrdiff_t y=0;y<img_in.height();y++)
             {
                 for(std::ptrdiff_t x=1;x<img_in.width()-1;x++)
                 {
                   for(std::ptrdiff_t z=0;z<number_of_channels;z++)
                   {
                       double pixel_intensity_after=img_in(x+1,y)[z];
                       double pixel_intensity_before=img_in(x-1,y)[z];
                       //img_out_view_x(j,i)[0]=img_in_view(j+1,i)[0]-img_in_view(j-1,i)[0];
                       img_out_view_x[y][x][z]=pixel_intensity_after-pixel_intensity_before;
                   }
                 }

             }
           //computing gradient along vertical direction
             for(std::ptrdiff_t y=1;y<img_in.height()-1;y++)
             {
                 for(std::ptrdiff_t x=0;x<img_in.width();x++)
                 {
                   for(std::ptrdiff_t z=0;z<number_of_channels;z++)
                   {
                       double pixel_intensity_after=img_in(x,y+1)[z];
                       double pixel_intensity_before=img_in(x,y-1)[z];
                        // img_out_view_y(j,i)[0]=img_in_view(j,i+1)[0]-img_in_view(j,i-1)[0];
                        img_out_view_y[y][x][z]=pixel_intensity_after-pixel_intensity_before;
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

              for(std::ptrdiff_t x=0;x<img_in.width();x++)
             {
               for(std::ptrdiff_t z=0;z<number_of_channels;z++)
               {
                 img_out_view_y[0][x][z]=0;
                 img_out_view_y[img_in.height()-1][x][z]=0;
               }

             }

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
              double histogram[number_of_cells_vertical][number_of_cells_horizontal][number_of_orientations];


              int r_i,c_i;
              int range_columns_stop=pixels_per_cell/2;
              int range_columns_start=-range_columns_stop;
              int r_0=pixels_per_cell/2;
              int c_0=pixels_per_cell/2;
              int cc=pixels_per_cell*number_of_cells_vertical;
              int cr=pixels_per_cell*number_of_cells_horizontal;
              int range_rows_stop=pixels_per_cell/2;
              int range_rows_start=-range_rows_stop;
              double number_of_orientations_per_180=180/(double)number_of_orientations;





           //constructing the histogram for each cell
              for(std::ptrdiff_t orientation_number=0;orientation_number<number_of_orientations;orientation_number++)
              {
                 double orientation_start=number_of_orientations_per_180*((double)(orientation_number+1));
                 double orientation_end=number_of_orientations_per_180*((double)orientation_number);
                 int c=c_0;
                 int r=r_0;
                 r_i=0;
                 c_i=0;

                   while(r<cc)
                   {
                      c_i=0;
                      c=c_0;

                      while(c<cr)
                      {
                        histogram[r_i][c_i][orientation_number]=detail::cell_hog(magnitude_view,angle_view,orientation_start,
                                                      orientation_end,pixels_per_cell,c,r,total_columns,
                                                       total_rows,range_rows_start,range_rows_stop,range_columns_start,
                                                        range_columns_stop);
                          c_i+=1;
                          c+=pixels_per_cell;



                      }
                      r_i+=1;
                      r+=pixels_per_cell;
                   }

              }

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
