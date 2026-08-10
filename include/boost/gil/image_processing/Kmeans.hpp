#ifndef BOOST_GIL_IMAGE_PROCESSING_KMEANS_HPP
#define BOOST_GIL_IMAGE_PROCESSING_KMEANS_HPP
//#include<boost/gil.hpp>

#include<boost/gil/extension/io/jpeg.hpp>
#include<boost/gil/extension/io/png.hpp>
#include<boost/gil/extension/io/tiff.hpp>
#include<bits/stdc++.h>

#include<time.h>

namespace boost{  namespace gil {






  //calculating euclidean distance between a two data points
    float calculate_distance(std::vector<float>&i,std::vector<float>& j)
  {
      float squared_distance=0;
      for(int k=0;k<i.size();k++)
      {
          float diff=(std::max(i[k],j[k])-std::min(i[k],j[k]));
          squared_distance+=(diff*diff);
      }

      return sqrt(squared_distance);
  }

  //we are trying to find out which of the given centroids is closest for each datapoint
namespace detail
{
  //recalculating the centroids to fine tune the clustering
  std::vector<std::vector<float>> calc_centroids(std::vector<int>&getcentroids,std::vector<std::vector<float>>&datapoints,int k)
  {
      std::vector<std::vector<float>>new_centroids(k);
      std::vector<int>cluster_size(k,0);
      for(int i=0;i<k;i++)
      {
          for(int j=0;j<datapoints[0].size();j++)
          {
            new_centroids[i].push_back(0);
          }
      }
      for(int i=0;i<getcentroids.size();i++)
      {
        for(int j=0;j<datapoints[0].size();j++)
        {
         new_centroids[getcentroids[i]][j]+=datapoints[i][j];


         }
         cluster_size[getcentroids[i]]++;
      }
      for(int i=0;i<k;i++)
      {

        for(int j=0;j<datapoints[0].size();j++)
        {
          new_centroids[i][j]/=(float)cluster_size[i];
        }
      }
      return new_centroids;
  }
  void findClosestCentroids(std::vector<std::vector<float>>&datapoints,std::vector<std::vector<float>>&centroids,std::vector<int>&labels,int iterations,std::vector<float>&euclidean_dist)
  {
       for(int j=0;j<datapoints.size();j++)
       {
         std::vector<float>distance;
          if(iterations>0)
          {
            //checking if the distance from the previously assigned cluster centroid is more than the newly generated cluster centroid
             if(calculate_distance(datapoints[j],centroids[labels[j]])<=euclidean_dist[j])
            {
               continue;
             }
          }
          for(auto u:centroids)
          {
            //calculating euclidean distance to find out the nearest centroid
            distance.push_back(calculate_distance(datapoints[j],u));
          }
          labels[j]=(std::min_element(distance.begin(),distance.end())-distance.begin());
          euclidean_dist[j]=distance[labels[j]];
       }

  }
}//detail namespace
  void kmeans(std::vector<std::vector<float>>&src,std::vector<int>&labels,std::vector<std::vector<float>>&centroids,int iterations,int centroid_count)
  {


    BOOST_ASSERT_MSG(centroids.size()>2,"number of clusters must be greater than 2");
    BOOST_ASSERT_MSG(src.size()>2,"number of samples should be atleast 2");
    int data_length=src.size();
    std::vector<std::vector<float>>datapoints(data_length);
    int n_features=src[0].size();

    for(int i=0;i<data_length;i++)
    {
      for(int j=0;j<n_features;j++)
      {
        datapoints[i].push_back(src[i][j]);

      }

    }


    srand(time(0));

    for(int i=0;i<centroid_count;i++)
    {
      //generate random centers for centroid initialization step
       int idx=rand()%data_length;
       for(int j=0;j<n_features;j++)
       {
         centroids[i].push_back(datapoints[idx][j]);
       }


    }
    std::vector<float>euclidean_dist(data_length);

    labels.resize(data_length);
    //the stopping criterion for KMeans here is the number of iterations
    for(int i=0;i<iterations;i++)
    {
      //find the closest centroid to assign the datapoint to the corresponding cluster
       detail::findClosestCentroids(datapoints,centroids,labels,iterations,euclidean_dist);
        //recalculating the centroids based on the data
       centroids=detail::calc_centroids(labels,datapoints,centroid_count);

    }

  }
}
}//boost namespace
#endif
