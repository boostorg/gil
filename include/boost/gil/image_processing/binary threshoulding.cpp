#include<boost/gil.hpp>
#include<boost/gil/extension/io/jpeg.hpp>
#include<boost/gil/extension/io/png.hpp>
#include<boost/gil/extension/io/tiff.hpp>
#include<bits/stdc++.h>

#include<time.h>

struct node
{
  float x;
  float y;
  float z;
  int cl;
};
struct node2
{
  float x;
  float y;
  float z;
  int c;
};

float calculate_distance(struct node i,struct node2 j)
{
    float squared_distance=(std::max(j.x,i.x)-std::min(j.x,i.x))*(std::max(j.x,i.x)-std::min(j.x,i.x))+(std::max(j.y,i.y)-std::min(j.y,i.y))*(std::max(j.y,i.y)-std::min(j.y,i.y))+(std::max(j.z,i.z)-std::min(j.z,i.z))*(std::max(j.z,i.z)-std::min(j.z,i.z));
    return sqrt(squared_distance);
}
//trying to find out which of the given centroids is closest for each datapoint
std::vector<int> findClosestCentroids(std::vector<struct node>&datapoints,std::vector<struct node2>&centroids)
{
     std::vector<int>assigned_centroid;
     for(auto u:datapoints)
     {
        std::vector<float>distance;
        for(auto j:centroids)
        {
          distance.push_back(calculate_distance(u,j));
        }
        assigned_centroid.push_back(std::min_element(distance.begin(),distance.end())-distance.begin());

     }
     return assigned_centroid;
}
//recalculating the centroids to fine tune the clustering
std::vector<struct node2> calc_centroids(std::vector<int>&getcentroids,std::vector<struct node>&datapoints,int k)
{
    std::vector<struct node2>new_centroids(k,{0,0,0,0});
    for(int i=0;i<getcentroids.size();i++)
    {
       new_centroids[getcentroids[i]].x+=datapoints[i].x;
        new_centroids[getcentroids[i]].y+=datapoints[i].y;
         new_centroids[getcentroids[i]].z+=datapoints[i].z;
         new_centroids[getcentroids[i]].c++;
    }
    for(int i=0;i<k;i++)
    {
      new_centroids[i].x/=(float)new_centroids[i].c;
      new_centroids[i].y/=(float)new_centroids[i].c;
      new_centroids[i].z/=(float)new_centroids[i].c;
    }
    return new_centroids;
}
int main(int argc,char *argv[])
{
    namespace gil=boost::gil;
    gil::rgb8_image_t img;

    gil::read_image(argv[1],img,gil::jpeg_tag());
    gil::rgb32f_image_t img_32(img.dimensions());
    gil::rgb32f_view_t view_32=view(img_32);

    int h=img.height();
    int w=img.width();
    int iterations,k,counter=0;
    std::cout<<"Enter the number of iterations:";
    std::cin>>iterations;
    std::cout<<"\nEnter the value of K";
    std::cin>>k;
    int data_length=h*w;
    std::vector<struct node>datapoints;
    //converting the rgb image into a set of datapoints each having three dimensions red channel,blue channel,green channel
    for(int i=0;i<h;i++)
    {
       for(int j=0;j<w;j++)
       {
          gil::rgb8_pixel_t b=*const_view(img).at(j,i);
          view_32(j,i)[0]=((int)b[0])/((float)255);
          view_32(j,i)[1]=((int)b[1])/((float)255);
          view_32(j,i)[2]=((int)b[2])/((float)255);
          datapoints.push_back({view_32(j,i)[0],view_32(j,i)[1],view_32(j,i)[2],-1});
         }
     }
    std::vector<struct node2>centroids;
    for(int i=0;i<k;i++)
    {
      //generate random centers
       int idx=rand()%data_length;
       centroids.push_back({datapoints[idx].x,datapoints[idx].y,datapoints[idx].z,-1});

    }
    std::vector<int>getcentroids;
    //the stopping criterion for KMeans here is the number of iterations
    for(int i=0;i<iterations;i++)
    {
       getcentroids=findClosestCentroids(datapoints,centroids);

       centroids=calc_centroids(getcentroids,datapoints,k);

    }
    for(int i=0;i<data_length;i++)
    {
        datapoints[i].x=centroids[getcentroids[i]].x;
        datapoints[i].y=centroids[getcentroids[i]].y;
        datapoints[i].z=centroids[getcentroids[i]].z;
    }
    for(int i=0;i<h;i++)
    {
       for(int j=0;j<w;j++)
       {

          view_32(j,i)[0]=datapoints[counter].x;
          view_32(j,i)[1]=datapoints[counter].y;
          view_32(j,i)[2]=datapoints[counter].z;
          counter++;
         }
     }

   gil::write_view("output-kmeans.tif",view_32,gil::tiff_tag());

}
