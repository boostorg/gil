// referenced from https://github.com/scikit-image/scikit-image/blob/main/skimage/feature/tests/test_hog.py

//Copyright (C) 2011, the scikit-image team All rights reserved.

/*Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of skimage nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.*/

#include <boost/gil/image_processing/Hog_implementation.hpp>
#include<boost/gil/image_view.hpp>

std::vector<double>fetch(std::string filename)
{
  std::string line;
  std::ifstream myfile(filename);
  std::vector<double>result;
  if(myfile.is_open())
  {

     while(getline(myfile,line))
     {
       double num=std::stod(line);
       result.push_back(num);
     }
     myfile.close();
  }
  return result;
}
void test_hog_L1_norm()
{
  boost::gil::rgb8_image_t img_color;
  boost::gil::read_image("download.png",img_color,boost::gil::png_tag{});
  boost::gil::rgb8_view_t img_color_view=boost::gil::view(img_color);

  std::vector<double>output=hog(img_color_view,9,8,3,true,false);

  std::vector<double>correct_output=fetch("data-L1-test.txt");
  BOOST_ASSERT_MSG(output.size()==correct_output.size(),"dimensions of feature vectors are not matching");
  for(int i=0;i<output.size();i++)
  {
     BOOST_ASSERT_MSG(std::abs(output[i]-correct_output[i])<=0.0001,"features not matching");
  }
}
void test_hog_L2_norm()
{
  boost::gil::gray8_image_t img_gray;
  boost::gil::read_image("cat.png",img_gray,boost::gil::png_tag{});
  boost::gil::gray8_view_t img_gray_view=boost::gil::view(img_gray);

  std::vector<double>output=hog(img_gray_view,9,8,2,false,true);

  std::vector<double>correct_output=fetch("data-L2-test.txt");
  BOOST_ASSERT_MSG(output.size()==correct_output.size(),"dimensions of feature vectors are not matching");
  for(int i=0;i<output.size();i++)
  {
     BOOST_ASSERT_MSG(std::abs(output[i]-correct_output[i])<=0.0001,"features not matching");
  }
}
void test_hog_image_size_cell_size_mismatch()
{
  boost::gil::gray8_image_t img_gray;
  boost::gil::read_image("camera.png",img_gray,boost::gil::png_tag{});
  boost::gil::gray8_view_t img_gray_view=boost::gil::view(img_gray);
  auto subview=boost::gil::subimage_view(img_gray_view,0,0,200,150);

  std::vector<double>output=boost::gil::hog(subview,9,8,1,true,false);
  BOOST_ASSERT_MSG(output.size()==(9*(150/8)*(200/8)),"feature vectors do not match in size");

}
int main()
{
     test_hog_L1_norm();
     test_hog_L2_norm();
     test_hog_image_size_cell_size_mismatch();
     return boost::report_errors();


}
