#ifndef BOOST_GIL_IMAGE_PROCESSING_KMEANS_HPP
#define BOOST_GIL_IMAGE_PROCESSING_KMEANS_HPP
//#include<boost/gil.hpp>
#include<boost/gil/extension/io/jpeg.hpp>
#include<boost/gil/extension/io/png.hpp>
#include<boost/gil/extension/io/tiff.hpp>
#include<bits/stdc++.h>

#include<time.h>

namespace boost{  namespace gil {




  typedef struct node
  {

     std::vector<float>v;
     int cl;
  }node;

  typedef struct node2
  {

    std::vector<float>v;
    int c;
  }node2;

  //calculating distance between a data point and a  centroid
    float calculate_distance(node i,node2 j)
  {
      float squared_distance=0;
      for(int k=0;k<i.v.size();k++)
      {
          squared_distance+=std::max(i.v[k],j.v[k])-std::min(i.v[k],j.v[k]);
      }
      //float squared_distance=(std::max(j.x,i.x)-std::min(j.x,i.x))*(std::max(j.x,i.x)-std::min(j.x,i.x))+(std::max(j.y,i.y)-std::min(j.y,i.y))*(std::max(j.y,i.y)-std::min(j.y,i.y))+(std::max(j.z,i.z)-std::min(j.z,i.z))*(std::max(j.z,i.z)-std::min(j.z,i.z));
      return sqrt(squared_distance);
  }

  //we are trying to find out which of the given centroids is closest for each datapoint

  std::vector<int> findClosestCentroids(std::vector<node>&datapoints,std::vector<node2>&centroids)
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
  std::vector<node2> calc_centroids(std::vector<int>&getcentroids,std::vector<node>&datapoints,int k)
  {
      std::vector<node2>new_centroids(k);
      for(int i=0;i<k;i++)
      {
          for(int j=0;j<datapoints[0].v.size();j++)
          {
            new_centroids[i].v.push_back(0);
          }
      }
      for(int i=0;i<getcentroids.size();i++)
      {
        for(int j=0;j<datapoints[0].v.size();j++)
        {
         new_centroids[getcentroids[i]].v[j]+=datapoints[i].v[j];


         }
         new_centroids[getcentroids[i]].c++;
      }
      for(int i=0;i<k;i++)
      {

        for(int j=0;j<datapoints[0].v.size();j++)
        {
          new_centroids[i].v[j]/=(float)new_centroids[i].c;
        }
      }
      return new_centroids;
  }
  template<typename SrcView,typename DstView>
  void kmeans(SrcView & src,DstView & dst,int iterations,int centroid_count)
  {

    int h=src.height();
    int w=src.width();
    int counter=0;

    int data_length=h*w;
    std::vector<node>datapoints(h*w);
    int array_size=sizeof(src(0,0))/sizeof(src(0,0)[0]);
    //converting the rgb image into a set of datapoints each having three dimensions red channel,blue channel,green channel
    for(int i=0;i<h;i++)
    {
       for(int j=0;j<w;j++)
       {
          //gil::rgb8_pixel_t b=*const_view(img).at(j,i);

          for(int k=0;k<array_size;k++)
          {
             dst(j,i)[k]=src(j,i)[k]/(float)255;
             datapoints[counter].v.push_back(dst(j,i)[k]);
          }

          counter++;
         }
     }
    std::vector<node2>centroids(centroid_count);
    srand(time(0));
    for(int i=0;i<centroid_count;i++)
    {
      //generate random centers
       int idx=rand()%data_length;
       for(int j=0;j<array_size;j++)
       {
         centroids[i].v.push_back(datapoints[idx].v[j]);
       }


    }
    std::vector<int>getcentroids;
    //the stopping criterion for KMeans here is the number of iterations
    for(int i=0;i<iterations;i++)
    {
       getcentroids=findClosestCentroids(datapoints,centroids);

       centroids=calc_centroids(getcentroids,datapoints,centroid_count);

    }
    for(int i=0;i<data_length;i++)
    {
        for(int j=0;j<array_size;j++)
        {
          datapoints[i].v[j]=centroids[getcentroids[i]].v[j];
        }


    }
   counter=0;
   //transforming the changed datapoints back to the original image shape
    for(int i=0;i<h;i++)
    {
       for(int j=0;j<w;j++)
       {
           for(int k=0;k<array_size;k++)
           {
             dst(j,i)[k]=datapoints[counter].v[k];
           }


          counter++;
         }
     }

  }
}
}//boost namespace
#endif
